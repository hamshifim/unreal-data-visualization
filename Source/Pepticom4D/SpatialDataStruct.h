#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SpatialDataStruct.generated.h"


/** Please add a struct description */
USTRUCT(BlueprintType)
struct FSpatialDataStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "X", MakeStructureDefaultValue = "0.000000"))
	double X;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Y", MakeStructureDefaultValue = "0.000000"))
	double Y;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Z", MakeStructureDefaultValue = "0.000000"))
	double Z;
};
