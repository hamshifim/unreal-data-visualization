#pragma once

#include "CoreMinimal.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Actor.h"
#include "Types/SlateEnums.h"
#include "FVarStruct.h"
#include "UADataTypeHandler.h"
#include "TimerManager.h" 
#include "Components/TextBlock.h"
#include "UAAnimationHandler.generated.h"

UCLASS(BlueprintType)
class PEPTICOM4D_API UAAnimationHandler : public UObject
{

	GENERATED_BODY()

private:

	FString AnimationName;
	FString AnimationDimension;
	int32 Min;
	int32 Max;
	int32 Interval;
	FString DataType;
	FString ActorTableName;
	FString OneToManyTableName;
	FString KeyRegex;

	UPROPERTY()
	TArray<FString> RegexVariableNames;
	UPROPERTY()
	TArray<FString> UpdateProperties;
	//probably better to send a table but that would require a lot of refactoring

	TMap<FString, UADataTypeHandler*>* DataTypeHandlerMap;
	UStruct* MetaDataStruct;
	TMap<FString, FProperty*> KeyProperties;
	UStruct* ManyToOneStruct;
	FString AnimationValue;

	UPROPERTY()
	TArray<int32> PossibleAnimationValues;
	int CurrentAnimationIndex;
	FTimerHandle AnimationTimerHandle;

	UPROPERTY()
	UTextBlock* AnimationTextBlock;

public:

	//An initializer that takes in all the necessary parameters to create an animation handler
	void Initialize(FString AAnimationName, FString AnimationDimension, int32 AMin, int32 AMax, int32 AInterval, FString ADataType, FString AActorTableName, FString AManyToOneTableName, FString AKeyRegex, TArray<FString> ARegexVariableNames, TArray<FString> AUpdateProperties, TMap<FString, UADataTypeHandler*>* ADataTypeHandlerMap);

	void Sanity();

	TArray<int32> GetPossibleAnimationValues();

	FString GetManyToOneTableName();
	FString GetPropertyValueAsString(FProperty* Property, const FTableRowBase& Metadata);

	UFUNCTION()
	void AnimateActors();

	UFUNCTION()
	void Animate();
	UFUNCTION()
	void AnimateStep();
	UFUNCTION()
	void OnAnimationValueChanged(FString AAnimationValue);
	UFUNCTION()
	void LoadData();

	UFUNCTION()
	float GetMinValue();
	UFUNCTION()
	float GetMaxValue();
	UFUNCTION()
	float GetInterval();
	UFUNCTION()
	FString GetAnimationName();
	UFUNCTION()
	void SetAnimationTextBlock(UTextBlock* AAnimationTextBlock);
};
