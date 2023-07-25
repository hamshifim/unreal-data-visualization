// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManager.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

// Sets default values
AUIManager::AUIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
    SpawnVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AUIManager::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("Initializing widgets..."));
    // Store the player controller
    DataInteractionPlayerController = Cast<ADataInteractionPlayerController>(GetWorld()->GetFirstPlayerController());
    // Store the actor spawner - find object of class
    ActorSpawner = Cast<AActorSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AActorSpawner::StaticClass()));
    // Initialize widgets
    CreateAndRenderWidget("/Game/ViewNameWidget.ViewNameWidget_C", ViewNameWidget);
    CreateAndRenderWidget("/Game/ActorDataWidget.ActorDataWidget_C", ActorDataWidgetGeneric);
    InitializedWidgets = true;

    // By default, the actor data widget is hidden
    if (ActorDataWidgetGeneric) 
    {
        // Get the actor data widget
        ActorDataWidget = Cast<UActorDataWidget>(ActorDataWidgetGeneric);
        if (ActorDataWidget) 
        {
            HideActorDataWidget();
        }
        else 
        {
			UE_LOG(LogTemp, Error, TEXT("ActorDataWidget is not of type UActorDataWidget"));
		}
    }
    else 
    {
		UE_LOG(LogTemp, Error, TEXT("ActorDataWidgetGeneric is null"));
	}
}

// Called every frame
void AUIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUIManager::CreateAndRenderWidget(FString WidgetName, UUserWidget*& WidgetObject) {
    UE_LOG(LogTemp, Warning, TEXT("Creating widget %s"), *WidgetName);
    FStringClassReference WidgetClassRef(*WidgetName);
    UClass* WidgetClass = WidgetClassRef.TryLoadClass<UUserWidget>();
    if (WidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("Widget %s class loaded"), *WidgetName);
        WidgetObject = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
        if (WidgetObject) {
            UE_LOG(LogTemp, Warning, TEXT("Widget %s created"), *WidgetName);
            // Set anchors and offsets to put the widget in the top left corner
            WidgetObject->SetAnchorsInViewport(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
            WidgetObject->SetAlignmentInViewport(FVector2D(0.0f, 0.0f));
            WidgetObject->SetPositionInViewport(FVector2D(0.0f, 0.0f));
            // Add the widget to the viewport with z-order 999
            WidgetObject->AddToViewport(999);
            // Check if we have added it to viewport
            if (WidgetObject->IsInViewport()) {
                UE_LOG(LogTemp, Warning, TEXT("Widget %s added to viewport"), *WidgetName);
            }
            else {
                UE_LOG(LogTemp, Warning, TEXT("Widget %s not added to viewport"), *WidgetName)
            }
            // Make sure that the widget is visible
            WidgetObject->SetVisibility(ESlateVisibility::Visible);
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("Unable to create widget: %s"), *WidgetName);
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Unable to load widget class: %s"), *WidgetName);
    }
}

void AUIManager::UpdateText(UUserWidget* TextWidgetObject, FString TextBlockName, FString NewText)
{
    UTextBlock* TextBlock = Cast<UTextBlock>(TextWidgetObject->GetWidgetFromName(*TextBlockName));
    if (TextBlock)
    {
        TextBlock->SetText(FText::FromString(NewText));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Text block with name %s not found"), *TextBlockName);
    }
}

void AUIManager::UpdateViewNameWidget(FString NewViewName) {
    if (ViewNameWidget) {
		UpdateText(ViewNameWidget, "ViewNameText", NewViewName);
	}
    else {
		UE_LOG(LogTemp, Warning, TEXT("View name widget not found in the UI Manager"));
	}
}

void AUIManager::DisplayActorDataWidget(AActor* Actor) {
    FVector ActorLocation = Actor->GetActorLocation();
    FVector2D ScreenPosition;
    if (UGameplayStatics::ProjectWorldToScreen(DataInteractionPlayerController, ActorLocation, ScreenPosition)) {
		// Set the position of the actor data widget
		ActorDataWidget->SetPositionInViewport(ScreenPosition);
        // Show the actor data widget
        ActorDataWidget->AnimateIn(2.0f);
	}
    else {
		UE_LOG(LogTemp, Error, TEXT("Unable to project world to screen"));
	}
}

FString AUIManager::GetFriendlyPropertyName(FString PropertyName) {
    // Convert underscores to spaces
    FString FriendlyPropertyName = PropertyName.Replace(TEXT("_"), TEXT(" "));
    // Make all letters lowercase
    FriendlyPropertyName = FriendlyPropertyName.ToLower();
    // Capitalize the first letter of each word (separated by spaces)
    // Split the string into an array of words
    TArray<FString> Words;
    FriendlyPropertyName.ParseIntoArray(Words, TEXT(" "), true);
    // Iterate through the array of words
    for (int32 i = 0; i < Words.Num(); i++) {
		// Get the word
		FString Word = Words[i];
		// Capitalize the first letter
		Word[0] = FChar::ToUpper(Word[0]);
		// Replace the word in the array
		Words[i] = Word;
	}
    // Join the array of words back into a string
    FriendlyPropertyName = FString::Join(Words, TEXT(" "));
    // Return the friendly property name
    return FriendlyPropertyName;
}

