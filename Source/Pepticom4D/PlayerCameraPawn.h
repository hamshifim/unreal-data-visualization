// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ViewPositionStruct.h"
#include "UIManager.h"
#include "PlayerCameraPawn.generated.h"

UCLASS()
class PEPTICOM4D_API APlayerCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerCameraPawn();

	UFUNCTION()
	void GenerateViewsFromPOIDataTable();

	UFUNCTION()
	void SwitchToNextView();

	UFUNCTION()
	void SaveView(FVector& ViewCenterOffset, int ViewNumber, FString ViewName);

	UFUNCTION()
	void SaveCurrentView();

	UFUNCTION()
	double CalculateFOV();

	UFUNCTION()
	FString GetCurrentViewName();

	UFUNCTION()
	void MoveCameraToActor(ADataPointActor* Target);

	UFUNCTION()
	void ResetCameraZoomAndRotation(double RadiusOfTarget, FVector CenterOfTarget, bool Animate);

	UFUNCTION()
	void RestoreDefaults(bool AbandonTarget);

	UFUNCTION()
	void UpdateCurrentViewName(FString NewViewName);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComp;

	FVector MovementInput;
	FVector2D CameraInput;
	float ZoomFactor;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void MoveUp(float AxisValue);
	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);
	void ZoomIn(float AxisValue);

	void StoreViewSphereCenterAndRadius();
	void AdjustMovementSpeedFromRadius();
	void LoadViewsToMapFromDataTable();

	bool bIsResettingCamera = false;
	FVector TargetResetCameraLocation;
	FRotator TargetResetSpringArmRotation;

	float BaseMovementSpeed = 1000.0f;
	float MovementSpeed = BaseMovementSpeed;
	float MovementToTargetSpeed = 2.0f;
	float ZoomSpeed = 100.0f;
	float ViewPaddingFactor = 4.0f;
	float FOVPaddingFactor = 2.0f;
	int CurrentViewNumber = -1;
	FString CurrentViewName;

	// Store the center and radius of the bounding sphere for the current data set
	FVector Center;
	double Radius;
	// Store the field of view to which the camera should be set for the current data set
	double FOV;
	// Store views in a map for O(1) lookup. Uni-directional; used for reading from the data table, but not writing to it. Important.
	TMap<int, FViewPositionStruct*> ViewNumberToRowMap;

	// Store a target towards which the camera should move
	UPROPERTY()
	ADataPointActor* TargetActor = nullptr;

	// Store the UI Manager
	UPROPERTY()
	class AUIManager* UIManager;

};
