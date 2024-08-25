// Copyright 2024 Gradess Games. All Rights Reserved.


#include "DiffHelperUtils.h"
#include "DiffHelperSettings.h"
#include "DiffHelperTypes.h"

#include "Framework/Notifications/NotificationManager.h"

#include "Misc/ComparisonUtility.h"

#include "Widgets/Notifications/SNotificationList.h"

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

bool UDiffHelperUtils::IsUnrealAsset(const FString& InPackageName)
{
	const auto PackageExtension = FPackagePath::ParseExtension(InPackageName);
	return PackageExtension != EPackageExtension::Custom && PackageExtension != EPackageExtension::Unspecified;
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
			CurrentPath = FPaths::Combine(CurrentPath, PathComponent);

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
				NodeChild = MakeShared<FDiffHelperItemNode>();
				NodeChild->Path = CurrentPath;
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

TMap<FString, TSharedPtr<FDiffHelperItemNode>> UDiffHelperUtils::GetDirectories(const TArray<TSharedPtr<FDiffHelperItemNode>>& InItems)
{
	TMap<FString, TSharedPtr<FDiffHelperItemNode>> OutMap;
	for (const auto& Node : InItems)
	{
		if (!Node->DiffItem.IsValid() && !Node->Path.IsEmpty())
		{
			OutMap.Add(Node->Path, Node);
		}

		OutMap.Append(GetDirectories(Node->Children));
	}

	return OutMap;
}

TMap<FString, TSharedPtr<FDiffHelperItemNode>> UDiffHelperUtils::GetDirectories(const TArrayView<const TSharedPtr<FDiffHelperItemNode>>& InItems)
{
	TMap<FString, TSharedPtr<FDiffHelperItemNode>> OutMap;
	for (const auto& Node : InItems)
	{
		if (!Node->DiffItem.IsValid() && !Node->Path.IsEmpty())
		{
			OutMap.Add(Node->Path, Node);
		}

		OutMap.Append(GetDirectories(Node->Children));
	}

	return OutMap;
}

TSharedPtr<FDiffHelperItemNode> UDiffHelperUtils::FindItemInTree(const TArray<TSharedPtr<FDiffHelperItemNode>>& InItems, const TSharedPtr<FDiffHelperItemNode>& InItem)
{
	if (!InItem.IsValid())
	{
		return nullptr;
	}
	
	return FindItemInTree(InItems, InItem->Path);
}

TSharedPtr<FDiffHelperItemNode> UDiffHelperUtils::FindItemInTree(const TArray<TSharedPtr<FDiffHelperItemNode>>& InItems, const FString& InPath)
{
	if (InPath.IsEmpty())
	{
		return nullptr;
	}

	TQueue<TSharedPtr<FDiffHelperItemNode>> Queue;

	auto EnqueueArray = [&Queue](const TArray<TSharedPtr<FDiffHelperItemNode>>& InArray)
	{
		for (const auto& Node : InArray)
		{
			Queue.Enqueue(Node);
		}
	};
	EnqueueArray(InItems);
	
	while (!Queue.IsEmpty())
	{
		TSharedPtr<FDiffHelperItemNode> Node;
		Queue.Dequeue(Node);
		if (Node->Path.Equals(InPath))
		{
			return Node;
		}

		EnqueueArray(Node->Children);
	}

	return nullptr;
}

void UDiffHelperUtils::CopyExpandedState(const TArray<TSharedPtr<FDiffHelperItemNode>>& InSource, TArray<TSharedPtr<FDiffHelperItemNode>>& InTarget)
{
	for (const auto& TargetNode : InTarget)
	{
		if (TargetNode->DiffItem.IsValid())
		{
			continue;
		}

		for (const auto& SourceNode : InSource)
		{
			if (SourceNode->Path == TargetNode->Path)
			{
				TargetNode->bExpanded = SourceNode->bExpanded;
				CopyExpandedState(SourceNode->Children, TargetNode->Children);
				break;
			}
		}
	}
}

void UDiffHelperUtils::SortDiffList(const EColumnSortMode::Type InSortMode, TArray<TSharedPtr<FDiffHelperItemNode>>& OutArray)
{
	auto SorterByName = [](const TSharedPtr<FDiffHelperItemNode>& A, const TSharedPtr<FDiffHelperItemNode>& B)
	{
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3
		return UE::ComparisonUtility::CompareNaturalOrder(A->Name, B->Name) < 0;
#else
		return A->Name.Compare(B->Name) < 0;
#endif
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
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3
		return UE::ComparisonUtility::CompareNaturalOrder(A->Path, B->Path) < 0;
#else
		return A->Path.Compare(B->Path) < 0;
#endif
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
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3
				EAppMsgCategory::Error,
#endif
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

TSharedPtr<SNotificationItem> UDiffHelperUtils::AddErrorNotification(const FText& InText)
{
	auto Info = GetBaseErrorNotificationInfo();
	Info.Text = InText;
	
	const auto NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
	if (NotificationItem.IsValid())
	{
		NotificationItem->SetCompletionState(SNotificationItem::CS_Fail);
	}

	UE_LOG(LogDiffHelper, Error, TEXT("%s"), *InText.ToString());

	return NotificationItem;
}

TSharedPtr<SNotificationItem> UDiffHelperUtils::AddErrorNotificationWithLink(const FText& InText, const FText& InHyperLinkText, const FSimpleDelegate& InHyperLink)
{
	auto Info = GetBaseErrorNotificationInfo();
	Info.Text = InText;
	Info.HyperlinkText = InHyperLinkText;
	Info.Hyperlink = InHyperLink;

	const auto NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
	if (NotificationItem.IsValid())
	{
		NotificationItem->SetCompletionState(SNotificationItem::CS_Fail);
	}

	UE_LOG(LogDiffHelper, Error, TEXT("%s"), *InText.ToString());

	return NotificationItem;
}

FNotificationInfo UDiffHelperUtils::GetBaseErrorNotificationInfo()
{
	const auto* Settings = GetDefault<UDiffHelperSettings>();
	
	FNotificationInfo Info(FText::GetEmpty());
	Info.ExpireDuration = Settings->ErrorExpireDuration;
	Info.bUseSuccessFailIcons = Settings->bUseSuccessFailIcons;

	return Info;
}

void UDiffHelperUtils::SetExpansionState(TArray<TSharedPtr<FDiffHelperItemNode>>& InArray, const bool bInExpanded)
{
	for (const auto& Node : InArray)
	{
		Node->bExpanded = bInExpanded;
		SetExpansionState(Node->Children, bInExpanded);
	}
}

void UDiffHelperUtils::ExpandAll(TArray<TSharedPtr<FDiffHelperItemNode>>& InArray)
{
	SetExpansionState(InArray, true);
}

void UDiffHelperUtils::CollapseAll(TArray<TSharedPtr<FDiffHelperItemNode>>& InArray)
{
	SetExpansionState(InArray, false);
}
