// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManager.h"

// Sets default values
AUIManager::AUIManager(): AActor()
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
    // Store the data manager
    DataManager = Cast<ADataManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADataManager::StaticClass()));
    // Store the HUD
    DataInteractionHUD = Cast<ADataInteractionHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
    // Initialize widgets
    CreateAndRenderWidget("/Game/ViewNameWidget.ViewNameWidget_C", ViewNameWidget);
    CreateAndRenderWidget("/Game/ActorDataWidget.ActorDataWidget_C", ActorDataWidgetGeneric);
    CreateAndRenderWidget("/Game/DataSelectorWidget.DataSelectorWidget_C", DataSelectorWidgetGeneric);
    CreateAndRenderWidget("/Game/DataFilteringWidget.DataFilteringWidget_C", DataFilteringWidgetGeneric);
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

    // Get the data selector widget from its generic version
    if (DataSelectorWidgetGeneric)
    {
		DataSelectorWidget = Cast<UDataSelectorWidget>(DataSelectorWidgetGeneric);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("DataSelectorWidgetGeneric is null"));
    }

    // Get the data filtering widget from its generic version
    if (DataFilteringWidgetGeneric)
    {
        DataFilteringWidget = Cast<UDataFilteringWidget>(DataFilteringWidgetGeneric);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("DataFilteringWidgetGeneric is null"));
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

