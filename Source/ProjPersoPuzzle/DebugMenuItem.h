#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DebugMenuItem.generated.h"

UENUM(BlueprintType)
enum class EMenuItemType : uint8 {
	Action,
	Toggle,
	SubMenu,
	DisplayOnly
};

UCLASS(BlueprintType)
class PROJPERSOPUZZLE_API UDebugMenuItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Debug")
	FString Label;

	UPROPERTY(BlueprintReadOnly, Category = "Debug")
	EMenuItemType Type;

	UPROPERTY(BlueprintReadOnly, Category = "Debug")
	FName ActionTag;

	UPROPERTY(BlueprintReadOnly, Category = "Debug")
	TArray<UDebugMenuItem*> SubItems;

	UPROPERTY(BlueprintReadOnly, Category = "Debug")
	FString DynamicValue;

	UPROPERTY(BlueprintReadOnly, Category = "Debug")
	bool bIsActive = false;


	UPROPERTY(BlueprintReadOnly, Category = "Debug")
	TArray<FString> ActionLabels;

	UPROPERTY(BlueprintReadOnly, Category = "Debug")
	int32 CurrentActionIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Debug")
	bool CanBeActive = true;
	

	UDebugMenuItem* GetItemByName(const FString& Name)
	{
		const int _count = SubItems.Num();
		for (int32 i = 0; i < _count; i++)
		{
			if (SubItems[i]->Label == Name)
				return SubItems[i];
		}
		return nullptr;
	}
};