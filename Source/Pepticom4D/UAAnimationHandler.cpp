#include "UAAnimationHandler.h"
#include "ADataManager.h"
#include "UATableHandler.h"


// An Initialization of the necessary variables
void UAAnimationHandler::Initialize(FString AAnimationName, int32 AMin, int32 AMax, int32 AInterval, FString ADataType, FString ATableName, FString AKeyRegex, TArray<FVarStruct> ARegexVariableRetrievalInstructions, TArray<FString> AUpdateProperties)
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
	// ADataManager* DataManager = ADataManager::GetInstance();
	// UATableHandler* TableHandler = DataManager.DataTypeToTableHandlerMap.FindRef(TEXT("clustered")).FindRef(TEXT("cycle"));
}
