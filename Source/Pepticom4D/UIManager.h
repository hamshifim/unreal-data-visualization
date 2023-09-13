// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Actor.h"
#include "ActorDataWidget.h"
#include "DataSelectorWidget.h"
#include "DataFilteringWidget.h"
#include "DataInteractionPlayerController.h"
#include "Engine/DataTable.h"
#include "ADataManager.h"
#include "ActorToSpawn.h"
#include "Types/SlateEnums.h"
#include "DataInteractionHUD.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
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

	UFUNCTION()
	void StartDrawingConnectingLineToActorDataWidget(AActor* Actor);

	UFUNCTION()
	void StopDrawingConnectingLineToActorDataWidget();

	UFUNCTION()
	void ConfigureDataSelectorWidget();

	UFUNCTION()
	void RefreshDataSelectorWidget();

	UFUNCTION()
	void OnDataSelectorWidgetDropdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void ConfigureDataFilteringWidget();

	UFUNCTION()
	void RefreshDataFilteringWidget();

	UFUNCTION()
	void OnDataFilteringWidgetDropdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

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
	UBorder* ActorDataWidgetBorder;

	UUserWidget* DataSelectorWidgetGeneric;
	UDataSelectorWidget* DataSelectorWidget;
	TMap<FString, UTextBlock*> DataSelectorWidgetDataTypeNamesMap;
	TMap<FString, UComboBoxString*> DataSelectorWidgetTableNameComboBoxesMap;
	
	UUserWidget* DataFilteringWidgetGeneric;
	UDataFilteringWidget* DataFilteringWidget;
	UTextBlock* DataFilteringWidgetTextBlock;
	UComboBoxString* DataFilteringWidgetComboBox;

	bool InitializedWidgets = false;
	ADataInteractionPlayerController* DataInteractionPlayerController;
	ADataManager* DataManager;
	ADataInteractionHUD* DataInteractionHUD;

	void CreateAndRenderWidget(FString WidgetName, UUserWidget*& WidgetObject);
	FString GetFriendlyPropertyName(FString PropertyName);

};
