// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DiffHelperUtils.generated.h"

enum class EDiffHelperFileStatus : uint8;
struct FDiffHelperDiffItem;
struct FDiffHelperItemNode;

UCLASS()
class DIFFHELPER_API UDiffHelperUtils : public UObject
{
	GENERATED_BODY()

public:
	template<typename T>
	static TArray<TSharedPtr<T>> ConvertToShared(const TArray<T>& InArray)
	{
		TArray<TSharedPtr<T>> OutArray;
		OutArray.Reserve(InArray.Num());
		for (const auto& Item : InArray)
		{
			OutArray.Add(MakeShared<T>(Item));
		}
		
		return OutArray;
	}

	UFUNCTION(BlueprintPure, Category = "DiffHelper|Utils")
	static TArray<FString> ConvertBranchesToStringArray(const TArray<FDiffHelperBranch>& InBranches);

	UFUNCTION(BlueprintPure, Category = "DiffHelper|Utils")
	static bool CompareStatus(const EDiffHelperFileStatus InStatusA, const EDiffHelperFileStatus InStatusB);

	static TArray<TSharedPtr<FDiffHelperItemNode>> GenerateTree(const TArray<FDiffHelperDiffItem>& InItems);
	static TArray<TSharedPtr<FDiffHelperItemNode>> GenerateTree(const TArray<TSharedPtr<FDiffHelperDiffItem>>& InItems);
	static TSharedPtr<FDiffHelperItemNode> PopulateTree(const TSet<FString>& InPaths);
	
	static void SortDiffArray(const FName& InSortColumnId, const EColumnSortMode::Type InSortMode, TArray<TSharedPtr<FDiffHelperDiffItem>>& OutArray);
};
