// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PostProcessComponent.h"
#include "SpatialDataStruct.h"
#include "DataPointActor.generated.h"

UCLASS()
class PEPTICOM4D_API ADataPointActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADataPointActor();
	void Create();
	void Initialize(FString ADataType, FString AMetaDataTableName, FSpatialDataStruct* ASpatialDataStruct, FTableRowBase* AMetadataRow);
	void ChangeColor(FString ColorHex);
	void ChangeColor(FColor NewColor);
	void ChangeScale(const float NewScale) const;
	void ExtractDataTypes(const TTuple<FString, TSharedPtr<FJsonValue, ESPMode::ThreadSafe>>&);
	FString GetDataType() const;
	FSpatialDataStruct* GetSpatialDataStruct() const;
	FTableRowBase& GetMetadataRow() const;
	FString GetTableName();
	FString GetMetaDataTableName() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystemComponent* ParticleComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInstanceDynamic* DynamicMaterial;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnMeshClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed);
	FString DataType;
	FString TableName;
	FString MetaDataTableName;
	FSpatialDataStruct* SpatialDataStruct;
	FTableRowBase* MetadataRow;
};
