// Fill out your copyright notice in the Description page of Project Settings.

#include "DataManager.h"


// Sets default values
ADataManager::ADataManager()
	: Super() // Initialize the base class constructor
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

	UE_LOG(LogTemp, Warning, TEXT("Data manager initialized"));
}

// Called every frame
void ADataManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADataManager::ProcessConfig(FString ConfigVarName)
{
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

	// Get the default view name
	FString DefaultViewName;
	if (!JsonObject->TryGetStringField("default_view", DefaultViewName))
	{
		UE_LOG(LogTemp, Error, TEXT("Config file JSON does not contain 'default_view' field."));
		return;
	}

	// Get the views object and make sure that it is an object that we can iterate over
	const TSharedPtr<FJsonObject>* ViewsObjectPtr;
	if (!JsonObject->TryGetObjectField("views", ViewsObjectPtr))
	{
		UE_LOG(LogTemp, Error, TEXT("Config file JSON does not contain 'views' field."));
		return;
	}

	// Iterate over all views
	for (const auto& ViewPair : (*ViewsObjectPtr)->Values)
	{
		FString ViewName = ViewPair.Key;
		TSharedPtr<FJsonObject> ViewObj = ViewPair.Value->AsObject();

		// Set the default view
		if (ViewName.Equals(DefaultViewName))
		{
			CurrentViewName = ViewName;
		}

		// Get the color maps for this view and make sure that we can iterate over them
		const TSharedPtr<FJsonObject>* ColorMapsObjectPtr;
		if (!ViewObj->TryGetObjectField("color_maps", ColorMapsObjectPtr))
		{
			// Iterate through the property names 
			TMap<FString, TMap<FString, FColor>> PropertyNameAndValueColorMap = TMap<FString, TMap<FString, FColor>>();
			for (const auto& ColorMapPair : (*ColorMapsObjectPtr)->Values)
			{
				FString PropertyName = ColorMapPair.Key;
				TSharedPtr<FJsonObject> ColorMapObj = ColorMapPair.Value->AsObject();
				// Iterate through the color map objects
				TMap<FString, FColor> ValueColorMap = TMap<FString, FColor>();
				for (const auto& ValueColorPair : ColorMapObj->Values)
				{
					FString Value = ValueColorPair.Key;
					FString ColorString = ValueColorPair.Value->AsString();
					// Get the color as an FColor
					FColor Color = FColor::FromHex(ColorString);
					// Add the value and color to the value color map
					ValueColorMap.Add(Value, Color);
				}
				// Add the property name and value color map to the map of property name and value color maps
				PropertyNameAndValueColorMap.Add(PropertyName, ValueColorMap);
			}
			// Add the view name and property name and value color map to the map
			ColorMap.Add(ViewName, PropertyNameAndValueColorMap);
		}
		else
		{
			UE_LOG(LogTemp, Warning,
			       TEXT(
				       "Config file JSON does not contain 'color_maps' field in at least one 'views' -> 'view' object (%s)."
			       ), *ViewName);
		}

		// Get the data types for this view and make sure that we can iterate over them
		const TArray<TSharedPtr<FJsonValue>>* DataTypesArray;
		if (!ViewObj->TryGetArrayField("data_types", DataTypesArray))
		{
			UE_LOG(LogTemp, Error,
			       TEXT("Config file JSON does not contain 'data_types' field in the 'views' -> 'view' object."));
			continue; // or handle the error
		}
		// Iterate over all data types
		TArray<FString> DataTypes;
		for (const auto& DataTypeValue : *DataTypesArray)
		{
			FString DataTypeName = DataTypeValue->AsString();
			// Add the data type name to the array
			DataTypes.Add(DataTypeName);
		}
		// Add the view name and data types to the map
		ViewNameToDataTypesMap.Add(ViewName, DataTypes);

		// Get the boundary points for this view
		const TSharedPtr<FJsonObject>* BoundaryPointsObjectPtr;
		if (!ViewObj->TryGetObjectField("boundaries", BoundaryPointsObjectPtr))
		{
			UE_LOG(LogTemp, Error,
			       TEXT("Config file JSON missing 'boundaries' field in the 'views' -> 'view' object."));
			continue; // or handle the error
		}
		// Add a name field to the boundary points object so that Unreal can read it into a DataTable
		(*BoundaryPointsObjectPtr)->SetStringField("name", "boundary_points");
		// Wrap the boundary points object inside of a new array
		TArray<TSharedPtr<FJsonValue>> BoundaryPointsArray;
		BoundaryPointsArray.Add(MakeShareable(new FJsonValueObject(*BoundaryPointsObjectPtr)));
		// Store the boundary points object as a string
		FString BoundaryPointsString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BoundaryPointsString);
		FJsonSerializer::Serialize(BoundaryPointsArray, Writer);
		// Add the view name and boundary points to the map
		ViewNameToBoundaryPointsMap.Add(ViewName, BoundaryPointsString);
	}

	// Access the data_types field of the config file
	const TSharedPtr<FJsonObject>* DataTypesObjectPtr;
	if (!JsonObject->TryGetObjectField("data_types", DataTypesObjectPtr))
	{
		UE_LOG(LogTemp, Error, TEXT("Config file JSON does not contain 'data_types' field."));
		return;
	}

	// Iterate over all main datasets
	for (const auto& DataTypePair : (*DataTypesObjectPtr)->Values)
	{
		FString DataTypeName = DataTypePair.Key;
		TSharedPtr<FJsonObject> DataTypeObj = DataTypePair.Value->AsObject();

		// Get the subsets object and make sure that it is an object that we can iterate over
		const TSharedPtr<FJsonObject>* TablesObjectPtr;
		if (!DataTypeObj->TryGetObjectField("tables", TablesObjectPtr))
		{
			UE_LOG(LogTemp, Error, TEXT("Config file JSON does not contain 'tables' field."));
			continue; // or handle the error
		}
		// Iterate over all sub datasets
		TArray<FString> TableNames;
		for (const auto& TablePair : (*TablesObjectPtr)->Values)
		{
			// Get the table name and object
			FString TableName = TablePair.Key;
			TSharedPtr<FJsonObject> TableObj = TablePair.Value->AsObject();
			// Create a map of file paths
			TMap<FString, FString> FilePathsMap = TMap<FString, FString>();
			// Get the file paths and populate the array
			FString DataSource;
			if (DataTypeObj->TryGetStringField("data_source", DataSource))
			{
				TPair<FString, FString> Pair = TPair<FString, FString>(TEXT("SpatialDataFilePath"), DataSource);
				FilePathsMap.Add(Pair);
			}
			else
			{
				UE_LOG(LogTemp, Error,
				       TEXT(
					       "Config file JSON does not contain 'data_source' field in the 'data_types' -> 'data_type' object."
				       ));
				continue;
			}

			FString MetadataSource;
			if (TableObj->TryGetStringField("data_source", MetadataSource))
			{
				TPair<FString, FString> Pair = TPair<FString, FString>(TEXT("SpatialMetadataFilePath"), MetadataSource);
				FilePathsMap.Add(Pair);
			}
			else
			{
				UE_LOG(LogTemp, Error,
				       TEXT(
					       "Config file JSON does not contain 'data_source' field in at least one 'data_types' -> 'data_type' -> 'tables' -> 'table' object."
				       ));
				continue;
			}

			// Add the file paths to the map
			FString FullTableName = GetFullTableName(DataTypeName, TableName);
			TableFilePathMap.Add(FullTableName, FilePathsMap);
			// Add the table name to the array of table names
			TableNames.Add(TableName);
			// Create a spatial metadata table for each data type, assuming a default metadata struct from the current table
			FString MetadataStructName = StructNameFromFullTableName(FullTableName);
			FString SpatialMetadataTableName = MetadataStructName + "DataTable";
			UScriptStruct* SpatialMetadataScriptStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *MetadataStructName);
			UDataTable* SpatialMetadataTable = CreateMetadataTableFromStruct(
				SpatialMetadataTableName, SpatialMetadataScriptStruct);
			// Add the spatial metadata table to the map
			FullDatasetNameToSpatialMetadataTableMap.Add(FullTableName, SpatialMetadataTable);
			// Store the metadata struct in the map of full dataset names to metadata structs
			UStruct* SpatialMetadataStruct = Cast<UStruct>(SpatialMetadataScriptStruct);
			FullTableNameToMetadataStructMap.Add(FullTableName, SpatialMetadataStruct);
		}
		// Store the table names in the map
		DataTypeToSubDatasetNamesMap.Add(DataTypeName, TableNames);

		// Set the default table to be the first one - set in the default_table property
		FString DefaultTableName;
		if (DataTypeObj->TryGetStringField("default_table", DefaultTableName))
		{
			FString FullDatasetName = GetFullTableName(DataTypeName, DefaultTableName);
			// Check if this main dataset is within the current view; if so, add the full dataset name to the list of current full dataset names
			if (ViewNameToDataTypesMap[CurrentViewName].Contains(DataTypeName))
			{
				CurrentFullTableNames.Add(FullDatasetName);
			}
			// Map the current main data type name to its current table name
			CurrentDataTypeNameToTableNameMap.Add(DataTypeName, DefaultTableName);
		}
		else
		{
			UE_LOG(LogTemp, Error,
			       TEXT("Config file JSON does not contain 'default_table' field for at least one data type."));
			continue;
		}
	}
}

