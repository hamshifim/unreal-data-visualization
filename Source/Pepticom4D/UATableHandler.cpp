#include "UATableHandler.h"

// An Initialization of the necessary variables
void UATableHandler::Initialize(FString ADataType, FString ATableName, FString AKeyRegex, FString TableSourceFilePath)
{
	this->DataType = ADataType;
	this->TableName = ATableName;
	this->KeyRegex = AKeyRegex;
	this->SourcePath = TableSourceFilePath;
	this->FullTableName = ADataType + TEXT("_") + ATableName;
	this->StructName = StructNameFromFullTableName();
	InitTable();
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
	UDataTable* ADataTable = NewObject<UDataTable>(GetTransientPackage(), FName(*TableName), RF_Transient);
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

void UATableHandler::AddDataToDataTableFromSource()
{
	FString FileExtension = FPaths::GetExtension(SourcePath).ToUpper();

	FString SourceFileContent = GetContentFromSourceFile(SourcePath);
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
		if (FileExtension == "CSV")
		{
			problems = TempDataTable->CreateTableFromCSVString(SourceFileContent);
		}
		else if (SourceFileContent == "JSON")
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
			UE_LOG(LogTemp, Display, TEXT("Added data to data table %s successfully"), *DataTablePath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Data table passed to add data function does not exist"));
	}
}


FString UATableHandler::GetContentFromSourceFile(FString SourceFilePath)
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


void UATableHandler::ClearData()
{
	this->DataTable->EmptyTable();
}


void UATableHandler::GetTableRow(const TArray<FVarStruct>& Variables)
{
	// FString StructName = DataTable->RowStruct->GetName();
	// UE_LOG(LogTemp, Display, TEXT("Shamooch %s."), *StructName);
	
	FName Key = GetSpecificKey(Variables);

	UE_LOG(LogTemp, Display, TEXT("RowKey: %s"), *Key.ToString());

	FTableRowBase* SpecificRow = DataTable->FindRow<FTableRowBase>(Key, TEXT(""));

	if(SpecificRow)
	{
		UE_LOG(LogTemp, Display, TEXT("girgash tov  yesh shoora %s."), *StructName);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("girgash ra ein shoora %s."), *StructName);
	}
}
