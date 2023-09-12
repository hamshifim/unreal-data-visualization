#include "UAAnimationHandler.h"



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

// 
FString UAAnimationHandler::GetManyToOneKey(const TArray<FVarStruct>& Variables)
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

void UAAnimationHandler::GetTableRow(const TArray<FVarStruct>& Variables)
{
	FString Key = GetManyToOneKey(Variables);
	UE_LOG(LogTemp, Display, TEXT("Regex replaced Drakula: %s."), *Key);

	// FullManyToOneTableNameToMetadataStructMap
}

FString UAAnimationHandler::GetTableName()
{
	return TableName;;
}
