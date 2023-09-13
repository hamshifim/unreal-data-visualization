#include "UAAnimationHandler.h"
#include "UATableHandler.h"


// An Initialization of the necessary variables
void UAAnimationHandler::Initialize(FString AAnimationName, int32 AMin, int32 AMax, int32 AInterval, FString ADataType, FString ATableName, FString AKeyRegex, TArray<FVarStruct> ARegexVariableRetrievalInstructions, TArray<FString> AUpdateProperties, TMap<FString, UADataTypeHandler*>* ADataTypeHandlerMap)
{
	this->AnimationName = AAnimationName;
	this->Min = AMin;
	this->Max = AMax;
	this->Interval = AInterval;
	this->DataType = ADataType;
	this->TableName = ATableName;
	this->KeyRegex = AKeyRegex;
	this->RegexVariableRetrievalInstructions = ARegexVariableRetrievalInstructions;
	this->UpdateProperties = AUpdateProperties;
	this->DataTypeHandlerMap = ADataTypeHandlerMap;
}

void UAAnimationHandler::Sanity() 
{
	UE_LOG(LogTemp, Display, TEXT("Ashmagog you bastard: %s."), *this->AnimationName);
}


//iterate from Min to Max adding Interval adding values to array and return it
TArray<int32> UAAnimationHandler::GetPossibleAnimationValues()
{
	TArray<int32> PossibleAnimationValues;
	for (int32 i= Min; i <= Max; i+=Interval)
	{
		PossibleAnimationValues.Add(i);
		UE_LOG(LogTemp, Display, TEXT("googalach: %d"), i);
	}

	return PossibleAnimationValues;
}



FString UAAnimationHandler::GetTableName()
{
	return TableName;;
}

void UAAnimationHandler::AnimateActor(TArray<FVarStruct> Variables)
{
	UADataTypeHandler* DataTypeHandler = DataTypeHandlerMap->FindRef(this->DataType);
	UATableHandler* TableHandler = DataTypeHandler->GetTableHandler(this->TableName);
	// TODO initiate table in the proper context!!!!
	TableHandler->AddDataToDataTableFromSource();

	UE_LOG(LogTemp, Display, TEXT("Garlik"));

	UDataTable* ManyToOneTable = TableHandler->GetDataTable();
			
	FString FOO = ManyToOneTable->RowStruct->GetName();
			
	UE_LOG(LogTemp, Display, TEXT("Shamooch %s."), *FOO);

	
	FString RowName = TableHandler->GetManyToOneKey(Variables);

	UE_LOG(LogTemp, Display, TEXT("moops RowName %s."), *RowName);

	//create an FName from RowName
	FName RowNameFName = FName(*RowName);

	UE_LOG(LogTemp, Display, TEXT("RowNameFName: %s"), *RowNameFName.ToString());
	
	TArray<FName> SpatialMetadataRowNames = ManyToOneTable->GetRowNames();

	//iterate over SpatialMetadataRowNames printing their names
	for (const auto& SpatialMetadataRowName : SpatialMetadataRowNames)
	{
		UE_LOG(LogTemp, Display, TEXT("Balbook SpatialMetadataRowName: %s"), *SpatialMetadataRowName.ToString());
	}

	FTableRowBase* SpecificRow = ManyToOneTable->FindRow<FTableRowBase>(RowNameFName, TEXT(""));

	if(SpecificRow)
	{
		UE_LOG(LogTemp, Display, TEXT("girgash %s."), *FOO);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("gisplash %s."), *FOO);
	}
}
