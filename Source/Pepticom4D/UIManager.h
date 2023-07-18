// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorDataWidget.h"
#include "DataInteractionPlayerController.h"
#include "ActorSpawner.h"
#include "UIManager.generated.h"

UCLASS()
class PEPTICOM4D_API AUIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUIManager();

	UFUNCTION(BlueprintCallable, Category="UIManager")
	void UpdateText(UUserWidget* TextWidgetObject, FString TextBlockName, FString NewText);

	UFUNCTION()
	void UpdateViewNameWidget(FString NewViewName);

	UFUNCTION()
	void DisplayActorDataWidget(AActor* Actor);

	UFUNCTION()
	void RefreshActorDataWidget(AActor* Actor);

	UFUNCTION()
	void HideActorDataWidget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* SpawnVolume;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UUserWidget* ViewNameWidget;
	UUserWidget* ActorDataWidgetGeneric;
	UActorDataWidget* ActorDataWidget;
	bool InitializedWidgets = false;
	ADataInteractionPlayerController* DataInteractionPlayerController;
	AActorSpawner* ActorSpawner;

	void CreateAndRenderWidget(FString WidgetName, UUserWidget*& WidgetObject);
	FString GetFriendlyPropertyName(FString PropertyName);
	FString GetPropertyValueAsString(FProperty *Property, FSpatialMetadataStruct& Metadata);

};
