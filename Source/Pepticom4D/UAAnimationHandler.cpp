#include "UAAnimationHandler.h"
#include "UATableHandler.h"


// An Initialization of the necessary variables
void UAAnimationHandler::Initialize(FString AAnimationName, FString AAnimationDimension, int32 AMin, int32 AMax, int32 AInterval, FString ADataType, FString ATableName, FString AKeyRegex, TArray<FString> ARegexVariableNames, TArray<FString> AUpdateProperties, TMap<FString, UADataTypeHandler*>* ADataTypeHandlerMap)
{
	this->AnimationName = AAnimationName;
	this->AnimationDimension = AAnimationDimension;
	this->Min = AMin;
	this->Max = AMax;
	this->Interval = AInterval;
	this->DataType = ADataType;
	this->TableName = ATableName;
	this->KeyRegex = AKeyRegex;
	this->RegexVariableNames = ARegexVariableNames;
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
	
	UATableHandler* ManyToOneTableHandler = DataTypeHandler->GetManyToOneTableHandler(this->TableName);

	UE_LOG(LogTemp, Display, TEXT("Zroobabvel"));

	ManyToOneTableHandler->GetTableRow(Variables);

	UE_LOG(LogTemp, Display, TEXT("Zroobabvel 1"));

	//TODO use the data to animate the actors
	const UDataTable* ManyToOneTable = ManyToOneTableHandler->GetDataTable();
	
	TArray<FName> SpatialMetadataRowNames = ManyToOneTable->GetRowNames();
	
	for (const auto& SpatialMetadataRowName : SpatialMetadataRowNames)
	{
		UE_LOG(LogTemp, Display, TEXT("Balbook SpatialMetadataRowName: %s"), *SpatialMetadataRowName.ToString());
	}

	UE_LOG(LogTemp, Display, TEXT("Zroobabvel 3"));
}

void UAAnimationHandler::OnAnimationValueChanged(FString AnimationValue)
{
	//init an array of FVarStruct
	TArray<FVarStruct> Variables;
	Variables.Add(FVarStruct("Index", "69973607186440"));
	Variables.Add(FVarStruct(AnimationDimension, AnimationValue));
	Variables.Add(FVarStruct("BackboneSize", "5"));

	UE_LOG(LogTemp, Display, TEXT("Wowfull AnimationDimension: %s"), *AnimationDimension);
	AnimateActor(Variables);
	UE_LOG(LogTemp, Display, TEXT("Bombardful AnimationDimension: %s"), *AnimationDimension);
}

void UAAnimationHandler::LoadData()
{
	UADataTypeHandler* DataTypeHandler = DataTypeHandlerMap->FindRef(this->DataType);
	UATableHandler* TableHandler = DataTypeHandler->GetManyToOneTableHandler(this->TableName);
	TableHandler->AddDataToDataTableFromSource();
}

float UAAnimationHandler::GetMinValue()
{
	return Min * 1.0f;
}

float UAAnimationHandler::GetMaxValue()
{
	return Max * 1.0f;
}

float UAAnimationHandler::GetInterval()
{
	return Interval * 1.0f;
}
