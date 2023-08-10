// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorToSpawn.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/PrimitiveComponent.h"
#include "PlayerCameraPawn.h"

void AActorToSpawn::Create() {
    // Creating our Default Components
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
    StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
    SetRootComponent(SphereComp);

    // Attaching the Components
    SphereComp->SetupAttachment(RootComponent);
    StaticMeshComp->AttachToComponent(SphereComp, FAttachmentTransformRules::KeepRelativeTransform);
    ParticleComp->AttachToComponent(StaticMeshComp, FAttachmentTransformRules::KeepRelativeTransform);

    // Set collision to query only
    StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // Block visibility trace response
    StaticMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
     
    // Force the actor to stay fixed
    StaticMeshComp->Mobility = EComponentMobility::Movable;

    // Allow the actor to be clicked
    StaticMeshComp->OnClicked.AddDynamic(this, &AActorToSpawn::OnMeshClicked);

    // Using Constructor Helpers to set our Static Mesh Comp with a Sphere Shape.
    static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
    StaticMeshComp->SetStaticMesh(SphereMeshAsset.Object);

    // Use a material to give the actor an inner glow
    static ConstructorHelpers::FObjectFinder<UMaterial> BaseMaterialAsset(TEXT("Material'/Game/Materials/BaseEmissiveMaterial.BaseEmissiveMaterial'"));
    if (BaseMaterialAsset.Succeeded()) {
        // Create a dynamic material instance for the mesh
        DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterialAsset.Object, StaticMeshComp);
        if (DynamicMaterial) {
			// Set the emissive color of the material to be red
			DynamicMaterial->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor::White);
            // Set the material on the mesh
			StaticMeshComp->SetMaterial(0, DynamicMaterial);
		}
	}

    // Using Constructor Helpers to set our Particle Comp with our Fire Particle Comp.
    //static ConstructorHelpers::FObjectFinder<UParticleSystem>ParticleCompAsset(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Fire.P_Fire'"));
    //ParticleComp->SetTemplate(ParticleCompAsset.Object);
}

// Sets default values
AActorToSpawn::AActorToSpawn()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    // Create the actor
    Create();
}

// Called when the game starts or when spawned
void AActorToSpawn::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AActorToSpawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AActorToSpawn::OnMeshClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed) {
    // Get the player camera pawn
    APlayerCameraPawn* PlayerCameraPawn = Cast<APlayerCameraPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (PlayerCameraPawn) {
        // Move the camera to the actor
        PlayerCameraPawn->MoveCameraToActor(this);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Unable to get player camera pawn from actor to spawn"));
    }
}

void AActorToSpawn::ChangeColor(FString ColorHex) {
	/* Converts the hex string to a linear color and updates the actor's color. Hex string should be in the format #RRGGBB or RRGGBB */
    // Convert the hex string to a linear color
    FColor Color = FColor::FromHex(ColorHex);
    ChangeColor(Color);
}

void AActorToSpawn::ChangeColor(FColor NewColor) {
    // Set the emissive color of the material to be the color
    DynamicMaterial->SetVectorParameterValue(FName("EmissiveColor"), NewColor);
}


void AActorToSpawn::ChangeScale(const float NewScale) const {

    // FString Message = FString::Printf(TEXT("Scale foo: %f"), NewScale);
    // UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

    SphereComp->SetRelativeScale3D(FVector(NewScale, NewScale, NewScale));
}