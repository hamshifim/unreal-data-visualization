#pragma once

#include "CoreMinimal.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Actor.h"
#include "Types/SlateEnums.h"
#include "UATableHandler.h"
#include "UADataTypeHandler.generated.h"

UCLASS(BlueprintType)


class UADataTypeHandler: public UObject
{
	GENERATED_BODY()

private:

	FString Name;
	FString DefaultTableName;
	TMap<FString, UATableHandler*> TableHandlerMap;
	TMap<FString, UATableHandler*> ManyToOneTableHandlerMap;

public:
	//itnializes the data type handler
	void Initialize(FString AName, FString ADefaultTableName);
	void Sanity();
	FString GetDefaultTableName();
	void SetManyToOneTableHandlerMap(TMap<FString, UATableHandler*> Map);
	UATableHandler* GetManyToOneTableHandler(FString TableName);
	UATableHandler* GetTableHandler(FString TableName);
	void AddTableHandler(FString TableName, UATableHandler* TableHandler);
};
