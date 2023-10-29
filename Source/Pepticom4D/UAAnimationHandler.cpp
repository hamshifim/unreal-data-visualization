#include "UAAnimationHandler.h"
#include "UATableHandler.h"
#include "Components/TextBlock.h"


void UAAnimationHandler::Initialize(FString AAnimationName, FString AAnimationDimension, int32 AMin, int32 AMax, int32 AInterval, FString ADataType, FString AActorTableName, FString AManyToOneTableName, FString AKeyRegex, TArray<FString> ARegexVariableNames, TArray<FString> AUpdateProperties, TMap<FString, UADataTypeHandler*>* ADataTypeHandlerMap)
{
	this->AnimationName = AAnimationName;
	this->AnimationDimension = AAnimationDimension;
	this->Min = AMin;
	this->Max = AMax;
	this->Interval = AInterval;
	this->DataType = ADataType;
	this->ActorTableName = AActorTableName;
	this->OneToManyTableName = AManyToOneTableName;
	this->KeyRegex = AKeyRegex;
	this->RegexVariableNames = ARegexVariableNames;
	this->UpdateProperties = AUpdateProperties;
	this->DataTypeHandlerMap = ADataTypeHandlerMap;
	this->AnimationValue = FString::Printf(TEXT("%d"), Max);

	UADataTypeHandler* DataTypeHandler = DataTypeHandlerMap->FindRef(DataType);
	UATableHandler* TableHandler = DataTypeHandler->GetTableHandler(ActorTableName);
	MetaDataStruct = TableHandler->GetDataTable()->RowStruct;

	//Iterate over RegexVariableNames and add the corresponding FProperty* to KeyProperties
	for (const auto& RegexVariableName : RegexVariableNames)
	{
		FProperty* Property = MetaDataStruct->FindPropertyByName(*RegexVariableName);
		KeyProperties.Add(RegexVariableName, Property);
	}

	UATableHandler* ManyToOneTableHandler = DataTypeHandler->GetManyToOneTableHandler(OneToManyTableName);
	ManyToOneStruct = ManyToOneTableHandler->GetDataTable()->RowStruct;

	this->PossibleAnimationValues = GetPossibleAnimationValues();
	this->CurrentAnimationIndex = 0;
}

void UAAnimationHandler::Sanity() 
{
	UE_LOG(LogTemp, Display, TEXT("Ashmagog you bastard: %s."), *this->AnimationName);
}


//iterate from Min to Max adding Interval adding values to array and return it
TArray<int32> UAAnimationHandler::GetPossibleAnimationValues()
{
	TArray<int32> PPossibleAnimationValues;
	for (int32 i= Min; i <= Max; i+=Interval)
	{
		PPossibleAnimationValues.Add(i);
		UE_LOG(LogTemp, Display, TEXT("googalach: %d"), i);
	}

	return PPossibleAnimationValues;
}


FString UAAnimationHandler::GetManyToOneTableName()
{
	return OneToManyTableName;
}

//TODO make sure the redundancy with the same method in DataManager is resolved
FString UAAnimationHandler::GetPropertyValueAsString(FProperty* Property, const FTableRowBase& Metadata)
{
	FString PropertyName = Property->GetName();
	FString PropertyValue = "";

	// Check the property type and convert it to a string accordingly
	FString PropertyTypeName = Property->GetClass()->GetName();

	UE_LOG(LogTemp, Display, TEXT("Baloo Property: %s Type: %s"), *PropertyName, *PropertyTypeName);

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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Struct property %s type not supported. Type: %s"), *PropertyName, *PropertyTypeName);
	}

	return PropertyValue;
}


