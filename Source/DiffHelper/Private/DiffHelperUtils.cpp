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
	TSharedPtr<FDiffHelperItemNode> Root = PopulateTree(ConvertToShared(InItems));
	return Root->Children;
}

TArray<TSharedPtr<FDiffHelperItemNode>> UDiffHelperUtils::GenerateTree(const TArray<TSharedPtr<FDiffHelperDiffItem>>& InItems)
{
	TSharedPtr<FDiffHelperItemNode> Root = PopulateTree(InItems);
	return Root->Children;
}

TSharedPtr<FDiffHelperItemNode> UDiffHelperUtils::PopulateTree(const TArray<TSharedPtr<FDiffHelperDiffItem>>& InItems)
{
	auto Root = MakeShared<FDiffHelperItemNode>();
	for (const auto& Item : InItems)
	{
		TArray<FString> PathComponents;
		Item->Path.ParseIntoArray(PathComponents, TEXT("/"), true);

		TSharedPtr<FDiffHelperItemNode> CurrentNode = Root;
		FString CurrentPath = "";

		for (const auto& PathComponent : PathComponents)
		{
			CurrentPath = CurrentPath + "/" + PathComponent;

			TSharedPtr<FDiffHelperItemNode> NodeChild;
			for (const auto& ExistingNodeChild : CurrentNode->Children)
			{
				if (ExistingNodeChild->Path == CurrentPath)
				{
					NodeChild = ExistingNodeChild;
					break;
				}
			}

			if (!NodeChild.IsValid())
			{
				NodeChild = MakeShared<FDiffHelperItemNode>(CurrentPath);
				NodeChild->Name = PathComponent;
				CurrentNode->Children.Add(NodeChild);
			}

			CurrentNode = NodeChild;
		}

		CurrentNode->DiffItem = Item;
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
