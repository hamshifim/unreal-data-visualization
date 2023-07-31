// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorSpawner.h"
#include "Components/BoxComponent.h"
#include "SpatialDataStruct.h"
#include "Engine/AssetManager.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AActorSpawner::AActorSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

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
				FTableRowBase* SpatialMetadataRow = SpatialMetadataTable->FindRow<FTableRowBase>(SpatialMetadataRowName, TEXT(""));
				// Make sure that we found the rows
				if (SpatialDataRow && SpatialMetadataRow) {
					// Spawn the actor
					FVector SpawnLocation = FVector(SpatialDataRow->X, SpatialDataRow->Y, SpatialDataRow->Z);
					TPair<FTableRowBase*, FVector> MetadataLocationPair = TPair<FTableRowBase*, FVector>(SpatialMetadataRow, SpawnLocation);
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
				TPair<FTableRowBase*, FVector> MetadataLocationPair;
				// Get the next spawn location
				ActorSpawnMetadataLocationPairQueue.Dequeue(MetadataLocationPair);
				FTableRowBase* Metadata = MetadataLocationPair.Key;
				// Get the metadata type for property examination
				UStruct* MetadataType = FTableRowBase::StaticStruct();
				FString MetadataStructName = DataManager->GetStructNameFromFullDatasetName(DataManager->CurrentFullDatasetName);
				UScriptStruct* MetadataStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *MetadataStructName);
				if (MetadataStruct) {
					MetadataType = Cast<UStruct>(MetadataStruct);
					if (!MetadataType) {
						UE_LOG(LogTemp, Warning, TEXT("Could not cast metadata struct to UStruct: %s"), *MetadataStructName)
							return;
					}
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("Could not load class for metadata parsing: %s"), *MetadataStructName);
					return;
				}
				FVector SpawnLocation = MetadataLocationPair.Value;
				// Spawn the actor
				// Set actor rotation to be the same as the rotation of the spawner
				FRotator SpawnRotation = GetActorRotation();
				// Spawn the actor
				AActorToSpawn* Actor = GetWorld()->SpawnActor<AActorToSpawn>(SpawnLocation, SpawnRotation);
				if (Actor) {
					// Check if the metadata has a color property and set the actor's color to that color
					FName ColorPropertyName = FName(TEXT("color"));
					FProperty* ColorProperty = MetadataType->FindPropertyByName(ColorPropertyName);
					if (ColorProperty) {
						FStrProperty* StrProp = CastField<FStrProperty>(ColorProperty);
						FString ColorHex = StrProp->GetPropertyValue_InContainer(Metadata);
						Actor->ChangeColor(ColorHex);
					}
					// Check if the metadata has a radius property and set the actor's size to that radius
					FName RadiusPropertyName = FName(TEXT("size"));
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
				DataManager->SpawnedActorsMap.Add(TPair<AActor*, FTableRowBase*>(Actor, Metadata));
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
	for (auto& Elem : DataManager->SpawnedActorsMap) {
		// Destroy the actor
		Elem.Key->Destroy();
	}
	DataManager->SpawnedActorsMap.Empty();
}

void AActorSpawner::ForceRefresh() {
	// Get the spatial data table
	FString SpatialDataTablePath = FString(TEXT("/Game/SpatialDataTable.SpatialDataTable"));
	UDataTable* SpatialDataTable = LoadObject<UDataTable>(NULL, *SpatialDataTablePath, NULL, LOAD_None, NULL);
	DataManager->RefreshDataTable(SpatialDataTable, DataManager->FullDatasetNameToFilePathsMap[DataManager->CurrentFullDatasetName]["SpatialDataFilePath"]);
	// Create the spatial metadata table (exists only during runtime)
	FString SpatialMetadataTableName = FString(TEXT("SpatialMetadataTable"));
	FString MetadataStructName = DataManager->GetStructNameFromFullDatasetName(DataManager->CurrentFullDatasetName);
	UScriptStruct* SpatialMetadataStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *MetadataStructName);
	SpatialMetadataTable = DataManager->CreateMetadataTableFromStruct(SpatialMetadataTableName, SpatialMetadataStruct);
	DataManager->RefreshDataTable(SpatialMetadataTable, DataManager->FullDatasetNameToFilePathsMap[DataManager->CurrentFullDatasetName]["SpatialMetadataFilePath"]);
	DestroySpawnedActors();
	EnqueueSpawningActorsFromDataTable();
	// TODO: We may need to create an event hook here to generate configuration structs/view-perspective stucts, etc. 
	// and reload those data tables IF data has changed since last loaded.
	// Best way to check for changes would be to store some kind of hash of parts of the data, so that the hash is not too large, and compare that to the hash of the current data.
	UDataTable* POIDataTable = LoadObject<UDataTable>(NULL, *FString(TEXT("/Game/POIDataTable.POIDataTable")), NULL, LOAD_None, NULL);
	DataManager->RefreshDataTable(POIDataTable, DataManager->FullDatasetNameToFilePathsMap[DataManager->CurrentFullDatasetName]["POIFilePath"]);
}

// Called when the game starts or when spawned
void AActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Run this 
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this]() {
		// Get and store a reference to the UI manager
		UIManager = Cast<AUIManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AUIManager::StaticClass()));
		if (!UIManager) {
			UE_LOG(LogTemp, Error, TEXT("UI Manager not found in actor spawner"));
			return;
		}
		// Get and store a reference to the data manager
		DataManager = Cast<ADataManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADataManager::StaticClass()));
		if (!DataManager) {
			UE_LOG(LogTemp, Error, TEXT("Data Manager not found in actor spawner"));
			return;
		}

		// Initial dataset setup
		DataManager->ProcessConfig(FString(TEXT("DataConfigFilePath")));
		UIManager->RefreshDataSelectorWidget();

		ForceRefresh();
	});

	// Set the timer to call the lambda function after a delay of 0.1 seconds
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 0.1f, false);
}

// Called every frame
void AActorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Spawn actors from the queue if the queue is not empty
	SpawnActorsFromQueue();
}

