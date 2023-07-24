// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorSpawner.h"
#include "Components/BoxComponent.h"
#include "SpatialDataStruct.h"
#include "Engine/AssetManager.h"


// Sets default values
AActorSpawner::AActorSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

}

void AActorSpawner::CreateMetadataTableFromStruct(const FString& TableName, UScriptStruct* RowStruct) {
	// Create a new Data Table asset
	UDataTable* DataTable = NewObject<UDataTable>(GetTransientPackage(), FName(*TableName), RF_Transient);
	DataTable->RowStruct = RowStruct;

	// Register the Data Table
	DataTable->AddToRoot();

	// Save the metadata table
	SpatialMetadataTable = DataTable;
}

void AActorSpawner::RefreshDataTable(FString DataTablePath, FString SourceFileName) {
	/* Imports data from the source file into the data table */
	// Get the data table
	UDataTable* DataTable = LoadObject<UDataTable>(NULL, *DataTablePath, NULL, LOAD_None, NULL);
	// Make sure that we found the data table
	if (DataTable) {
		// Get the source file
		FString SourceFile;
		GConfig->GetString(TEXT("Data"), *SourceFileName, SourceFile, GGameIni);
		// Make sure that the source file exists
		if (FPaths::FileExists(SourceFile)) {
			// Read the file into a string and store problems, if any, in an array
			FString FileContent;
			FFileHelper::LoadFileToString(FileContent, *SourceFile);
			TArray<FString> problems = TArray<FString>();
			// Check if the file is a CSV or JSON file and parse it accordingly
			if (SourceFile.EndsWith(".csv")) {
				problems = DataTable->CreateTableFromCSVString(FileContent);
			} 
			else if (SourceFile.EndsWith(".json")) {
				problems = DataTable->CreateTableFromJSONString(FileContent);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("File source for %s is not a CSV or JSON file: %s"), *DataTablePath, *SourceFile);
				return;
			}
			// Make sure that there were no problems
			if (problems.Num() > 0) {
				UE_LOG(LogTemp, Warning, TEXT("There were problems importing %s"), *DataTablePath);
				for (auto& problem : problems) {
					UE_LOG(LogTemp, Warning, TEXT("%s"), *problem);
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Data table %s imported successfully"), *DataTablePath);
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("File source for %s does not exist: %s"), *DataTablePath, *SourceFile);
		}
	}
}

void AActorSpawner::EnqueueSpawningActorsFromDataTable() {
	/* Using data from the appropriate data table, actors are spawned in the world */
	// Get the spatial data table
	FString SpatialDataTablePath = FString(TEXT("/Game/SpatialDataTable.SpatialDataTable"));
	UDataTable* SpatialDataTable = LoadObject<UDataTable>(NULL, *SpatialDataTablePath, NULL, LOAD_None, NULL);
	// Make sure that we found the data table
	if (SpatialDataTable) {
		// Get all of the row names
		TArray<FName> SpatialDataRowNames = SpatialDataTable->GetRowNames();
		// Get the total number of rows
		int32 NumSpatialDataRows = SpatialDataRowNames.Num();

		// Make sure that we found the metadata table
		if (SpatialMetadataTable) {
			// Get all of the row names
			TArray<FName> SpatialMetadataRowNames = SpatialMetadataTable->GetRowNames();
			// Get the total number of rows
			int32 NumSpatialMetadataRows = SpatialMetadataRowNames.Num();

			// Make sure that there are the same number of rows in both tables
			if (NumSpatialDataRows != NumSpatialMetadataRows) {
				UE_LOG(LogTemp, Error, TEXT("The number of rows in the spatial data table (%d) does not match the number of rows in the spatial metadata table (%d)"), NumSpatialDataRows, NumSpatialMetadataRows);
				return;
			}

			// Iterate through the rows while keeping track of the row index
			for (int32 i = 0; i < NumSpatialDataRows; ++i) {
				// Get the spatial data row name
				FName SpatialDataRowName = SpatialDataRowNames[i];
				FSpatialDataStruct* SpatialDataRow = SpatialDataTable->FindRow<FSpatialDataStruct>(SpatialDataRowName, TEXT(""));
				// Get the corresponding metadata row
				FName SpatialMetadataRowName = SpatialMetadataRowNames[i];
				FSpatialMetadataStruct* SpatialMetadataRow = SpatialMetadataTable->FindRow<FSpatialMetadataStruct>(SpatialMetadataRowName, TEXT(""));
				// Make sure that we found the rows
				if (SpatialDataRow && SpatialMetadataRow) {
					// Spawn the actor
					FVector SpawnLocation = FVector(SpatialDataRow->X, SpatialDataRow->Y, SpatialDataRow->Z);
					TPair<FSpatialMetadataStruct*, FVector> MetadataLocationPair = TPair<FSpatialMetadataStruct*, FVector>(SpatialMetadataRow, SpawnLocation);
					ActorSpawnMetadataLocationPairQueue.Enqueue(MetadataLocationPair);
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("Could not find one or more rows in the spatial data table or spatial metadata table"));
				}
			}

		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Could not find spatial metadata table"));
			return;
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Could not find spatial data table: %s"), *SpatialDataTablePath);
		return;
	}
}

