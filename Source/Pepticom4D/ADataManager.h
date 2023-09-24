// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Components/BoxComponent.h"
#include "SpatialDataStruct.h"
#include "UObject/UnrealType.h"
#include "UADataTypeHandler.h"
#include "UATableHandler.h"
#include "UAViewHandler.h"
#include "UAAnimationHandler.h"
#include "DataPointActor.h"
#include "ADataManager.generated.h"

UCLASS()
class PEPTICOM4D_API ADataManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADataManager();

	UFUNCTION()
	UAViewHandler* GetCurrentViewHandler();

	UFUNCTION()
	FString GetFullTableName(FString DataTypeName, FString TableName);

	UFUNCTION()
	FString StructNameFromFullTableName(FString FullTableName);

	UFUNCTION()
	void ProcessConfig(FString ConfigVarName);

	UFUNCTION()
	UDataTable* CreateTableFromStruct(const FString& TableName, UScriptStruct* RowStruct);

	UFUNCTION()
	FString GetContentFromSourceFile(FString SourceFilePath);

	UFUNCTION()
	TArray<FString> GetChunkedContentFromCSVSourceFile(FString SourceFilePath, int ChunkSize);

	UFUNCTION()
	FString GetFileTypeFromSourceFile(FString SourceFilePath);

	UFUNCTION()
	void AddDataToDataTableFromSource(UDataTable* DataTable, FString& SourceFileContent, FString& SourceFileType);

	UFUNCTION()
	void ClearDataTable(UDataTable* DataTable);

	UFUNCTION()
	FString GetBoundaryPointsFromViewName(FString ViewName);

	UFUNCTION()
	FString GetFullDatasetNameFromDataType(FString DataType);

	UFUNCTION()
	UStruct* GetMetadataStructFromActor(ADataPointActor* DataPointActor);
	
	UFUNCTION()
	bool ActorHasMetadataProperty(ADataPointActor* DataPointActor, FString PropertyName);

	UFUNCTION()
	FString GetPropertyValueStringFromMetadata(const FTableRowBase& Metadata, UStruct* MetadataStruct, FString PropertyName);

	UFUNCTION()
	void ForceRefresh();

	UFUNCTION()
	TArray<FString> GetCurrentDataTypes();

	FString GetPropertyValueAsString(FProperty* Property, const FTableRowBase& Metadata);

	// Each view contains at least one main dataset, and each main dataset contains at least one sub dataset. Multiple main datasets can be viewed at once, but only one sub dataset can be viewed per main dataset.
	FString CurrentViewName = TEXT("");

	// map view names to UAViewHandler pointers
	UPROPERTY()
	TMap<FString, UAViewHandler*> ViewHandlerMap;
	
	//Map of animation names to animation objects
	UPROPERTY()
	TMap<FString, UAAnimationHandler*> AnimationHandlerMap;

	//Map of DataTypeNames to UADatatypeHandlers
	UPROPERTY()
	TMap<FString, UADataTypeHandler*> DataTypeHandlerMap;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* SpawnVolume;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:

	void ExtractViews(TSharedPtr<FJsonObject> JsonObject);
	void ExtractDataTypes(TSharedPtr<FJsonObject> JsonObject);
	TArray<FString> ExtractTables(UADataTypeHandler* DataTypeHandler, FString DataTypeName, TSharedPtr<FJsonObject> DataTypeObj);
	void ExtractManyToOneTables(UADataTypeHandler* DataTypeHandler, FString DataTypeName, TSharedPtr<FJsonObject> DataTypeObj);
	void ExtractAnimations(TSharedPtr<FJsonObject> ViewObject);
	int32 ExtractIntField(TSharedPtr<FJsonObject>JsonObject, FString FieldName);
	FString ExtractStringField(TSharedPtr<FJsonObject>JsonObject, FString FieldName);
	TArray<TSharedPtr<FJsonValue>> ExtractStringArrayField(TSharedPtr<FJsonObject> JsonObject, FString FieldName);
};