FString ADataManager::GetFullTableName(FString DataTypeName, FString TableName)
{
	return DataTypeName + TEXT("_") + TableName;
}

FString ADataManager::StructNameFromFullTableName(FString FullTableName)
{
	// Assumes an input of the form "dat_type_name_table_name, converts to DataTypeNameTableNameTempStruct"
	FString PascalCaseString;
	bool bNextIsUpper = true;
	for (const TCHAR Char : FullTableName)
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

UDataTable* ADataManager::CreateMetadataTableFromStruct(const FString& TableName, UScriptStruct* RowStruct)
{
	// Create a new Data Table asset
	UDataTable* DataTable = NewObject<UDataTable>(GetTransientPackage(), FName(*TableName), RF_Transient);
	DataTable->RowStruct = RowStruct;

	// Register the Data Table
	DataTable->AddToRoot();

	// Return the metadata table
	return DataTable;
}

FString ADataManager::GetContentFromSourceFile(FString SourceFilePath)
{
	FString FileContent = TEXT("");
	// Make sure that the source file exists
	if (FPaths::FileExists(SourceFilePath))
	{
		// Read the file into a string
		FFileHelper::LoadFileToString(FileContent, *SourceFilePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Source file does not exist: %s"), *SourceFilePath);
	}

	return FileContent;
}

TArray<FString> ADataManager::GetChunkedContentFromCSVSourceFile(FString SourceFilePath, int ChunkSize)
{
	// Validate the file extension
	FString FileExtension = GetFileTypeFromSourceFile(SourceFilePath);
	if (!FileExtension.Equals("CSV"))
	{
		UE_LOG(LogTemp, Error, TEXT("Source file passed to get chunked content is not a CSV file: %s"),
		       *SourceFilePath);
		return TArray<FString>();
	}
	FString FileContent = GetContentFromSourceFile(SourceFilePath);
	TArray<FString> Chunks;
	TArray<FString> Lines;
	// Split the file content into lines
	FileContent.ParseIntoArrayLines(Lines);
	FString CurrentChunk;
	// Loop over the lines
	for (int32 i = 0; i < Lines.Num(); i++)
	{
		if (i % ChunkSize == 0)
		{
			// If we have read ChunkSize lines, add the current chunk to the chunks array
			if (!CurrentChunk.IsEmpty())
			{
				Chunks.Add(CurrentChunk);
			}
			// Start a new chunk with the column row
			CurrentChunk = Lines[0] + TEXT("\n");
		}
		if (i == 0)
		{
			// Skip the column row
			continue;
		}
		// Add the line to the current chunk
		CurrentChunk += Lines[i] + TEXT("\n");
	}
	// Add the last chunk if it is not empty
	if (!CurrentChunk.IsEmpty() && CurrentChunk != Lines[0] + TEXT("\n"))
	{
		Chunks.Add(CurrentChunk);
	}
	// Return the chunks
	return Chunks;
}

FString ADataManager::GetFileTypeFromSourceFile(FString SourceFilePath)
{
	FString FileExtension = FPaths::GetExtension(SourceFilePath).ToUpper();
	return FileExtension;
}

void ADataManager::AddDataToDataTableFromSource(UDataTable* DataTable, FString& SourceFileContent,
                                                FString& SourceFileType)
{
	// Make sure that we found the data table
	if (DataTable)
	{
		// Create a temporary data table to hold the new data and copy the row struct from the original data table
		UDataTable* TempDataTable = NewObject<UDataTable>();
		TempDataTable->RowStruct = DataTable->RowStruct;
		// Get data table path names
		FString TempDataTablePath = TempDataTable->GetPathName();
		FString DataTablePath = DataTable->GetPathName();
		// Make sure that the file content is not empty
		if (SourceFileContent.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Source file content for %s data table is empty."), *DataTablePath);
			return;
		}

		TArray<FString> problems = TArray<FString>();
		// Check if the file is a CSV or JSON file and parse it accordingly
		if (SourceFileType == "CSV")
		{
			problems = TempDataTable->CreateTableFromCSVString(SourceFileContent);
		}
		else if (SourceFileType == "JSON")
		{
			problems = TempDataTable->CreateTableFromJSONString(SourceFileContent);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("File source for %s is not a CSV or JSON file"), *TempDataTablePath);
			return;
		}
		// Make sure that there were no problems
		if (problems.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("There were problems importing %s"), *TempDataTablePath);
			for (auto& problem : problems)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s"), *problem);
			}
		}
		else
		{
			// Add the new data to the existing data table
			for (auto& Row : TempDataTable->GetRowMap())
			{
				FName RowName = Row.Key;
				FTableRowBase* RowValue = (FTableRowBase*)Row.Value;
				DataTable->AddRow(RowName, *RowValue);
			}
			// Log success
			UE_LOG(LogTemp, Warning, TEXT("Added data to data table %s successfully"), *DataTablePath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Data table passed to add data function does not exist"));
	}
}

void ADataManager::ClearDataTable(UDataTable* DataTable)
{
	// Make sure that the data table exists
	if (DataTable)
	{
		// Get data table path names
		FString DataTablePath = DataTable->GetPathName();
		// Clear the data table
		DataTable->EmptyTable();
		// Log success
		UE_LOG(LogTemp, Warning, TEXT("Cleared data table %s successfully"), *DataTablePath);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Data table passed to clear data function does not exist"));
	}
}

FTableRowBase& ADataManager::GetMetadataFromActor(AActor* Actor)
{
	return *(ActorToMetadataMap.FindRef(Actor));
}

FSpatialDataStruct& ADataManager::GetSpatialDataFromActor(AActor* Actor)
{
	return *(ActorToSpatialDataMap.FindRef(Actor));
}

FString ADataManager::GetDataTypeFromActor(AActor* Actor)
{
	return ActorToDataTypeMap.FindRef(Actor);
}

UDataTable* ADataManager::GetMetadataTableFromFullDatasetName(FString FullDatasetName)
{
	return FullDatasetNameToSpatialMetadataTableMap.FindRef(FullDatasetName);
}

FString ADataManager::GetBoundaryPointsFromViewName(FString ViewName)
{
	return ViewNameToBoundaryPointsMap.FindRef(ViewName);
}

FString ADataManager::GetFullDatasetNameFromDataType(FString DataType)
{
	FString SubDatasetName = CurrentDataTypeNameToTableNameMap.FindRef(DataType);
	FString FullDatasetName = GetFullTableName(DataType, SubDatasetName);
	return FullDatasetName;
}

UStruct* ADataManager::GetMetadataStructFromActor(AActor* Actor)
{
	// Get the data type from the actor
	FString DataType = GetDataTypeFromActor(Actor);
	// Get the full dataset name from the data type
	FString FullDatasetName = GetFullDatasetNameFromDataType(DataType);
	// Get the metadata struct from the full dataset name
	UStruct* MetadataStruct = FullTableNameToMetadataStructMap.FindRef(FullDatasetName);

	return MetadataStruct;
}

bool ADataManager::ActorHasMetadataProperty(AActor* Actor, FString PropertyName)
{
	// Get the metadata type from the actor
	UStruct* MetadataType = GetMetadataStructFromActor(Actor);
	// Check if the metadata type has the property
	bool HasProperty = MetadataType->FindPropertyByName(*PropertyName) != nullptr;
	// Return the result
	return HasProperty;
}

FString ADataManager::GetPropertyValueAsString(FProperty* Property, const FTableRowBase& Metadata)
{
	FString PropertyName = Property->GetName();
	FString PropertyValue;

	// Check the property type and convert it to a string accordingly
	FString PropertyTypeName = Property->GetClass()->GetName();

	if (PropertyTypeName.Equals("IntProperty"))
	{
		FIntProperty* IntProp = CastField<FIntProperty>(Property);
		int32 Value = IntProp->GetPropertyValue_InContainer(&Metadata);
		PropertyValue = FString::Printf(TEXT("%d"), Value);
	}
	else if (PropertyTypeName.Equals("FloatProperty"))
	{
		FFloatProperty* FloatProp = CastField<FFloatProperty>(Property);
		float Value = FloatProp->GetPropertyValue_InContainer(&Metadata);
		PropertyValue = FString::Printf(TEXT("%f"), Value);
	}
	else if (PropertyTypeName.Equals("DoubleProperty"))
	{
		FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property);
		double Value = DoubleProp->GetPropertyValue_InContainer(&Metadata);
		PropertyValue = FString::Printf(TEXT("%f"), Value);
	}
	else if (PropertyTypeName.Equals("BoolProperty"))
	{
		FBoolProperty* BoolProp = CastField<FBoolProperty>(Property);
		bool Value = BoolProp->GetPropertyValue_InContainer(&Metadata);
		PropertyValue = Value ? "True" : "False";
	}
	else if (PropertyTypeName.Equals("StrProperty"))
	{
		FStrProperty* StrProp = CastField<FStrProperty>(Property);
		FString Value = StrProp->GetPropertyValue_InContainer(&Metadata);
		PropertyValue = Value;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Struct property %s type not supported. Type: %s"), *PropertyName,
		       *PropertyTypeName);
	}

	return PropertyValue;
}

