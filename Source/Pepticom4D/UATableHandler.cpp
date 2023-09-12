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

FString UATableHandler::GetManyToOneKey(const TArray<FVarStruct>& Variables)
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

	return SpecificKey;
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