void UAAnimationHandler::AnimateActors()
{
	FString Message = GetAnimationName() + " " + AnimationValue;
	AnimationTextBlock->SetText(FText::FromString(Message));
	
	UADataTypeHandler* DataTypeHandler = DataTypeHandlerMap->FindRef(this->DataType);
	
	UATableHandler* ManyToOneTableHandler = DataTypeHandler->GetManyToOneTableHandler(this->OneToManyTableName);

	UE_LOG(LogTemp, Display, TEXT("Shikor 0 AnimationDimension: %s"), *AnimationDimension);

	for(ADataPointActor* DataPointActor: DataTypeHandler->GetDataPointActors())
	{

		UE_LOG(LogTemp, Display, TEXT("Zroobabvel 0 AnimationDimension: %s"), *AnimationDimension);

		if(IsValid(DataPointActor))
		{
			FTableRowBase& MetaDataRow = DataPointActor->GetMetadataRow();

		// Iterate through all properties of the struct and extract the property name and value
		// for (TFieldIterator<FProperty> PropertyIt(MetaDataStruct); PropertyIt; ++PropertyIt)
		// {
		// 	FProperty* Property = *PropertyIt;
		// 	FString PropertyName = Property->GetName();
		// 	FString PropertyValue = GetPropertyValueAsString(Property, MetaDataRow);
		//
		// 	UE_LOG(LogTemp, Display, TEXT("Pizza Property %s: %s"), *PropertyName,*PropertyValue);
		// }
		
		TArray<FVarStruct> Variables;
		FString Index = DataPointActor->GetIndex();
		UE_LOG(LogTemp, Display, TEXT("Zroobabvel 1 Index: %s"), *Index);
		Variables.Add(FVarStruct("Index", Index));
		Variables.Add(FVarStruct(AnimationDimension, AnimationValue));
		
		// iterate over KeyProperties and set the corresponding property in MetaDataRow
		 for (const auto& KeyProperty : KeyProperties)
		 {
		 	FString PropertyName = KeyProperty.Key;
		 	FProperty* Property = KeyProperty.Value;
		 	FString PropertyValue = GetPropertyValueAsString(Property, MetaDataRow);

		 	Variables.Add(FVarStruct(PropertyName, PropertyValue));

		 	UE_LOG(LogTemp, Display, TEXT("Zroobabvel 2 Property: %s: %s"), *PropertyName, *PropertyValue);
		 	FTableRowBase* ManyToOneRow = ManyToOneTableHandler->GetTableRow(Variables);
		 	UE_LOG(LogTemp, Display, TEXT("Zroobabvel 3 "));

		 	//Iterate over UpdateProperties
		 	for(FString UpdatePropName: UpdateProperties)
		 	{
		 		UE_LOG(LogTemp, Display, TEXT("Zuchini 0 Property: %s"), *UpdatePropName);

		 		FProperty* UpdateProp = ManyToOneStruct->FindPropertyByName(*UpdatePropName);
		 		FString UpdatePropValue = GetPropertyValueAsString(UpdateProp, *ManyToOneRow);

		 		UE_LOG(LogTemp, Display, TEXT("Zuchini 1 Property: %s: %s"), *UpdatePropName, *UpdatePropValue);

		 		if(UpdatePropName.Equals("size"))
		 		{
		 			UE_LOG(LogTemp, Display, TEXT("Zuchini 2 attempting to change size"));

		 			FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(UpdateProp);
		 			UE_LOG(LogTemp, Display, TEXT("Zuchini 2.1"));
		 			double Radius = DoubleProp->GetPropertyValue_InContainer(ManyToOneRow);

		 			UE_LOG(LogTemp, Display, TEXT("Zuchini 2.2"));
		 			FString Gomes = FString::Printf(TEXT("%f"), Radius);
		 			UE_LOG(LogTemp, Display, TEXT("Zuchini 2.3: %s"), *Gomes);
		 			
		 			DataPointActor->ChangeScale(Radius);
		 			UE_LOG(LogTemp, Display, TEXT("Zuchini 2.4: %s"), *Gomes);
		 		}
		 		else if(UpdatePropName.Equals("color"))
		 		{
		 			UE_LOG(LogTemp, Display, TEXT("Zuchini 2 attempting to change color"));
		 			
		 			FStrProperty* StrProp = CastField<FStrProperty>(UpdateProp);
		 			FString ColorHex = StrProp->GetPropertyValue_InContainer(&ManyToOneRow);
		 			DataPointActor->ChangeColor(ColorHex);
		 		}
		 		else
		 		{
		 			UE_LOG(LogTemp, Warning, TEXT("ADataPointActor does not support PropertyName: %s"), *PropertyName);
		 		}

		 		UE_LOG(LogTemp, Display, TEXT("Zuchini 3"));
		 	}
		 }
		}
		else
		{
			//WARN the actor is not valid
			UE_LOG(LogTemp, Warning, TEXT("DataPointActor is not valid"));
		}
	}
	

	//TODO use the data to animate the actors
	// const UDataTable* ManyToOneTable = ManyToOneTableHandler->GetDataTable();
	//
	// TArray<FName> SpatialMetadataRowNames = ManyToOneTable->GetRowNames();
	//
	// for (const auto& SpatialMetadataRowName : SpatialMetadataRowNames)
	// {
	// 	UE_LOG(LogTemp, Display, TEXT("Balbook SpatialMetadataRowName: %s"), *SpatialMetadataRowName.ToString());
	// }
	//
	// UE_LOG(LogTemp, Display, TEXT("Kadlaomer 3"));
}

