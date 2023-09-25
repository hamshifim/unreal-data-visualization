#include "UAAnimationHandler.h"
#include "UATableHandler.h"


// An Initialization of the necessary variables
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
}

void UAAnimationHandler::Sanity() 
{
	UE_LOG(LogTemp, Display, TEXT("Ashmagog you bastard: %s."), *this->AnimationName);
}


//iterate from Min to Max adding Interval adding values to array and return it
TArray<int32> UAAnimationHandler::GetPossibleAnimationValues()
{
	TArray<int32> PossibleAnimationValues;
	for (int32 i= Min; i <= Max; i+=Interval)
	{
		PossibleAnimationValues.Add(i);
		UE_LOG(LogTemp, Display, TEXT("googalach: %d"), i);
	}

	return PossibleAnimationValues;
}


FString UAAnimationHandler::GetManyToOneTableName()
{
	return OneToManyTableName;
}


FString UAAnimationHandler::GetPropertyValueAsString(FProperty* Property, const FTableRowBase& Metadata)
{
	FString PropertyName = Property->GetName();
	FString PropertyValue = "";

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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Struct property %s type not supported. Type: %s"), *PropertyName,
			   *PropertyTypeName);
	}

	return PropertyValue;
}


void UAAnimationHandler::AnimateActors()
{
	UADataTypeHandler* DataTypeHandler = DataTypeHandlerMap->FindRef(this->DataType);
	
	UATableHandler* ManyToOneTableHandler = DataTypeHandler->GetManyToOneTableHandler(this->OneToManyTableName);

	for(ADataPointActor* DataPointActor: DataTypeHandler->GetDataPointActors())
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
		UE_LOG(LogTemp, Display, TEXT("Zroobabvel 0 Index: %s"), *Index);
		Variables.Add(FVarStruct("Index", Index));
		Variables.Add(FVarStruct(AnimationDimension, AnimationValue));
		
		// iterate over KeyProperties and set the corresponding property in MetaDataRow
		 for (const auto& KeyProperty : KeyProperties)
		 {
		 	FString PropertyName = KeyProperty.Key;
		 	FProperty* Property = KeyProperty.Value;
		 	FString PropertyValue = GetPropertyValueAsString(Property, MetaDataRow);

		 	Variables.Add(FVarStruct(PropertyName, PropertyValue));

		 	UE_LOG(LogTemp, Display, TEXT("Zroobabvel 1 Property: %s: %s"), *PropertyName, *PropertyValue);
		 	FTableRowBase* ManyToOneRow = ManyToOneTableHandler->GetTableRow(Variables);
		 	UE_LOG(LogTemp, Display, TEXT("Zroobabvel 2 "));

		 	//Iterate over UpdateProperties
		 	for(FString PropName: UpdateProperties)
		 	{
		 		UE_LOG(LogTemp, Display, TEXT("Zuchini 0 Property: %s"), *PropName);
		 		
		 		FString PropValue = GetPropertyValueAsString(ManyToOneStruct->FindPropertyByName(*PropName), *ManyToOneRow);

		 		UE_LOG(LogTemp, Display, TEXT("Zuchini 1 Property: %s: %s"), *PropName, *PropValue);
		 	}
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
