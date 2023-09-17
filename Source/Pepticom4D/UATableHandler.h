#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Actor.h"
#include "Types/SlateEnums.h"
#include "FVarStruct.h"
#include "UATableHandler.generated.h"

UCLASS(BlueprintType)

class UATableHandler : public UObject
{
	GENERATED_BODY()

private:
	FString DataType;
	FString TableName;
	FString KeyRegex;
	FString FullTableName;
	FString SourcePath;
	FString StructName;
	UDataTable* DataTable;
	FString StructNameFromFullTableName();
	void InitTable();

public:
	void Initialize(FString ADataType, FString ATableName, FString AKeyRegex, FString ASourcePath);
	FString GetFullTableName();
	FName GetSpecificKey(const TArray<FVarStruct>& Variables);
	void VerbosePrint();
	UDataTable* GetDataTable();
	void AddDataToDataTableFromSource();
	TArray<FString> GetChunkedContentFromCSVSourceFile(int ChunkSize);
	FString GetContentFromSourceFile(FString SourceFilePath);
	void ClearData();
	FTableRowBase* GetTableRow(const TArray<FVarStruct>& Variables);
};
