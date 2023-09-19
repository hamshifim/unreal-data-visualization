#include "UATableHandler.h"

// An Initialization of the necessary variables
void UATableHandler::InitializeTransientTable(FString ADataType, FString ATableName, FString AKeyRegex, FString ASourcePath)
{
	this->DataType = ADataType;
	this->TableName = ATableName;
	this->KeyRegex = AKeyRegex;
	this->SourcePath = ASourcePath;
	this->FullTableName = ADataType + TEXT("_") + ATableName;
	this->StructName = StructNameFromFullTableName();
	InitTable();
}


void UATableHandler::InitializeSpatialTable(FString ADataType, FString ATableName, FString ADataSource)
{
	this->DataType = ADataType;
	this->TableName = ATableName;
	this->KeyRegex = TEXT("<Index>");
	this->SourcePath = ADataSource;
	this->FullTableName = ADataType + TEXT("_") + ATableName;
	this->StructName = TEXT("SpatialDataStruct");

	FString SpatialDataTablePath = TEXT("/Game/SpatialDataTable.SpatialDataTable");
	UDataTable* Snoopy = LoadObject<UDataTable>(NULL, *SpatialDataTablePath, NULL, LOAD_None, NULL);

	// Create a new Data Table asset
	UDataTable* ADataTable = NewObject<UDataTable>(GetTransientPackage(), FName(*FullTableName), RF_Transient);
	ADataTable->RowStruct = Snoopy->RowStruct;
	this->DataTable = ADataTable;
}


FString UATableHandler::GetFullTableName()
{
	return FullTableName;
}

FString UATableHandler::StructNameFromFullTableName()
{
	// Assumes an input of the form "dat_type_name_table_name, converts to DataTypeNameTableNameTempStruct"
	FString PascalCaseString;
	bool bNextIsUpper = true;
	for (const TCHAR Char : this->FullTableName)
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
	FString AStructName = PascalCaseString + "TempStruct";
	return AStructName;
}

void UATableHandler::InitTable()
{
	UScriptStruct* RowStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *StructName);

	// Create a new Data Table asset
	UDataTable* ADataTable = NewObject<UDataTable>(GetTransientPackage(), FName(*FullTableName), RF_Transient);
	ADataTable->RowStruct = RowStruct;

	// Register the Data Table
	ADataTable->AddToRoot();

	// Return the metadata table
	this->DataTable = ADataTable;
}


FName UATableHandler::GetSpecificKey(const TArray<FVarStruct>& Variables)
{
	FString SpecificKey = KeyRegex;
	//iterate through the array of variables
	for (int32 i = 0; i < Variables.Num(); i++)
	{
		//get the variable name
		FString VarName = FString::Printf(TEXT("<%s>"), *Variables[i].VarName);
		//get the variable source type
		FString VarValue = Variables[i].SourceType;

		SpecificKey = SpecificKey.Replace(*VarName, *VarValue);
	}

	return FName(*SpecificKey);
}

//To String method
void UATableHandler::VerbosePrint()
{
	FString SelfString = "DataType: " + DataType + ", TableName: " + TableName + ", KeyRegex: " + KeyRegex + ", FullTableName: " + FullTableName + ", StructName: " + StructName + ",\nSourcePath: " + SourcePath;
	UE_LOG(LogTemp, Display, TEXT("%s"), *SelfString);
}

UDataTable* UATableHandler::GetDataTable()
{
	return DataTable;
}

void UATableHandler::AddContentToTable(FString Content, FString FileType)
{
	// Create a temporary data table to hold the new data and copy the row struct from the original data table
	UDataTable* TempDataTable = NewObject<UDataTable>();
	TempDataTable->RowStruct = DataTable->RowStruct;
	// Get data table path names
	FString TempDataTablePath = TempDataTable->GetPathName();
	FString DataTablePath = DataTable->GetPathName();
	// Make sure that the file content is not empty
	if (Content.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Source file content for %s data table is empty."), *DataTablePath);
		return;
	}

	TArray<FString> problems = TArray<FString>();
	// Check if the file is a CSV or JSON file and parse it accordingly
	if (FileType == "CSV")
	{
		problems = TempDataTable->CreateTableFromCSVString(Content);
	}
	else if (FileType == "JSON")
	{
		problems = TempDataTable->CreateTableFromJSONString(Content);
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
		UE_LOG(LogTemp, Display, TEXT("Added data to data table %s successfully"), *DataTablePath);
	}
}

void UATableHandler::AddDataToDataTableFromSource()
{
	FString FileExtension = FPaths::GetExtension(SourcePath).ToUpper();

	FString SourceFileContent = GetContentFromSourceFile();
	// Make sure that we found the data table
	if (DataTable)
	{
		AddContentToTable(SourceFileContent, FileExtension);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Data table passed to add data function does not exist"));
	}
}


void UATableHandler::AddDataToDataTableFromSource(int ChunkSize)
{
	TArray<FString> SpatialDataSourceFileContentChunks = GetChunkedContentFromCSVSourceFile(ChunkSize);

	for (int32 ChunkIndex = 0; ChunkIndex < SpatialDataSourceFileContentChunks.Num(); ++ChunkIndex)
	{
		UE_LOG(LogTemp, Display, TEXT("Chunk %d of %d"), ChunkIndex + 1, SpatialDataSourceFileContentChunks.Num());
		UE_LOG(LogTemp, Display, TEXT("File contents: %s"), *(SpatialDataSourceFileContentChunks[ChunkIndex]));
		
		AddContentToTable(SpatialDataSourceFileContentChunks[ChunkIndex], GetFileType());
	}
}


TArray<FString> UATableHandler::GetChunkedContentFromCSVSourceFile(int ChunkSize)
{
	// Validate the file extension
	FString FileExtension = FPaths::GetExtension(SourcePath).ToUpper();
	if (!FileExtension.Equals("CSV"))
	{
		UE_LOG(LogTemp, Error, TEXT("Source file passed to get chunked content is not a CSV file: %s"), *SourcePath);
		return TArray<FString>();
	}
	FString FileContent = GetContentFromSourceFile();
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


FString UATableHandler::GetContentFromSourceFile()
{
	FString FileContent = TEXT("");
	// Make sure that the source file exists
	if (FPaths::FileExists(SourcePath))
	{
		// Read the file into a string
		FFileHelper::LoadFileToString(FileContent, *SourcePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Source file does not exist: %s"), *SourcePath);
	}

	return FileContent;
}


void UATableHandler::ClearData()
{
	this->DataTable->EmptyTable();
}


FTableRowBase* UATableHandler::GetTableRow(const TArray<FVarStruct>& Variables)
{
	// FString StructName = DataTable->RowStruct->GetName();
	// UE_LOG(LogTemp, Display, TEXT("Shamooch %s."), *StructName);
	
	FName Key = GetSpecificKey(Variables);

	UE_LOG(LogTemp, Display, TEXT("RowKey: %s"), *Key.ToString());

	FTableRowBase* SpecificRow = DataTable->FindRow<FTableRowBase>(Key, TEXT(""));

	if(SpecificRow)
	{
		UE_LOG(LogTemp, Display, TEXT("girgash tov yesh shoora %s."), *StructName);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("girgash ra ein shoora %s."), *StructName);
	}

	return SpecificRow;
}

//TODO get rid of this by encapsulation
FString UATableHandler::GetFileType()
{
	return FPaths::GetExtension(SourcePath).ToUpper();
}

FString UATableHandler::GetTableName()
{
	return this->TableName;
}
