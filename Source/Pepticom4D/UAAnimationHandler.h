#pragma once

#include "CoreMinimal.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Actor.h"
#include "Types/SlateEnums.h"
#include "FVarStruct.h"
#include "UADataTypeHandler.h"
#include "UAAnimationHandler.generated.h"

UCLASS(BlueprintType)
class PEPTICOM4D_API UAAnimationHandler : public UObject
{

	GENERATED_BODY()

private:

	FString AnimationName;
	int32 Min;
	int32 Max;
	int32 Interval;
	FString DataType;
	FString TableName;
	FString KeyRegex;
	TArray<FVarStruct> RegexVariableRetrievalInstructions;
	TArray<FString> UpdateProperties;
	//probably better to send a table but that would require a lot of refactoring
	TMap<FString, UADataTypeHandler*>* DataTypeHandlerMap; 
	
public:

	//An initializer that takes in all the necessary parameters to create an animation handler
	void Initialize(FString AAnimationName, int32 AMin, int32 AMax, int32 AInterval, FString ADataType, FString ATableName, FString AKeyRegex, TArray<FVarStruct> ARegexVariableRetrievalInstructions, TArray<FString> AUpdateProperties, TMap<FString, UADataTypeHandler*>* ADataTypeHandlerMap);

	void Sanity();

	TArray<int32> GetPossibleAnimationValues();

	FString GetTableName();

	void AnimateActor(TArray<FVarStruct> Variables);
};
