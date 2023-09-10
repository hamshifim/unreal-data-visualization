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
FString UAAnimationHandler::ReplaceVarNames(const TArray<FVarStruct>& Variables)
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


// FString UAAnimationHandler::ReplaceVarNames(const TArray<FVarStruct>& Variables)
// check if VarSrc equals the string actor else if it equals the string animation
// if (VarSrc.Equals("actor"))
// {
// 	// // add the variable name to the array of actor variables
// 						
// 	// ActorVariables.Emplace(VarName);
// }
// else if (VarSrc.Equals("animation"))
// {
// 	// // add the variable name to the array of animation variables
// 	// AnimationVariables.Emplace(VarName);
// }
// else
// {
// 	UE_LOG(LogTemp, Display, TEXT("Regex variable source type is not actor or animation."));
// }