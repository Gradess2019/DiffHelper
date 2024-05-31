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

TArray<TSharedPtr<FDiffHelperItemNode>> UDiffHelperUtils::ConvertTreeToList(const TArray<TSharedPtr<FDiffHelperItemNode>>& InRoot)
{
	TArray<TSharedPtr<FDiffHelperItemNode>> OutArray;
	for (const auto& Node : InRoot)
	{
		if (Node->DiffItem.IsValid())
		{
			OutArray.Add(Node);
		}
		else
		{
			OutArray.Append(ConvertTreeToList(Node->Children));
		}
	}

	return OutArray;
}

TArray<TSharedPtr<FDiffHelperItemNode>> UDiffHelperUtils::ConvertListToTree(const TArray<TSharedPtr<FDiffHelperItemNode>>& InList)
{
	TArray<TSharedPtr<FDiffHelperDiffItem>> Items;
	
	for (const auto& Node : InList)
	{
		if (ensure(Node->DiffItem.IsValid()))
		{
			Items.Add(Node->DiffItem);
		}
	}

	return GenerateTree(Items);
}

// TArray<TSharedPtr<FDiffHelperItemNode>> UDiffHelperUtils::FilterDiffTree(const TArray<TSharedPtr<FDiffHelperItemNode>>& InTree, const TSharedPtr<IFilter<const FDiffHelperDiffItem&>> InFilter)
// {
// 	
// }

void UDiffHelperUtils::SortDiffArray(const FName& InSortColumnId, const EColumnSortMode::Type InSortMode, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray)
{
	// verify array that it hass valid data
	if (OutArray.Num() == 0)
	{
		return;
	}

	OutArray.RemoveAll([](const TSharedPtr<FDiffHelperItemNode>& InItem)
	{
		return !ensure(InItem.IsValid()) || !ensure(InItem->DiffItem.IsValid());
	});
	
	auto CompareIcons = [](const TSharedPtr<FDiffHelperItemNode>& A, const TSharedPtr<FDiffHelperItemNode>& B)
	{
		return CompareStatus(A->DiffItem->Status, B->DiffItem->Status);
	};

	auto CompareFiles = [](const TSharedPtr<FDiffHelperItemNode>& A, const TSharedPtr<FDiffHelperItemNode>& B)
	{
		return UE::ComparisonUtility::CompareNaturalOrder(A->Path, B->Path) < 0;
	};

	auto GetCompareFunc = [CompareIcons, CompareFiles](const FName& ColumnId)
	{
		if (ColumnId == SDiffHelperDiffPanelConstants::StatusColumnId)
		{
			return TFunction<bool(const TSharedPtr<FDiffHelperItemNode>&, const TSharedPtr<FDiffHelperItemNode>)>(CompareIcons);
		}
		else if (ColumnId == SDiffHelperDiffPanelConstants::PathColumnId)
		{
			return TFunction<bool(const TSharedPtr<FDiffHelperItemNode>&, const TSharedPtr<FDiffHelperItemNode>)>(CompareFiles);
		}

		ensure(false);
		return TFunction<bool(const TSharedPtr<FDiffHelperItemNode>&, const TSharedPtr<FDiffHelperItemNode>)>();
	};

	const auto CompareFunc = GetCompareFunc(InSortColumnId);
	OutArray.Sort([InSortMode, &CompareFunc](const TSharedPtr<FDiffHelperItemNode>& A, const TSharedPtr<FDiffHelperItemNode>& B)
	{
		const auto Result = CompareFunc(A, B);
		return InSortMode == EColumnSortMode::Ascending ? Result : !Result;
	});
}