void AUIManager::DisplayActorDataWidget(ADataPointActor* Actor) {
    FVector ActorLocation = Actor->GetActorLocation();
    FVector2D ScreenPosition;
    if (UGameplayStatics::ProjectWorldToScreen(DataInteractionPlayerController, ActorLocation, ScreenPosition)) {
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

void AUIManager::RefreshActorDataWidget(ADataPointActor* DataPointActor) {

    // cast Actor to ADataPointActor
    
    // Get the widget's vertical box, assuming that all text blocks are children of a vertical box
    UVerticalBox* VerticalBox = Cast<UVerticalBox>(ActorDataWidget->GetWidgetFromName("ActorDataVerticalBox"));
    // Get the border, which is the parent of the vertical box
    ActorDataWidgetBorder = Cast<UBorder>(VerticalBox->GetParent());
    // Clear all of the existing fields from the widget. 
    VerticalBox->ClearChildren();
    // Get the metadata from the actor
    FTableRowBase& Metadata = DataPointActor->GetMetadataStruct();
    // Get the data type from the actor
    UStruct* MetadataStruct = DataManager->GetMetadataStructFromActor(DataPointActor);
    // Iterate through all properties of the struct and extract the property name and value
    for (TFieldIterator<FProperty> PropertyIt(MetadataStruct); PropertyIt; ++PropertyIt)
    {
        FProperty* Property = *PropertyIt;
        FString PropertyName = Property->GetName();
        FString PropertyValue = DataManager->GetPropertyValueAsString(Property, Metadata);

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
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, VerticalBox]()
    {
        // After creating all text blocks, resize the parent canvas based on the size of the vertical box
        // Get the size of the vertical box
        FVector2D VerticalBoxSize = VerticalBox->GetDesiredSize();
        if (ActorDataWidgetBorder) {
            // Get the parent of the vertical box, which is the canvas
            UCanvasPanelSlot* BorderSlot = Cast<UCanvasPanelSlot>(ActorDataWidgetBorder->Slot);
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

void AUIManager::StartDrawingConnectingLineToActorDataWidget(ADataPointActor* Actor) {
    // Set the start point of the line to be the center of the ActorDataWidget Border
    FVector2D LineStart = ActorDataWidgetBorder->GetCachedGeometry().GetAbsolutePosition() + ActorDataWidgetBorder->GetCachedGeometry().GetAbsoluteSize() / 2.0f;
    // Tell the HUD to draw a line (and keep updating it) from the actor to the actor data widget
    DataInteractionHUD->LineStart = LineStart;
    DataInteractionHUD->ActorToDrawLineTo = Actor;
}

void AUIManager::StopDrawingConnectingLineToActorDataWidget() {
    // Tell the HUD to stop drawing the line
    // DataInteractionHUD->LineStart = FVector2D::ZeroVector;
    DataInteractionHUD->ActorToDrawLineTo = nullptr;
}

void AUIManager::ConfigureDataSelectorWidget() {
    /* Add options to the data selector widget from the available datasets, bind events to functions */
    // Initialize variables
    if (DataSelectorWidget) {
        // Generate horizontal boxes containing the data type and a combo box for the sub-dataset name
        // Get the vertical box which will contain all of these horizontal boxes
        UVerticalBox* VerticalBox = Cast<UVerticalBox>(DataSelectorWidget->GetWidgetFromName("DataSelectorVerticalBox"));

        // get a ViewHandler from DataManager using its current view Name
        UAViewHandler* ViewHandler = DataManager->ViewHandlerMap.FindRef(DataManager->CurrentViewName);
        
        // Iterate through all data types
        for (const FString& DataType : ViewHandler->GetDataTypes()) {
            // Create a horizontal box element inside the vertical box
            UHorizontalBox* HorizontalBox = NewObject<UHorizontalBox>(VerticalBox);
            // Add the horizontal box to the vertical box
            UVerticalBoxSlot* VerticalBoxSlot = VerticalBox->AddChildToVerticalBox(HorizontalBox);
            // Give the horizontal box a lower padding of 10
            FMargin HorizontalBoxPadding = FMargin(0.0f, 0.0f, 0.0f, 10.0f);
            VerticalBoxSlot->SetPadding(HorizontalBoxPadding);
            // Create a text block for the data type inside the horizontal box
            UTextBlock* TextBlock = NewObject<UTextBlock>(HorizontalBox);
            // Set the text to be the data type
            TextBlock->SetText(FText::FromString(DataType));
            // Set the text's font size
            TextBlock->Font.Size = 14;
            // Set the name of the text block to be the data type + "TextBlock"
            FString TextBlockName = DataType + "TextBlock";
            // Add the text block to the list of text blocks
            DataSelectorWidgetDataTypeNamesMap.Add(TextBlockName, TextBlock);
            // Add the text block to the horizontal box
            HorizontalBox->AddChildToHorizontalBox(TextBlock);
            // Create a combo box for the sub-dataset name inside the horizontal box
            UComboBoxString* ComboBox = NewObject<UComboBoxString>(HorizontalBox);
            // Set the name of the combo box to be the data type + "ComboBox"
            FString ComboBoxName = DataType + "ComboBox";
            // Add the combo box to the list of combo boxes
            DataSelectorWidgetTableNameComboBoxesMap.Add(ComboBoxName, ComboBox);
            // Add the combo box to the horizontal box
            UHorizontalBoxSlot* HorizontalBoxSlot = HorizontalBox->AddChildToHorizontalBox(ComboBox);
            // Give the combo box a left padding of 10
            FMargin ComboBoxPadding = FMargin(10.0f, 0.0f, 0.0f, 0.0f);
            HorizontalBoxSlot->SetPadding(ComboBoxPadding);
            // Set the font size of the combo box
            ComboBox->Font.Size = 12;
            // Add the sub-dataset names to the combo box
            for (const FString& SubDatasetName : DataManager->DataTypeToTableNamesMap.FindRef(DataType)) {
                // Add the sub-dataset name to the combo box
                ComboBox->AddOption(SubDatasetName);
            }
            // Bind to the combo box's OnSelectionChanged event
            ComboBox->OnSelectionChanged.AddDynamic(this, &AUIManager::OnDataSelectorWidgetDropdownChanged);
            // Set the default selected item
            FString DefaultSelectedItem = DataManager->CurrentDataTypeNameToTableNameMap.FindRef(DataType);
            ComboBox->SetSelectedOption(DefaultSelectedItem);
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Data selector widget not found in the UI Manager"));
    }
}

void AUIManager::OnDataSelectorWidgetDropdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType) {
    // PLACEHOLDER
 
    //// Update the current full dataset name
    //DataManager->CurrentFullDatasetName = SelectedItem;
    //// 
}

void AUIManager::RefreshDataSelectorWidget() {
    // PLACEHOLDER

  //  if (DataSelectorWidget) {
  //      // Iterate through all data types
  //      for (const FString& DataType : DataManager->ViewNameToDataTypesMap[DataManager->CurrentViewName]) {
		//	// Get the text block for the data type
  //          FString TextBlockName = DataType + "TextBlock";
  //          UTextBlock* TextBlock = DataSelectorWidgetDataTypeNamesMap[TextBlockName];
  //          //
		//}
  //  }
  //  else {
  //      UE_LOG(LogTemp, Error, TEXT("Data selector widget not found in the UI Manager"));
  //  }
}

void AUIManager::ConfigureDataFilteringWidget() {
    if (DataFilteringWidget) {
        // Get the text block for this widget
        DataFilteringWidgetTextBlock = Cast<UTextBlock>(DataFilteringWidget->GetWidgetFromName("ColorDataByPropertyTextBlock"));
        // Get the combo box for this widget
        DataFilteringWidgetComboBox = Cast<UComboBoxString>(DataFilteringWidget->GetWidgetFromName("ColorDataByPropertyComboBox"));
        // Add the options to the combo box
        FString DefaultOption = "Default";
        DataFilteringWidgetComboBox->AddOption(DefaultOption);
        DataFilteringWidgetComboBox->SetSelectedOption(DefaultOption);
        // Add options to the combo box, if they exist

        // get a ViewHandler from DataManager using its current view Name
        UAViewHandler* ViewHandler = DataManager->ViewHandlerMap.FindRef(DataManager->CurrentViewName);
        
        if (ViewHandler) {
            for (const auto& PropertyNameMap : *ViewHandler->GetColorMap()) {
                FString PropertyName = PropertyNameMap.Key;
                DataFilteringWidgetComboBox->AddOption(PropertyName);
            }
        }
        // Bind the combo box to the OnSelectionChanged event
        DataFilteringWidgetComboBox->OnSelectionChanged.AddDynamic(this, &AUIManager::OnDataFilteringWidgetDropdownChanged);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Data filtering widget not found in the UI Manager"));
    }
}

void AUIManager::RefreshDataFilteringWidget() {
    // PLACEHOLDER
}

void AUIManager::OnDataFilteringWidgetDropdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType) {
    // PLACEHOLDER
    // Update the text on the text block

    // use ternary operator to set the text to be displayed
    FString Text = SelectedItem.Equals("Default") ? "Default Coloring" : "Coloring by " + SelectedItem;
    
    DataFilteringWidgetTextBlock->SetText(FText::FromString(Text));
    // Create a map of data type to a boolean - whether or not the data type supports coloring by the selected property
    TMap<FString, bool> DataTypeToSupportsColoringMap = TMap<FString, bool>();
    // Iterate through all actors
    for (const auto& ActorDataPair : DataManager->ActorToSpatialDataMap) {
        ADataPointActor* DataPointActor = ActorDataPair.Key;
        if (SelectedItem.Equals("Default")) {
            // Apply coloring from spatial data
            // Get the actor's spatial data
            FSpatialDataStruct& SpatialData = DataManager->GetSpatialDataFromActor(DataPointActor);
            // Get the color to set on the actor
            FColor NewColor = FColor::FromHex(SpatialData.color);
            // Cast the actor to an ActorToSpawn
            // Set the actor's color
            DataPointActor->ChangeColor(NewColor);
        }
        else {
            bool DataTypeSupportsColoring;
            // Get the actor's data type
            FString ActorDataType = DataPointActor->GetDataType();
            // Check if the data type is already in the map
            if (DataTypeToSupportsColoringMap.Contains(ActorDataType)) {
                // Check if the data type supports coloring by the selected property
                DataTypeSupportsColoring = DataTypeToSupportsColoringMap.FindRef(ActorDataType);
            }
            else {
                // Check if the actor's data type supports coloring by the selected property
                DataTypeSupportsColoring = DataManager->ActorHasMetadataProperty(DataPointActor, SelectedItem);
                // Add the data type to the map
                DataTypeToSupportsColoringMap.Add(ActorDataType, DataTypeSupportsColoring);
            }
            // Apply coloring to the actor if the actor supports coloring by the selected property
            if (DataTypeSupportsColoring) {
                // Get the actor's metadata
                FTableRowBase& Metadata = DataPointActor->GetMetadataStruct();
                // Get the metadata struct from the actor
                UStruct* MetadataStruct = DataManager->GetMetadataStructFromActor(DataPointActor);
                // Get the value of the selected property from the actor's metadata
                FString PropertyValue = DataManager->GetPropertyValueStringFromMetadata(Metadata, MetadataStruct, SelectedItem);
                // Get the color from the color map based on the property value

                UAViewHandler* ViewHandler = DataManager->ViewHandlerMap.FindRef(DataManager->CurrentViewName);

                auto ValueColorMap = ViewHandler->GetColorMap()->FindRef(SelectedItem);
                if (ValueColorMap.Contains(PropertyValue)) {
                    FColor NewColor = ValueColorMap[PropertyValue];
                    // Cast the actor to an ActorToSpawn
                    ADataPointActor* ActorToSpawn = Cast<ADataPointActor>(DataPointActor);
                    // Set the actor's color
                    ActorToSpawn->ChangeColor(NewColor);
                }
                else {
                    UE_LOG(LogTemp, Error, TEXT("Color map for the current view does not contain a color for the property value %s for property name %s"), *PropertyValue, *SelectedItem);
                    return;
                }
            }
        }
    }
}

