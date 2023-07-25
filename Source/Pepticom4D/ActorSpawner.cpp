// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorSpawner.h"
#include "Components/BoxComponent.h"
#include "SpatialDataStruct.h"
#include "Engine/AssetManager.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Misc/FileHelper.h"


// Sets default values
AActorSpawner::AActorSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

}

UDataTable* AActorSpawner::CreateMetadataTableFromStruct(const FString& TableName, UScriptStruct* RowStruct) {
	// Create a new Data Table asset
	UDataTable* DataTable = NewObject<UDataTable>(GetTransientPackage(), FName(*TableName), RF_Transient);
	DataTable->RowStruct = RowStruct;

	// Register the Data Table
	DataTable->AddToRoot();

	// Return the metadata table
	return DataTable;
}

void AActorSpawner::RefreshDataTable(UDataTable* DataTable, FString SourceFilePath) {
	/* Imports data from the source file into the data table */
	// Make sure that we found the data table
	if (DataTable) {
		FString DataTablePath = DataTable->GetPathName();
		// Make sure that the source file exists
		if (FPaths::FileExists(SourceFilePath)) {
			// Read the file into a string and store problems, if any, in an array
			FString FileContent;
			FFileHelper::LoadFileToString(FileContent, *SourceFilePath);
			TArray<FString> problems = TArray<FString>();
			// Check if the file is a CSV or JSON file and parse it accordingly
			if (SourceFilePath.EndsWith(".csv")) {
				problems = DataTable->CreateTableFromCSVString(FileContent);
			} 
			else if (SourceFilePath.EndsWith(".json")) {
				problems = DataTable->CreateTableFromJSONString(FileContent);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("File source for %s is not a CSV or JSON file: %s"), *DataTablePath, *SourceFilePath);
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
			UE_LOG(LogTemp, Warning, TEXT("File source for %s does not exist: %s"), *DataTablePath, *SourceFilePath);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Data table passed to refresh function does not exist"));
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
				FString MetadataStructName = GetStructNameFromFullDatasetName(CurrentFullDatasetName);
				UScriptStruct* MetadataStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *MetadataStructName);
				if (MetadataStruct) {
					MetadataType = Cast<UStruct>(MetadataStruct);
					if (!MetadataType) {
						UE_LOG(LogTemp, Warning, TEXT("Could not cast metadata struct to UStruct: %s"), *MetadataStructName)
					}
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("Could not load class for metadata parsing: %s"), *MetadataStructName);
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
					FName RadiusPropertyName = FName(TEXT("radius"));
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
				SpawnedActorsMap.Add(TPair<AActor*, FTableRowBase*>(Actor, Metadata));
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

FString AActorSpawner::GetCurrentFullDatasetName() { return CurrentFullDatasetName; }

FString AActorSpawner::GetFullDatasetNameFromMainAndSubDatasetNames(FString MainDatasetName, FString SubDatasetName) {
	return MainDatasetName + TEXT("_") + SubDatasetName;
}

FString AActorSpawner::GetStructNameFromFullDatasetName(FString FullDatasetName) {
	// Assumes an input of the form "main_dataset_name_sub_dataset_name, converts to MainDatasetNameSubDatasetNameTempStruct"
	FString PascalCaseString;
	bool bNextIsUpper = true;
	for (TCHAR Char : FullDatasetName)
	{
		if (Char == '_')
		{
			bNextIsUpper = true;
		}
		else
		{
			PascalCaseString.AppendChar(bNextIsUpper ? FChar::ToUpper(Char) : Char);
			bNextIsUpper = false;
		}
	}
	FString StructName = PascalCaseString + "TempStruct";
	return StructName;
}

void AActorSpawner::ProcessConfig(FString ConfigVarName) {
	// Get the config file path
	FString ConfigFilePath;
	GConfig->GetString(TEXT("Data"), *ConfigVarName, ConfigFilePath, GGameIni);
	// Read the config file, which is a JSON file. 
	FString JsonRaw;
	if (!FFileHelper::LoadFileToString(JsonRaw, *ConfigFilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load config file: %s"), *ConfigFilePath);
		return;
	}
	// Parse the JSON string into a JSON object
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonRaw);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON from config file."));
		return;
	}
	// Access the datasets field of the config file
	const TSharedPtr<FJsonObject>* DatasetsObjectPtr;
	if (!JsonObject->TryGetObjectField("datasets", DatasetsObjectPtr))
	{
		UE_LOG(LogTemp, Error, TEXT("Config file JSON does not contain 'datasets' field."));
		return;
	}

	// Process the JSON data and initialize variables
	bool bFirstMainDataset = true;
	// Iterate over all main datasets
	for (const auto& MainPair : (*DatasetsObjectPtr)->Values)
	{
		FString MainDatasetName = MainPair.Key;
		// Set the default main-dataset to be the first one
		if (bFirstMainDataset)
		{
			CurrentMainDatasetName = MainDatasetName;
			bFirstMainDataset = false;
		}
		// Get the subsets object and make sure that it is an object that we can iterate over
		const TSharedPtr<FJsonObject>* SubsetsObjectPtr;
		if (!MainPair.Value->AsObject()->TryGetObjectField("subsets", SubsetsObjectPtr))
		{
			UE_LOG(LogTemp, Error, TEXT("Config file JSON does not contain 'subsets' field."));
			continue; // or handle the error
		}
		bool bFirstSubDataset = true;
		// Iterate over all sub datasets
		for (const auto& SubPair : (*SubsetsObjectPtr)->Values)
		{
			// Get the sub-dataset name and object
			FString SubDatasetName = SubPair.Key;
			TSharedPtr<FJsonObject> SubDatasetObj = SubPair.Value->AsObject();
			// Set the default sub-dataset to be the first one
			if (bFirstSubDataset)
			{
				CurrentSubDatasetName = SubDatasetName;
				CurrentFullDatasetName = GetFullDatasetNameFromMainAndSubDatasetNames(CurrentMainDatasetName, CurrentSubDatasetName);
				bFirstSubDataset = false;
			}
			// Create a map of file paths
			TMap<FString, FString> FilePathsMap = TMap<FString, FString>();
			// Get the file paths and populate the array
			FString DataSource;
			if (SubDatasetObj->TryGetStringField("data_source", DataSource))
			{
				TPair<FString, FString> Pair = TPair<FString, FString>(TEXT("SpatialDataFilePath"), DataSource);
				FilePathsMap.Add(Pair);
			}
			FString MetadataSource;
			if (SubDatasetObj->TryGetStringField("metadata_source", MetadataSource))
			{
				TPair<FString, FString> Pair = TPair<FString, FString>(TEXT("SpatialMetadataFilePath"), MetadataSource);
				FilePathsMap.Add(Pair);
			}
			FString POISource;
			if (SubDatasetObj->TryGetStringField("poi_source", POISource))
			{
				TPair<FString, FString> Pair = TPair<FString, FString>(TEXT("POIFilePath"), POISource);
				FilePathsMap.Add(Pair);
			}
			// Add the file paths to the map
			FullDatasetNameToFilePathsMap.Add(GetFullDatasetNameFromMainAndSubDatasetNames(MainDatasetName, SubDatasetName), FilePathsMap);
		}
	}
}

void AActorSpawner::ForceRefresh() {
	ProcessConfig(FString(TEXT("DataConfigFilePath")));
	// Get the spatial data table
	FString SpatialDataTablePath = FString(TEXT("/Game/SpatialDataTable.SpatialDataTable"));
	UDataTable* SpatialDataTable = LoadObject<UDataTable>(NULL, *SpatialDataTablePath, NULL, LOAD_None, NULL);
	RefreshDataTable(SpatialDataTable, FullDatasetNameToFilePathsMap[CurrentFullDatasetName]["SpatialDataFilePath"]);
	// Create the spatial metadata table (exists only during runtime)
	FString SpatialMetadataTableName = FString(TEXT("SpatialMetadataTable"));
	FString MetadataStructName = GetStructNameFromFullDatasetName(CurrentFullDatasetName);
	UScriptStruct* SpatialMetadataStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *MetadataStructName);
	SpatialMetadataTable = CreateMetadataTableFromStruct(SpatialMetadataTableName, SpatialMetadataStruct);
	RefreshDataTable(SpatialMetadataTable, FullDatasetNameToFilePathsMap[CurrentFullDatasetName]["SpatialMetadataFilePath"]);
	DestroySpawnedActors();
	EnqueueSpawningActorsFromDataTable();
	// TODO: We may need to create an event hook here to generate configuration structs/view-perspective stucts, etc. 
	// and reload those data tables IF data has changed since last loaded.
	// Best way to check for changes would be to store some kind of hash of parts of the data, so that the hash is not too large, and compare that to the hash of the current data.
	UDataTable* POIDataTable = LoadObject<UDataTable>(NULL, *FString(TEXT("/Game/POIDataTable.POIDataTable")), NULL, LOAD_None, NULL);
	RefreshDataTable(POIDataTable, FullDatasetNameToFilePathsMap[CurrentFullDatasetName]["POIFilePath"]);
}

FTableRowBase& AActorSpawner::GetMetadataFromActor(AActor* Actor) {
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

