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
	void InitializeSpatialTable(FString ADataType, FString ATableName, FString ADataSource);
	void InitializeTransientTable(FString ADataType, FString ATableName, FString AKeyRegex, FString ASourcePath);
	FString GetFullTableName();
	FName GetSpecificKey(const TArray<FVarStruct>& Variables);
	void VerbosePrint();
	UDataTable* GetDataTable();
	void AddContentToTable(FString Content, FString FileType);
	void AddDataToDataTableFromSource();
	void AddDataToDataTableFromSource(int ChunkSize);
	TArray<FString> GetChunkedContentFromCSVSourceFile(int ChunkSize);
	FString GetContentFromSourceFile();
	void ClearData();
	FTableRowBase* GetTableRow(const TArray<FVarStruct>& Variables);
	FString GetFileType();
};
