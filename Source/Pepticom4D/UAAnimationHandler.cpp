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


void UAAnimationHandler::AnimateActor(TArray<FVarStruct> Variables)
{
	UADataTypeHandler* DataTypeHandler = DataTypeHandlerMap->FindRef(this->DataType);

	UATableHandler* TableHandler = DataTypeHandler->GetTableHandler(this->ActorTableName);
	UStruct* MetaDataStruct = TableHandler->GetDataTable()->RowStruct;

	//Get the FProperty* corresponding to   "BackboneSize" in MetaDataStruct
	FProperty* Property = MetaDataStruct->FindPropertyByName("BackboneSize");
	
	UATableHandler* ManyToOneTableHandler = DataTypeHandler->GetManyToOneTableHandler(this->OneToManyTableName);

	for(ADataPointActor* DataPointActor: DataTypeHandler->GetDataPointActors())
	{
		FTableRowBase& MetaDataRow = DataPointActor->GetMetadataRow();

		FString PropertyValue = GetPropertyValueAsString(Property, MetaDataRow);

		UE_LOG(LogTemp, Display, TEXT("Knisch BackboneSize: %s"), *PropertyValue);
	}
	

	UE_LOG(LogTemp, Display, TEXT("Zroobabvel"));

	ManyToOneTableHandler->GetTableRow(Variables);

	UE_LOG(LogTemp, Display, TEXT("Zroobabvel 1"));

	//TODO use the data to animate the actors
	const UDataTable* ManyToOneTable = ManyToOneTableHandler->GetDataTable();
	
	TArray<FName> SpatialMetadataRowNames = ManyToOneTable->GetRowNames();
	
	for (const auto& SpatialMetadataRowName : SpatialMetadataRowNames)
	{
		UE_LOG(LogTemp, Display, TEXT("Balbook SpatialMetadataRowName: %s"), *SpatialMetadataRowName.ToString());
	}

	UE_LOG(LogTemp, Display, TEXT("Zroobabvel 3"));
}

void UAAnimationHandler::OnAnimationValueChanged(FString AnimationValue)
{
	//init an array of FVarStruct
	TArray<FVarStruct> Variables;
	Variables.Add(FVarStruct("Index", "69973607186440"));
	Variables.Add(FVarStruct(AnimationDimension, AnimationValue));
	Variables.Add(FVarStruct("BackboneSize", "5"));

	UE_LOG(LogTemp, Display, TEXT("Wowfull AnimationDimension: %s"), *AnimationDimension);
	AnimateActor(Variables);
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
