// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.h"

#include "UObject/Object.h"
#include "Misc/IFilter.h"
#include "DiffHelperUtils.generated.h"

struct FDiffHelperBranch;
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

	template<typename T>
	static FString EnumToString(const T EnumValue)
	{
		FString Name = StaticEnum<T>()->GetNameStringByValue(static_cast<__underlying_type(T)>(EnumValue));

		ensure(Name.Len() != 0);

		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "DiffHelper|Utils")
	static TArray<FString> ConvertBranchesToStringArray(const TArray<FDiffHelperBranch>& InBranches);

	UFUNCTION(BlueprintPure, Category = "DiffHelper|Utils")
	static bool CompareStatus(const EDiffHelperFileStatus InStatusA, const EDiffHelperFileStatus InStatusB);

public:
	static bool IsDiffAvailable(const TSharedPtr<FDiffHelperCommit>& InCommit, const FString& InPath);
	static bool IsDiffAvailable(const TArray<TSharedPtr<FDiffHelperCommit>>& InCommits, const FString& InPath);

	static int32 GetItemNodeFilesCount(const TSharedPtr<FDiffHelperItemNode>& InItem);
	
	static TArray<TSharedPtr<FDiffHelperItemNode>> GenerateList(const TArray<FDiffHelperDiffItem>& InItems);
	
	static TArray<TSharedPtr<FDiffHelperItemNode>> GenerateTree(const TArray<FDiffHelperDiffItem>& InItems);
	static TArray<TSharedPtr<FDiffHelperItemNode>> GenerateTree(const TArray<TSharedPtr<FDiffHelperDiffItem>>& InItems);
	static TSharedPtr<FDiffHelperItemNode> PopulateTree(const TArray<TSharedPtr<FDiffHelperDiffItem>>& InItems);

	static TArray<TSharedPtr<FDiffHelperItemNode>> ConvertTreeToList(const TArray<TSharedPtr<FDiffHelperItemNode>>& InRoot);
	static TArray<TSharedPtr<FDiffHelperItemNode>> ConvertListToTree(const TArray<TSharedPtr<FDiffHelperItemNode>>& InList);

	static TMap<FString, TSharedPtr<FDiffHelperItemNode>> GetDirectories(const TArray<TSharedPtr<FDiffHelperItemNode>>& InItems);
	static TMap<FString, TSharedPtr<FDiffHelperItemNode>> GetDirectories(const TArrayView<const TSharedPtr<FDiffHelperItemNode>>& InItems);

	static void SortDiffList(const EColumnSortMode::Type InSortMode, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray);
	static void SortDiffTree(const EColumnSortMode::Type InSortMode, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray);
	static void Sort(EColumnSortMode::Type InSortMode, const TFunction<bool(const TSharedPtr<FDiffHelperItemNode>& A, const TSharedPtr<FDiffHelperItemNode>& B)>& InFileComparator, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray);

	static void FilterListItems(const TSharedPtr<IFilter<const FDiffHelperDiffItem&>>& InFilter, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray);
	static void FilterTreeItems(const TSharedPtr<IFilter<const FDiffHelperDiffItem&>>& InFilter, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray);
	static void Filter(TSharedPtr<IFilter<const FDiffHelperDiffItem&>> InFilter, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray);

	static void CopyExpandedState(const TArray<TSharedPtr<FDiffHelperItemNode>>& InSource, TArray<TSharedPtr<FDiffHelperItemNode>>& InTarget);

	static void ShowDiffUnavailableDialog(const TArray<TSharedPtr<FDiffHelperCommit>>& InCommits, const FString& InPath);
};
