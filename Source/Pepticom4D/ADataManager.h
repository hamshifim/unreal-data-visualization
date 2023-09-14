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
	FTableRowBase& GetMetadataFromActor(ADataPointActor* Actor);

	UFUNCTION()
	FSpatialDataStruct& GetSpatialDataFromActor(ADataPointActor* Actor);

	UFUNCTION()
	FString GetDataTypeFromActor(ADataPointActor* Actor);

	UFUNCTION()
	UDataTable* GetMetadataTableFromFullDatasetName(FString FullDatasetName);

	UFUNCTION()
	FString GetBoundaryPointsFromViewName(FString ViewName);

	UFUNCTION()
	FString GetFullDatasetNameFromDataType(FString DataType);

	UFUNCTION()
	UStruct* GetMetadataStructFromActor(ADataPointActor* Actor);

	UFUNCTION()
	bool ActorHasMetadataProperty(ADataPointActor* Actor, FString PropertyName);

	UFUNCTION()
	FString GetPropertyValueStringFromMetadata(const FTableRowBase& Metadata, UStruct* MetadataStruct, FString PropertyName);

	UFUNCTION()
	void ForceRefresh();

	FString GetPropertyValueAsString(FProperty* Property, const FTableRowBase& Metadata);

	// Properties
	// Map of actors that have been spawned; key is metadata, value is the actor
	// TODO: Change map to be from unique ID within metadata to actor, or create an additional map
	TMap<ADataPointActor*, FTableRowBase*> ActorToMetadataMap = TMap<ADataPointActor*, FTableRowBase*>();
	// Map of actors to their spatial data
	TMap<ADataPointActor*, FSpatialDataStruct*> ActorToSpatialDataMap = TMap<ADataPointActor*, FSpatialDataStruct*>();
	// Map of actors to their data type
	TMap<ADataPointActor*, FString> ActorToDataTypeMap = TMap<ADataPointActor*, FString>();
	// Each view contains at least one main dataset, and each main dataset contains at least one sub dataset. Multiple main datasets can be viewed at once, but only one sub dataset can be viewed per main dataset.
	FString CurrentViewName = TEXT("");
	// Combination of main and sub dataset names within the current view
	TArray<FString> CurrentFullTableNames = TArray<FString>();
	// Map of data type (main dataset name) to the current sub dataset name for that data type
	TMap<FString, FString> CurrentDataTypeNameToTableNameMap = TMap<FString, FString>();
	// Current valid keys for each TPair: "SpatialDataFilePath", "SpatialMetadataFilePath". All entries in the map should have pairs with these keys.
	TMap<FString, TMap<FString, FString>> TableFilePathMap = TMap<FString, TMap<FString, FString>>();

	// Map of full dataset names to the UStruct type of their metadata
	TMap<FString, UStruct*> FullTableNameToMetadataStructMap = TMap<FString, UStruct*>();
	
	// Map of data full dataset names to spatial metadata tables
	TMap<FString, UDataTable*> FullTableNameToSpatialMetadataTableMap = TMap<FString, UDataTable*>();
	
	// map view names to UAViewHandler pointers
	TMap<FString, UAViewHandler*> ViewHandlerMap;

	// Map of data types to table names
	TMap<FString, TArray<FString>> DataTypeToTableNamesMap = TMap<FString, TArray<FString>>();
	
	//Map of animation names to animation objects
	TMap<FString, UAAnimationHandler*> AnimationHandlerMap;

	//Map of DataTypeNames to UADatatypeHandlers
	TMap<FString, UADataTypeHandler*> DataTypeHandlerMap;
	
	//Map of data types to table names to TableHandlers
	TMap<FString, TMap<FString, UATableHandler*>> DataTypeToTableHandlerMap;

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
	void ExtractAnimations(FString ViewName, TSharedPtr<FJsonObject> ViewObject);
	int32 ExtractIntField(TSharedPtr<FJsonObject>JsonObject, FString FieldName);
	FString ExtractStringField(TSharedPtr<FJsonObject>JsonObject, FString FieldName);
	TArray<TSharedPtr<FJsonValue>> ExtractStringArrayField(TSharedPtr<FJsonObject> JsonObject, FString FieldName);
};