FString AUIManager::GetPropertyValueAsString(FProperty* Property, FTableRowBase& Metadata) {
    FString PropertyName = Property->GetName();
    FString PropertyValue;

    // Check the property type and convert it to a string accordingly
    FString PropertyTypeName = Property->GetClass()->GetName();

    if (PropertyTypeName.Equals("IntProperty"))
    {
        FIntProperty* IntProp = CastField<FIntProperty>(Property);
        int32 Value = IntProp->GetPropertyValue_InContainer(&Metadata);
        PropertyValue = FString::Printf(TEXT("%d"), Value);
    }
    else if (PropertyTypeName.Equals("FloatProperty"))
    {
        FFloatProperty* FloatProp = CastField<FFloatProperty>(Property);
        float Value = FloatProp->GetPropertyValue_InContainer(&Metadata);
        PropertyValue = FString::Printf(TEXT("%f"), Value);
    }
    else if (PropertyTypeName.Equals("DoubleProperty"))
    {
        FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property);
        double Value = DoubleProp->GetPropertyValue_InContainer(&Metadata);
        PropertyValue = FString::Printf(TEXT("%f"), Value);
    }
    else if (PropertyTypeName.Equals("BoolProperty"))
    {
        FBoolProperty* BoolProp = CastField<FBoolProperty>(Property);
        bool Value = BoolProp->GetPropertyValue_InContainer(&Metadata);
        PropertyValue = Value ? "True" : "False";
    }
    else if (PropertyTypeName.Equals("StrProperty"))
    {
        FStrProperty* StrProp = CastField<FStrProperty>(Property);
        FString Value = StrProp->GetPropertyValue_InContainer(&Metadata);
        PropertyValue = Value;
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Struct property %s type not supported. Type: %s"), *PropertyName, *PropertyTypeName);
    }
    
    return PropertyValue;
}

void AUIManager::RefreshActorDataWidget(AActor* Actor) {
    // Get the widget's vertical box, assuming that all text blocks are children of a vertical box
    UVerticalBox* VerticalBox = Cast<UVerticalBox>(ActorDataWidget->GetWidgetFromName("ActorDataVerticalBox"));
    // Get the border, which is the parent of the vertical box
    UBorder* Border = Cast<UBorder>(VerticalBox->GetParent());
    // Clear all of the existing fields from the widget. 
    VerticalBox->ClearChildren();
    // Get the metadata from the actor
    FTableRowBase& Metadata = ActorSpawner->GetMetadataFromActor(Actor);
    // Get a pointer to the actor's dynamic struct
    UStruct* MetadataType = FTableRowBase::StaticStruct();
    FString MetadataStructName = ActorSpawner->GetStructNameFromFullDatasetName(ActorSpawner->GetCurrentFullDatasetName());
    UScriptStruct* MetadataStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *MetadataStructName);
    if (MetadataStruct) {
        MetadataType = Cast<UStruct>(MetadataStruct);
        if (!MetadataType) {
            UE_LOG(LogTemp, Warning, TEXT("Could not cast metadata struct to UStruct: %s"), *MetadataStructName)
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Could not load class for metadata parsing: %s"), *MetadataStructName);
    }
    // Iterate through all properties of the struct and extract the property name and value
    for (TFieldIterator<FProperty> PropertyIt(MetadataType); PropertyIt; ++PropertyIt)
    {
        FProperty* Property = *PropertyIt;
        FString PropertyName = Property->GetName();
        FString PropertyValue = GetPropertyValueAsString(Property, Metadata);

        // Create a text block
        UTextBlock* TextBlock = NewObject<UTextBlock>(VerticalBox);
        if (TextBlock)
        {
            // Set the text to be "Property Name: Property Value"
            FText Text = FText::FromString(GetFriendlyPropertyName(PropertyName) + ": " + PropertyValue);
            TextBlock->SetText(Text);
            
            // Set the text's font size
            TextBlock->Font.Size = 14;

            // Add the text block to the VerticalBox
            VerticalBox->AddChildToVerticalBox(TextBlock);
        }
    }

    // Wait a small amount of time before resizing the parent canvas
    // This is necessary because the vertical box's size is not updated immediately after adding children
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Border, VerticalBox]()
    {
        // After creating all text blocks, resize the parent canvas based on the size of the vertical box
        // Get the size of the vertical box
        FVector2D VerticalBoxSize = VerticalBox->GetDesiredSize();
        if (Border) {
            // Get the parent of the vertical box, which is the canvas
            UCanvasPanelSlot* BorderSlot = Cast<UCanvasPanelSlot>(Border->Slot);
            // Check if we found the slot
            if (BorderSlot)
            {
                // Set the size of the parent (canvas) to be the size of the vertical box
                BorderSlot->SetSize(VerticalBoxSize);
            }
            else {
                UE_LOG(LogTemp, Warning, TEXT("Unable to find canvas panel as a parent for border of the UI"));
            }
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("Unable to find border of the UI"));
        }
    }, 0.1f, false);
}

void AUIManager::HideActorDataWidget() {
    if (ActorDataWidget) {
        // Hide the actor data widget
        ActorDataWidget->SetVisibility(ESlateVisibility::Hidden);
	}
    else {
		UE_LOG(LogTemp, Warning, TEXT("Actor data widget not found in the UI Manager"));
	}
}

