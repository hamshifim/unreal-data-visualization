#pragma once

#include "CoreMinimal.h"
#include "UAAnimationHandler.h"
#include "Engine/DataTable.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Actor.h"
#include "Types/SlateEnums.h"
#include "UAViewHandler.generated.h"

UCLASS(BlueprintType)

class UAViewHandler : public UObject
{
	GENERATED_BODY()

private:
	FString Name;
	TArray<FString> DataTypeNames;
	TArray<FString> Animations;
	FString BoundaryPointsString;
	// Map of colors which can be applied to properties - K: view name, V: K: property name, V: K: property value, V: color
	TMap<FString, TMap<FString, FColor>> ColorMap;
	TArray<UAAnimationHandler*> AnimationHandlers;
	//TODO: make a struct of boundaries of the view
	//TODO: keep a Map of color maps of the view
	
public:
	void Initialize(FString AName, TArray<FString> ADataTypeNames);
	void SetAnimationHandlers(TArray<UAAnimationHandler*> AAnimationHandlers);
	void SetBoundaryPointsString(FString ABoundaryPointsString);
	void SetColorMap(TMap<FString, TMap<FString, FColor>> AColorMap);
	void AddAnimation(FString Animation);
	TMap<FString, TMap<FString, FColor>>* GetColorMap();
	FString GetBoundaryPointsString();
	TArray<FString> GetDataTypes();
	TArray<FString> GetAnimations();
};
