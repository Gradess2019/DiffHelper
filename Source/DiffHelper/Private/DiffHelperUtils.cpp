// Copyright 2024 Gradess Games. All Rights Reserved.


#include "DiffHelperUtils.h"
#include "DiffHelperTypes.h"
#include "Misc/ComparisonUtility.h"

TArray<FString> UDiffHelperUtils::ConvertBranchesToStringArray(const TArray<FDiffHelperBranch>& InBranches)
{
	TArray<FString> OutArray;
	OutArray.Reserve(InBranches.Num());
	for (const auto& Branch : InBranches)
	{
		OutArray.Add(Branch.Name);
	}

	return OutArray;
}

bool UDiffHelperUtils::CompareStatus(const EDiffHelperFileStatus InStatusA, const EDiffHelperFileStatus InStatusB)
{
	return static_cast<uint8>(InStatusA) < static_cast<uint8>(InStatusB);
}

TArray<TSharedPtr<FDiffHelperItemNode>> UDiffHelperUtils::GenerateTree(const TArray<FDiffHelperDiffItem>& InItems)
{
	TSet<FString> Paths;
	for (const auto DiffItem : InItems)
	{
		const auto& Path = DiffItem.Path;
		Paths.Add(Path);
	}

	TSharedPtr<FDiffHelperItemNode> Root = PopulateTree(Paths);
	return Root->Children;
}

TArray<TSharedPtr<FDiffHelperItemNode>> UDiffHelperUtils::GenerateTree(const TArray<TSharedPtr<FDiffHelperDiffItem>>& InItems)
{
	TSet<FString> Paths;
	for (const auto DiffItem : InItems)
	{
		const auto& Path = DiffItem->Path;
		Paths.Add(Path);
	}

	TSharedPtr<FDiffHelperItemNode> Root = PopulateTree(Paths);
	return Root->Children;
}

TSharedPtr<FDiffHelperItemNode> UDiffHelperUtils::PopulateTree(const TSet<FString>& InPaths)
{
	auto Root = MakeShared<FDiffHelperItemNode>();
	for (const auto& Path : InPaths)
	{
		TArray<FString> PathComponents;
		Path.ParseIntoArray(PathComponents, TEXT("/"), true);

		TSharedPtr<FDiffHelperItemNode> Current = Root;
		FString CurrentPath = "";

		for (const auto& PathComponent : PathComponents)
		{
			CurrentPath = CurrentPath + "/" + PathComponent;

			TSharedPtr<FDiffHelperItemNode> Child;
			for (const auto& CurrentChil : Current->Children)
			{
				if (CurrentChil->Path == CurrentPath)
				{
					Child = CurrentChil;
					break;
				}
			}

			if (!Child.IsValid())
			{
				Child = MakeShared<FDiffHelperItemNode>(CurrentPath);
				Current->Children.Add(Child);
			}

			Current = Child;
		}
	}

	return Root;
}

void UDiffHelperUtils::SortDiffArray(const FName& InSortColumnId, const EColumnSortMode::Type InSortMode, TArray<TSharedPtr<FDiffHelperDiffItem>>& OutArray)
{
	auto CompareIcons = [](const TSharedPtr<FDiffHelperDiffItem>& A, const TSharedPtr<FDiffHelperDiffItem>& B)
	{
		return CompareStatus(A->Status, B->Status);
	};

	auto CompareFiles = [](const TSharedPtr<FDiffHelperDiffItem>& A, const TSharedPtr<FDiffHelperDiffItem>& B)
	{
		return UE::ComparisonUtility::CompareNaturalOrder(A->Path, B->Path) < 0;
	};

	auto GetCompareFunc = [CompareIcons, CompareFiles](const FName& ColumnId)
	{
		if (ColumnId == SDiffHelperDiffPanelConstants::StatusColumnId)
		{
			return TFunction<bool(const TSharedPtr<FDiffHelperDiffItem>&, const TSharedPtr<FDiffHelperDiffItem>)>(CompareIcons);
		}
		else if (ColumnId == SDiffHelperDiffPanelConstants::PathColumnId)
		{
			return TFunction<bool(const TSharedPtr<FDiffHelperDiffItem>&, const TSharedPtr<FDiffHelperDiffItem>)>(CompareFiles);
		}

		ensure(false);
		return TFunction<bool(const TSharedPtr<FDiffHelperDiffItem>&, const TSharedPtr<FDiffHelperDiffItem>)>();
	};

	const auto CompareFunc = GetCompareFunc(InSortColumnId);
	OutArray.Sort([InSortMode, &CompareFunc](const TSharedPtr<FDiffHelperDiffItem>& A, const TSharedPtr<FDiffHelperDiffItem>& B)
	{
		const auto Result = CompareFunc(A, B);
		return InSortMode == EColumnSortMode::Ascending ? Result : !Result;
	});
}
