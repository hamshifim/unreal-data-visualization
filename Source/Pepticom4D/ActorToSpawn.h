// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PostProcessComponent.h"
#include "ActorToSpawn.generated.h"

UCLASS()
class PEPTICOM4D_API AActorToSpawn : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActorToSpawn();
	void Create();
	void ChangeColor(FString ColorHex);
	void ChangeColor(FColor NewColor);
	void ChangeScale(float NewScale);

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
	float GetNormalizedScale(float ScaleToNormalize, float MinScale, float MaxScale);

	float Scale = 1.0f;
};