FString ADataManager::GetPropertyValueStringFromMetadata(const FTableRowBase& Metadata, UStruct* MetadataStruct,
                                                         FString PropertyName)
{
	// Find the property with the given name in the metadata type
	FProperty* Property = MetadataStruct->FindPropertyByName(*PropertyName);
	if (Property == nullptr)
	{
		// The property was not found
		return FString();
	}
	// Get the property value as a string
	FString PropertyValue = GetPropertyValueAsString(Property, const_cast<FTableRowBase&>(Metadata));
	// Return the property value
	return PropertyValue;
}

void ADataManager::ForceRefresh()
{
	// Get the spatial data table
	FString SpatialDataTablePath = FString(TEXT("/Game/SpatialDataTable.SpatialDataTable"));
	UDataTable* SpatialDataTable = LoadObject<UDataTable>(NULL, *SpatialDataTablePath, NULL, LOAD_None, NULL);
	// Clear the spatial data table
	ClearDataTable(SpatialDataTable);
	// Iterate over the array of current full dataset names and populate the spatial data table
	for (int32 index = 0; index < CurrentFullTableNames.Num(); ++index)
	{
		FString FullDatasetName = CurrentFullTableNames[index];
		FString SpatialDataSourceFileType = GetFileTypeFromSourceFile(
			TableFilePathMap[FullDatasetName]["SpatialDataFilePath"]);
		TArray<FString> SpatialDataSourceFileContentChunks;
		FString SpatialDataSourceFileContents;
		if (SpatialDataSourceFileType.Equals("CSV"))
		{
			SpatialDataSourceFileContentChunks = GetChunkedContentFromCSVSourceFile(
				TableFilePathMap[FullDatasetName]["SpatialDataFilePath"], 1000);
			UE_LOG(LogTemp, Warning, TEXT("Loading spatial data into data table (in chunks) for dataset %s"),
			       *FullDatasetName);
			for (int32 ChunkIndex = 0; ChunkIndex < SpatialDataSourceFileContentChunks.Num(); ++ChunkIndex)
			{
				UE_LOG(LogTemp, Warning, TEXT("Chunk %d of %d"), ChunkIndex + 1,
				       SpatialDataSourceFileContentChunks.Num());
				UE_LOG(LogTemp, Warning, TEXT("File contents: %s"), *(SpatialDataSourceFileContentChunks[ChunkIndex]));
				AddDataToDataTableFromSource(SpatialDataTable, SpatialDataSourceFileContentChunks[ChunkIndex],
				                             SpatialDataSourceFileType);
			}
		}
		else
		{
			SpatialDataSourceFileContents = GetContentFromSourceFile(
				TableFilePathMap[FullDatasetName]["SpatialDataFilePath"]);
			UE_LOG(LogTemp, Warning, TEXT("Loading spatial data into data table for dataset %s"), *FullDatasetName);
			AddDataToDataTableFromSource(SpatialDataTable, SpatialDataSourceFileContents, SpatialDataSourceFileType);
		}
		UE_LOG(LogTemp, Warning, TEXT("Finished loading spatial data into data table for dataset %s"),
		       *FullDatasetName);

		// Get the corresponding spatial metadata table
		UDataTable* MetadataTable = GetMetadataTableFromFullDatasetName(FullDatasetName);
		// Clear the metadata table
		ClearDataTable(MetadataTable);
		// Load the metadata into the metadata table
		FString MetadataSourceFileType = GetFileTypeFromSourceFile(
			TableFilePathMap[FullDatasetName]["SpatialMetadataFilePath"]);
		TArray<FString> MetadataSourceFileContentChunks;
		FString MetadataSourceFileContents;
		if (MetadataSourceFileType.Equals("CSV"))
		{
			MetadataSourceFileContentChunks = GetChunkedContentFromCSVSourceFile(
				TableFilePathMap[FullDatasetName]["SpatialMetadataFilePath"], 1000);
			UE_LOG(LogTemp, Warning, TEXT("Loading metadata into data table (in chunks) for dataset %s"),
			       *FullDatasetName);
			for (int32 ChunkIndex = 0; ChunkIndex < MetadataSourceFileContentChunks.Num(); ++ChunkIndex)
			{
				UE_LOG(LogTemp, Warning, TEXT("Chunk %d of %d"), ChunkIndex + 1, MetadataSourceFileContentChunks.Num());
				UE_LOG(LogTemp, Warning, TEXT("File contents: %s"), *(MetadataSourceFileContentChunks[ChunkIndex]));
				AddDataToDataTableFromSource(MetadataTable, MetadataSourceFileContentChunks[ChunkIndex],
				                             MetadataSourceFileType);
			}
		}
		else
		{
			MetadataSourceFileContents = GetContentFromSourceFile(
				TableFilePathMap[FullDatasetName]["SpatialMetadataFilePath"]);
			UE_LOG(LogTemp, Warning, TEXT("Loading metadata into data table for dataset %s"), *FullDatasetName);
			AddDataToDataTableFromSource(MetadataTable, MetadataSourceFileContents, MetadataSourceFileType);
		}
		UE_LOG(LogTemp, Warning, TEXT("Finished loading metadata into data table for dataset %s"), *FullDatasetName);
	}
	// Load the boundary points (POIs) for the current view
	FString BoundaryPointsSourceFileContents = GetBoundaryPointsFromViewName(CurrentViewName);
	FString BoundaryPointsSourceFileType = "JSON";
	// This is hard-coded; we will always assume that boundary points will be provided as JSON within the main JSON config file
	UDataTable* POIDataTable = LoadObject<UDataTable>(NULL, *FString(TEXT("/Game/POIDataTable.POIDataTable")), NULL,
	                                                  LOAD_None, NULL);
	AddDataToDataTableFromSource(POIDataTable, BoundaryPointsSourceFileContents, BoundaryPointsSourceFileType);
}
