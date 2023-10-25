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
	UPROPERTY()
	UATableHandler* DefaultTableHandler;
	UPROPERTY()
	TMap<FString, UATableHandler*> TableHandlerMap;
	UPROPERTY()
	TMap<FString, UATableHandler*> ManyToOneTableHandlerMap;
	UPROPERTY()
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
	UFUNCTION()
	UATableHandler* GetDefaultTableHandler();
	TMap<FString, UATableHandler*> GetTableHandlerMap();
	UFUNCTION()
	TArray<ADataPointActor*> GetDataPointActors();

	UFUNCTION()
	void AddDataPointActor(ADataPointActor* DataPointActor);
};
