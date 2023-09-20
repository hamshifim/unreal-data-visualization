#pragma once

#include "CoreMinimal.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Actor.h"
#include "Types/SlateEnums.h"
#include "UATableHandler.h"
#include "DataPointActor.h"
#include "UADataTypeHandler.generated.h"

UCLASS(BlueprintType)


class UADataTypeHandler: public UObject
{
	GENERATED_BODY()

private:

	FString Name;
	FString DefaultTableName;
	UATableHandler* DefaultTableHandler;
	TMap<FString, UATableHandler*> TableHandlerMap;
	TMap<FString, UATableHandler*> ManyToOneTableHandlerMap;
	TArray<ADataPointActor*> DataPointActors;

public:

	void Initialize(FString Name, FString DefaultTableName);
	void Sanity();
	FString GetDefaultTableName();
	void AddToManyToOneTableHandlerMap(FString TableName, UATableHandler* TableHandler);
	UATableHandler* GetManyToOneTableHandler(FString TableName);
	UATableHandler* GetTableHandler(FString TableName);
	void AddTableHandler(FString TableName, UATableHandler* TableHandler);
	void SetDefaultTableHandler(UATableHandler* TableHandler);
	UATableHandler* GetDefaultTableHandler() const;
	TMap<FString, UATableHandler*> GetTableHandlerMap();
	void AddDataPointActor(ADataPointActor* DataPointActor);
	TArray<ADataPointActor*> GetDataPointActors();
};
