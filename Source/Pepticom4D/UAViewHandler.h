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
	FString BoundaryPointsString;
	TArray<UAAnimationHandler*> AnimationHandlers;
	//TODO: make a struct of boundaries of the view
	//TODO: keep a Map of color maps of the view
	
public:
	void Initialize(FString AName, TArray<FString> ADataTypeNames);
	void SetAnimationHandlers(TArray<UAAnimationHandler*> AAnimationHandlers);
	void SetBoundaryPointsString(FString ABoundaryPointsString);
	FString GetBoundaryPointsString();
};