void AActorSpawner::SpawnActorsFromQueue() {
	/* Spawns actors from the queue */
	if (!ActorSpawnMetadataLocationPairQueue.IsEmpty()) {
		for (int32 i = 0; i < SpawnActorsPerTick; ++i) {
			// Make sure that there are actors to spawn
			if (!ActorSpawnMetadataLocationPairQueue.IsEmpty()) {
				TPair<FSpatialMetadataStruct*, FVector> MetadataLocationPair;
				// Get the next spawn location
				ActorSpawnMetadataLocationPairQueue.Dequeue(MetadataLocationPair);
				FSpatialMetadataStruct* Metadata = MetadataLocationPair.Key;
				UStruct* MetadataType = FSpatialMetadataStruct::StaticStruct();
				FVector SpawnLocation = MetadataLocationPair.Value;
				// Spawn the actor
				// Set actor rotation to be the same as the rotation of the spawner
				FRotator SpawnRotation = GetActorRotation();
				// Spawn the actor
				AActorToSpawn* Actor = GetWorld()->SpawnActor<AActorToSpawn>(SpawnLocation, SpawnRotation);
				if (Actor) {
					// Check if the metadata has a COLOR property and set the actor's color to that color
					FName ColorPropertyName = FName(TEXT("COLOR"));
					FProperty* ColorProperty = MetadataType->FindPropertyByName(ColorPropertyName);
					if (ColorProperty) {
						FStrProperty* StrProp = CastField<FStrProperty>(ColorProperty);
						FString ColorHex = StrProp->GetPropertyValue_InContainer(Metadata);
						Actor->ChangeColor(ColorHex);
					}
					// Check if the metadata has a RADIUS property and set the actor's size to that radius
					FName RadiusPropertyName = FName(TEXT("RADIUS"));
					FProperty* RadiusProperty = MetadataType->FindPropertyByName(RadiusPropertyName);
					if (RadiusProperty) {
						FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(RadiusProperty);
						float Radius = DoubleProp->GetPropertyValue_InContainer(Metadata);
						Actor->ChangeScale(Radius);
					}
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("Actor not ready yet"));
				}
				// Add the spawned actor to the array of spawned actors
				SpawnedActorsMap.Add(TPair<AActor*, FSpatialMetadataStruct*>(Actor, Metadata));
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Spawned all actors"));
				break;
			}
		}
	}
}

void AActorSpawner::DestroySpawnedActors() {
	/* Destroys all actors which have been spawned by this actor spawner */
	for (auto& Elem : SpawnedActorsMap) {
		// Destroy the actor
		Elem.Key->Destroy();
	}
	SpawnedActorsMap.Empty();
}

void AActorSpawner::ForceRefresh() {
	RefreshDataTable(FString(TEXT("/Game/SpatialDataTable.SpatialDataTable")), FString(TEXT("SpatialDataFilePath")));
	RefreshDataTable(FString(TEXT("/Game/SpatialMetadataTable.SpatialMetadataTable")), FString(TEXT("SpatialMetadataFilePath")));
	DestroySpawnedActors();
	EnqueueSpawningActorsFromDataTable();
	// TODO: We may need to create an event hook here to generate configuration structs/view-perspective stucts, etc. 
	// and reload those data tables IF data has changed since last loaded.
	// Best way to check for changes would be to store some kind of hash of parts of the data, so that the hash is not too large, and compare that to the hash of the current data.
	RefreshDataTable(FString(TEXT("/Game/POIDataTable.POIDataTable")), FString(TEXT("POIDataFilePath")));
}

FSpatialMetadataStruct& AActorSpawner::GetMetadataFromActor(AActor* Actor) {
	return *(SpawnedActorsMap.FindRef(Actor));
}

// Called when the game starts or when spawned
void AActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	ForceRefresh();
}

// Called every frame
void AActorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Spawn actors from the queue if the queue is not empty
	SpawnActorsFromQueue();
}

