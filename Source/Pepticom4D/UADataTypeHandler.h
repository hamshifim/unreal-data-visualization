#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Actor.h"
#include "Types/SlateEnums.h"
#include "UADataTypeHandler.generated.h"

UCLASS(BlueprintType)


class UADataTypeHandler: public UObject
{
	GENERATED_BODY()

private:

	FString Name;
	FString DefaultTableName;
	
public:
	//itnializes the data type handler
	void Initialize(FString AName, FString ADefaultTableName);
	void Sanity();
	FString GetDefaultTableName();
};
