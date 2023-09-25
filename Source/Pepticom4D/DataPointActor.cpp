// Fill out your copyright notice in the Description page of Project Settings.


#include "DataPointActor.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/PrimitiveComponent.h"
#include "PlayerCameraPawn.h"

void ADataPointActor::Create() {
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
    StaticMeshComp->OnClicked.AddDynamic(this, &ADataPointActor::OnMeshClicked);

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

void ADataPointActor::Initialize(FString ADataType, FString AMetaDataTableName, FSpatialDataStruct* ASpatialDataStruct, FTableRowBase* AMetadataRow)
{
    //init variables
    this->DataType = ADataType;
    this->MetaDataTableName = AMetaDataTableName;
    this->SpatialDataStruct = ASpatialDataStruct;
    this->MetadataRow = AMetadataRow;
}

// Sets default values
ADataPointActor::ADataPointActor(): AActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    // Create the actor
    Create();
}

// Called when the game starts or when spawned
void ADataPointActor::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ADataPointActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ADataPointActor::OnMeshClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed) {
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

void ADataPointActor::ChangeColor(FString ColorHex) {
	/* Converts the hex string to a linear color and updates the actor's color. Hex string should be in the format #RRGGBB or RRGGBB */
    // Convert the hex string to a linear color
    FColor Color = FColor::FromHex(ColorHex);
    ChangeColor(Color);
}

void ADataPointActor::ChangeColor(FColor NewColor) {
    // Set the emissive color of the material to be the color
    DynamicMaterial->SetVectorParameterValue(FName("EmissiveColor"), NewColor);
}


void ADataPointActor::ChangeScale(const float NewScale) const {

    // FString Message = FString::Printf(TEXT("Scale foo: %f"), NewScale);
    // UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

    SphereComp->SetRelativeScale3D(FVector(NewScale, NewScale, NewScale));
}


void ADataPointActor::ExtractDataTypes(const TTuple<FString, TSharedPtr<FJsonValue, ESPMode::ThreadSafe>>& MainPair)
{
    
}


FString ADataPointActor::GetDataType() const
{
    return DataType;
}


FSpatialDataStruct* ADataPointActor::GetSpatialDataStruct() const
{
    return SpatialDataStruct;
}

//create getters for the variables
FTableRowBase& ADataPointActor::GetMetadataRow() const
{
    //return the metadata struct
    return *MetadataRow;
}

FString ADataPointActor::GetTableName()
{
    //return the table name
    return this->TableName;
}

FString ADataPointActor::GetMetaDataTableName() const
{
    return this->MetaDataTableName;
}
