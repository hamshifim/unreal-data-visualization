// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Components/BoxComponent.h"
#include "SpatialDataStruct.h"
#include "UObject/UnrealType.h"
#include "DataManager.generated.h"

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
	UDataTable* CreateMetadataTableFromStruct(const FString& TableName, UScriptStruct* RowStruct);

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
	FTableRowBase& GetMetadataFromActor(AActor* Actor);

	UFUNCTION()
	FSpatialDataStruct& GetSpatialDataFromActor(AActor* Actor);

	UFUNCTION()
	FString GetDataTypeFromActor(AActor* Actor);

	UFUNCTION()
	UDataTable* GetMetadataTableFromFullDatasetName(FString FullDatasetName);

	UFUNCTION()
	FString GetBoundaryPointsFromViewName(FString ViewName);

	UFUNCTION()
	FString GetFullDatasetNameFromDataType(FString DataType);

	UFUNCTION()
	UStruct* GetMetadataStructFromActor(AActor* Actor);

	UFUNCTION()
	bool ActorHasMetadataProperty(AActor* Actor, FString PropertyName);

	UFUNCTION()
	FString GetPropertyValueStringFromMetadata(const FTableRowBase& Metadata, UStruct* MetadataStruct, FString PropertyName);

	UFUNCTION()
	void ForceRefresh();

	FString GetPropertyValueAsString(FProperty* Property, const FTableRowBase& Metadata);

	// Properties
	// Map of actors that have been spawned; key is metadata, value is the actor
	// TODO: Change map to be from unique ID within metadata to actor, or create an additional map
	TMap<AActor*, FTableRowBase*> ActorToMetadataMap = TMap<AActor*, FTableRowBase*>();
	// Map of actors to their spatial data
	TMap<AActor*, FSpatialDataStruct*> ActorToSpatialDataMap = TMap<AActor*, FSpatialDataStruct*>();
	// Map of actors to their data type
	TMap<AActor*, FString> ActorToDataTypeMap = TMap<AActor*, FString>();
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
	TMap<FString, UDataTable*> FullDatasetNameToSpatialMetadataTableMap = TMap<FString, UDataTable*>();
	// Map of view names to data types/main dataset names that are displayed in that view
	TMap<FString, TArray<FString>> ViewNameToDataTypesMap = TMap<FString, TArray<FString>>();
	// Map of view names to a JSON object string representing boundary points (POIs) for that view
	TMap<FString, FString> ViewNameToBoundaryPointsMap = TMap<FString, FString>();
	// Map of data types to table/sub dataset names
	TMap<FString, TArray<FString>> DataTypeToSubDatasetNamesMap = TMap<FString, TArray<FString>>();
	// Map of colors which can be applied to properties - K: view name, V: K: property name, V: K: property value, V: color
	TMap<FString, TMap<FString, TMap<FString, FColor>>> ColorMap = TMap<FString, TMap<FString, TMap<FString, FColor>>>();



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

};
