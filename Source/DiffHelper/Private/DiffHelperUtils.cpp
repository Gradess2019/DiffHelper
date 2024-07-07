// Copyright 2024 Gradess Games. All Rights Reserved.


#include "DiffHelperUtils.h"
#include "DiffHelperSettings.h"
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

bool UDiffHelperUtils::IsDiffAvailable(const TSharedPtr<FDiffHelperCommit>& InCommit, const FString& InPath)
{
	if (!InCommit.IsValid())
	{
		return false;
	}

	const auto* Settings = GetDefault<UDiffHelperSettings>();
	for (const auto& Item : InCommit->Files)
	{
		if (Item.Path == InPath && !Settings->StatusBlacklist.Contains(Item.Status))
		{
			return true;
		}
	}

	return false;
}

bool UDiffHelperUtils::IsDiffAvailable(const TArray<TSharedPtr<FDiffHelperCommit>>& InCommits, const FString& InPath)
{
	for (const auto& Commit : InCommits)
	{
		if (!IsDiffAvailable(Commit, InPath))
		{
			return false;
		}
	}

	return true;
}

int32 UDiffHelperUtils::GetItemNodeFilesCount(const TSharedPtr<FDiffHelperItemNode>& InItem)
{
	const auto& Children = InItem->Children;
	int32 Count = InItem->DiffItem.IsValid() ? 1 : 0;

	for (const auto& Child : Children)
	{
		Count += GetItemNodeFilesCount(Child);
	}

	return Count;
}

TArray<TSharedPtr<FDiffHelperItemNode>> UDiffHelperUtils::GenerateList(const TArray<FDiffHelperDiffItem>& InItems)
{
	TArray<TSharedPtr<FDiffHelperItemNode>> OutArray;

	for (const auto& Item : InItems)
	{
		auto Node = MakeShared<FDiffHelperItemNode>();
		Node->DiffItem = MakeShared<FDiffHelperDiffItem>(Item);
		Node->Path = Item.Path;
		Node->Name = FPaths::GetCleanFilename(Item.Path);

		OutArray.Add(Node);
	}

	return OutArray;
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

void UDiffHelperUtils::SortDiffList(const EColumnSortMode::Type InSortMode, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray)
{
	auto SorterByName = [](const TSharedPtr<FDiffHelperItemNode>& A, const TSharedPtr<FDiffHelperItemNode>& B)
	{
		return UE::ComparisonUtility::CompareNaturalOrder(A->Name, B->Name) < 0;
	};

	// verify array that it has valid data
	OutArray.RemoveAll([](const TSharedPtr<FDiffHelperItemNode>& InItem)
	{
		return !ensure(InItem.IsValid()) || !ensure(InItem->DiffItem.IsValid());
	});

	Sort(InSortMode, SorterByName, OutArray);
}

void UDiffHelperUtils::SortDiffTree(const EColumnSortMode::Type InSortMode, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray)
{
	auto SorterByPath = [](const TSharedPtr<FDiffHelperItemNode>& A, const TSharedPtr<FDiffHelperItemNode>& B)
	{
		return UE::ComparisonUtility::CompareNaturalOrder(A->Path, B->Path) < 0;
	};

	Sort(InSortMode, SorterByPath, OutArray);
	for (const auto& Node : OutArray)
	{
		SortDiffTree(InSortMode, Node->Children);
	}
}

void UDiffHelperUtils::Sort(EColumnSortMode::Type InSortMode, const TFunction<bool(const TSharedPtr<FDiffHelperItemNode>& A, const TSharedPtr<FDiffHelperItemNode>& B)>& InFileComparator, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray)
{
	if (OutArray.Num() == 0)
	{
		return;
	}

	OutArray.Sort([InSortMode, InFileComparator](const TSharedPtr<FDiffHelperItemNode>& A, const TSharedPtr<FDiffHelperItemNode>& B)
	{
		const auto Result = InFileComparator(A, B);
		return InSortMode == EColumnSortMode::Ascending ? Result : !Result;
	});
}

void UDiffHelperUtils::FilterListItems(const TSharedPtr<IFilter<const FDiffHelperDiffItem&>>& InFilter, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray)
{
	Filter(InFilter, OutArray);
}

void UDiffHelperUtils::FilterTreeItems(const TSharedPtr<IFilter<const FDiffHelperDiffItem&>>& InFilter, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray)
{
	Filter(InFilter, OutArray);
	for (const auto& Node : OutArray)
	{
		FilterTreeItems(InFilter, Node->Children);
	}

	OutArray.RemoveAll([](const TSharedPtr<FDiffHelperItemNode>& InItem)
	{
		return InItem->Children.Num() == 0 && !InItem->DiffItem.IsValid();
	});
}

void UDiffHelperUtils::Filter(TSharedPtr<IFilter<const FDiffHelperDiffItem&>> InFilter, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray)
{
	OutArray.RemoveAll([InFilter](const TSharedPtr<FDiffHelperItemNode>& InItem)
	{
		if (InItem->DiffItem.IsValid())
		{
			return !InFilter->PassesFilter(*InItem->DiffItem);
		}

		return false;
	});
}

void UDiffHelperUtils::ShowDiffUnavailableDialog(const TArray<TSharedPtr<FDiffHelperCommit>>& InCommits, const FString& InPath)
{
	for (const auto& Commit : InCommits)
	{
		if (!IsDiffAvailable(Commit, InPath))
		{
			FMessageDialog::Open(
				EAppMsgCategory::Error,
				EAppMsgType::Ok,
				FText::Format(
					NSLOCTEXT("DiffHelper", "DiffUnavailable", "Diff is not available for path: {0}\nCommit: {1}\nFile Status: {2}"),
					FText::FromString(InPath),
					FText::FromString(Commit->Revision),
					FText::FromString(EnumToString(Commit->Files[0].Status))
				)
			);
		}
	}
}
