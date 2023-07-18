// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorToSpawn.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/PrimitiveComponent.h"
#include "PlayerCameraPawn.h"

void AActorToSpawn::Create() {
    //Creating our Default Components
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
    StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
    SetRootComponent(SphereComp);

    //Attaching the Components
    SphereComp->SetupAttachment(RootComponent);
    StaticMeshComp->AttachToComponent(SphereComp, FAttachmentTransformRules::KeepRelativeTransform);
    ParticleComp->AttachToComponent(StaticMeshComp, FAttachmentTransformRules::KeepRelativeTransform);

    //Setting the Sphere radius to be of a smaller size in line with the Static Mesh.
    SphereComp->SetSphereRadius(Size); // was 16

    //Setting the Static Mesh Scale and Location to fit the radius of the Sphere.
    StaticMeshComp->SetRelativeLocation(FVector(0.0, 0.0, -3.0f)); // was 0, 0, -12
    StaticMeshComp->SetRelativeScale3D(FVector(0.25, 0.25, 0.25));

    // Set collision to query only
    StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // Block visibility trace response
    StaticMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
     
    // Force the actor to stay fixed
    StaticMeshComp->Mobility = EComponentMobility::Movable;

    // Allow the actor to be clicked
    StaticMeshComp->OnClicked.AddDynamic(this, &AActorToSpawn::OnMeshClicked);

    //Using Constructor Helpers to set our Static Mesh Comp with a Sphere Shape.
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

    /*
    //Using Constructor Helpers to set our Particle Comp with our Fire Particle Comp.
    static ConstructorHelpers::FObjectFinder<UParticleSystem>ParticleCompAsset(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Fire.P_Fire'"));
    ParticleComp->SetTemplate(ParticleCompAsset.Object);
    */
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
    // Highlight the actor by making it blue
    DynamicMaterial->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor::Blue);

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
    UE_LOG(LogTemp, Warning, TEXT("Changing color to %s"), *ColorHex)
    // Convert the hex string to a linear color
    FColor Color = FColor::FromHex(ColorHex);
	// Set the emissive color of the material to be the color
	DynamicMaterial->SetVectorParameterValue(FName("EmissiveColor"), Color);
}

void AActorToSpawn::ChangeSize(float NewSize) {
    // PLACEHOLDER: Does not work yet

	/* Updates the actor's size. Size can be any positive number. */
    UE_LOG(LogTemp, Warning, TEXT("Changing size to %f"), NewSize)
    // Scale the actor by the new size. Scale will not be between 0 and 1
    SphereComp->SetSphereRadius(NewSize);

    // TODO: Setting the Static Mesh Scale and Location to fit the radius of the Sphere.
    // ...

    // Update the size
    Size = NewSize;
}