void UAAnimationHandler::Animate()
{
	CurrentAnimationIndex = 0;

	// Start the animation process by triggering the first step
	GetWorld()->GetTimerManager().SetTimer(AnimationTimerHandle, this, &UAAnimationHandler::AnimateStep, Interval, true);
}

void UAAnimationHandler::AnimateStep()
{
	if (CurrentAnimationIndex < PossibleAnimationValues.Num())
	{
		int32 i = PossibleAnimationValues[CurrentAnimationIndex++];
		const FString AnimationValueString = FString::Printf(TEXT("%d"), i);
		UE_LOG(LogTemp, Display, TEXT("Pitsootz: AnimationValueString: %s"), *AnimationValueString);
		this->AnimationValue = *AnimationValueString;
		AnimateActors();
	}
	else
	{
		// Stop the timer when all animation steps have been completed
		GetWorld()->GetTimerManager().ClearTimer(AnimationTimerHandle);
	}
}

void UAAnimationHandler::OnAnimationValueChanged(FString AAnimationValue)
{
	this->AnimationValue = AAnimationValue;
	
	//init an array of FVarStruct

	UE_LOG(LogTemp, Display, TEXT("Wowfull AnimationDimension: %s"), *AnimationDimension);
	AnimateActors();
	UE_LOG(LogTemp, Display, TEXT("Bombardful AnimationDimension: %s"), *AnimationDimension);
}

void UAAnimationHandler::LoadData()
{
	UADataTypeHandler* DataTypeHandler = DataTypeHandlerMap->FindRef(this->DataType);
	UATableHandler* TableHandler = DataTypeHandler->GetManyToOneTableHandler(this->OneToManyTableName);
	TableHandler->AddDataToDataTableFromSource();
}

float UAAnimationHandler::GetMinValue()
{
	return Min * 1.0f;
}

float UAAnimationHandler::GetMaxValue()
{
	return Max * 1.0f;
}

float UAAnimationHandler::GetInterval()
{
	return Interval * 1.0f;
}

FString UAAnimationHandler::GetAnimationName()
{
	return this->AnimationName;
}

void UAAnimationHandler::SetAnimationTextBlock(UTextBlock* AAnimationTextBlock)
{
	this->AnimationTextBlock = AAnimationTextBlock;

	const FString Message = GetAnimationName() + " " + AnimationValue;
	AnimationTextBlock->SetText(FText::FromString(Message));
}

