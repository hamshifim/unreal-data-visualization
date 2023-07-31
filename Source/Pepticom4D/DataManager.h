// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Components/BoxComponent.h"
#include "DataManager.generated.h"

UCLASS()
class PEPTICOM4D_API ADataManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADataManager();

	UFUNCTION()
	FString GetFullDatasetNameFromMainAndSubDatasetNames(FString MainDatasetName, FString SubDatasetName);

	UFUNCTION()
	FString GetStructNameFromFullDatasetName(FString FullDatasetName);

	UFUNCTION()
	void ProcessConfig(FString ConfigVarName);

	UFUNCTION()
	UDataTable* CreateMetadataTableFromStruct(const FString& TableName, UScriptStruct* RowStruct);

	UFUNCTION()
	void RefreshDataTable(UDataTable* DataTable, FString SourceFileName);

	UFUNCTION()
	FTableRowBase& GetMetadataFromActor(AActor* Actor);

	// Properties
	// Map of actors that have been spawned; key is metadata, value is the actor
	// TODO: Change map to be from unique ID within metadata to actor, or create an additional map
	TMap<AActor*, FTableRowBase*> SpawnedActorsMap = TMap<AActor*, FTableRowBase*>();
	// Combination of main and sub dataset names
	FString CurrentFullDatasetName;
	// Current valid keys for each TPair: "SpatialDataFilePath", "SpatialMetadataFilePath", "POIFilePath". All entries in the map should have pairs with these keys.
	TMap<FString, TMap<FString, FString>> FullDatasetNameToFilePathsMap = TMap<FString, TMap<FString, FString>>();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* SpawnVolume;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
