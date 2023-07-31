// Fill out your copyright notice in the Description page of Project Settings.

#include "DataManager.h"


// Sets default values
ADataManager::ADataManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ADataManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADataManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADataManager::ProcessConfig(FString ConfigVarName) {
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
	FString CurrentMainDatasetName;
	FString CurrentSubDatasetName;
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

FString ADataManager::GetFullDatasetNameFromMainAndSubDatasetNames(FString MainDatasetName, FString SubDatasetName) {
	return MainDatasetName + TEXT("_") + SubDatasetName;
}

FString ADataManager::GetStructNameFromFullDatasetName(FString FullDatasetName) {
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

UDataTable* ADataManager::CreateMetadataTableFromStruct(const FString& TableName, UScriptStruct* RowStruct) {
	// Create a new Data Table asset
	UDataTable* DataTable = NewObject<UDataTable>(GetTransientPackage(), FName(*TableName), RF_Transient);
	DataTable->RowStruct = RowStruct;

	// Register the Data Table
	DataTable->AddToRoot();

	// Return the metadata table
	return DataTable;
}

void ADataManager::RefreshDataTable(UDataTable* DataTable, FString SourceFilePath) {
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

FTableRowBase& ADataManager::GetMetadataFromActor(AActor* Actor) {
	return *(SpawnedActorsMap.FindRef(Actor));
}

