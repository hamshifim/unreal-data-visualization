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
#include "AnimationControlWidget.h"
#include "DataPointActor.h"
#include "Types/SlateEnums.h"
#include "DataInteractionHUD.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "UIManager.generated.h"


// Add this outside the class declaration.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnViewChangeDelegate, FString, SelectedItem, ESelectInfo::Type, SelectionType);


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
	void DisplayActorDataWidget(ADataPointActor* Actor);

	UFUNCTION()
	void RefreshActorDataWidget(ADataPointActor* Actor);

	UFUNCTION()
	void HideActorDataWidget();

	UFUNCTION()
	void StartDrawingConnectingLineToActorDataWidget(ADataPointActor* Actor);

	UFUNCTION()
	void StopDrawingConnectingLineToActorDataWidget();

	UFUNCTION()
	void ConfigureDataFilteringWidget();

	UFUNCTION()
	void OnAnimationButtonClick();

	UFUNCTION()
	void OnAnimationSliderChange(float AnimationValue);

	UFUNCTION()
	void OnViewChange(FString SelectedItem, ESelectInfo::Type SelectionType);
	

	UFUNCTION()
	void OnControlSwitchButtonClick();

	UFUNCTION()
	void ConfigureAnimations();

	UFUNCTION()
	void ConfigureAnimationControlWidget();

	UFUNCTION()
	void RefreshDataFilteringWidget();

	UFUNCTION()
	void OnDataFilteringWidgetDropdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	void ConfigureViewSwitchWidget();
	FComboBoxStyle CreateCustomComboBoxStyle();
	void ConfigureDataTypeControlWidget();
	void ConfigureUserControlWidget();

	UFUNCTION()
	void RefreshUI();

	UPROPERTY(BlueprintAssignable)
	FOnViewChangeDelegate OnViewChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* SpawnVolume;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	UUserWidget* ViewNameWidget;
	UPROPERTY()
	UUserWidget* ActorDataWidgetGeneric;
	UPROPERTY()
	UActorDataWidget* ActorDataWidget;
	UPROPERTY()
	UBorder* ActorDataWidgetBorder;
	UPROPERTY()
	UUserWidget* UserControlWidget;
	UPROPERTY()
	UWidgetSwitcher* ControlSwitcherWidget;
	UPROPERTY()
	UDataSelectorWidget* DataSelectorWidget;
	UPROPERTY()
	UUserWidget* DataFilteringWidgetGeneric;
	UPROPERTY()
	UDataFilteringWidget* DataFilteringWidget;
	UPROPERTY()
	UTextBlock* DataFilteringWidgetTextBlock;
	UPROPERTY()
	UComboBoxString* DataFilteringWidgetComboBox;

	bool InitializedWidgets = false;
	UPROPERTY()
	ADataInteractionPlayerController* DataInteractionPlayerController;
	UPROPERTY()
	ADataManager* DataManager;
	UPROPERTY()
	UAAnimationHandler* AnimationHandler;
	UPROPERTY()
	ADataInteractionHUD* DataInteractionHUD;

	UPROPERTY()
	UUserWidget* AnimationControlWidget;
	UPROPERTY()
	USlider* AnimationSlider;
	UPROPERTY()
	UTextBlock* AnimationTextBlock;
	UPROPERTY()
	UUserWidget* ViewSwitchWidget;
	UPROPERTY()
	UUserWidget* DataTypeControlWidget;

	void CreateAndRenderWidget(FString WidgetName, UUserWidget*& WidgetObject, bool AddToViewport = true);
	FString GetFriendlyPropertyName(FString PropertyName);

};
