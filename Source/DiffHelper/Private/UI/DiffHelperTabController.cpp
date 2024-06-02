﻿// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/DiffHelperTabController.h"

#include "AssetToolsModule.h"
#include "Misc/ComparisonUtility.h"
#include "DiffHelper.h"
#include "DiffHelperCacheManager.h"
#include "DiffHelperManager.h"
#include "DiffHelperSettings.h"
#include "DiffHelperUtils.h"
#include "DiffUtils.h"
#include "UI/DiffHelperTabModel.h"

void UDiffHelperTabController::Init()
{
	AddToRoot();
	Model = NewObject<UDiffHelperTabModel>(this);
	Model->OnModelUpdated_Raw.AddWeakLambda(this, [this]() { Model->OnModelUpdated.Broadcast(); });

	const auto* Manager = FDiffHelperModule::Get().GetManager();
	Model->Branches = Manager->GetBranches();

	if (UDiffHelperSettings::IsCachingEnabled())
	{
		LoadCachedBranches();
	}
}

void UDiffHelperTabController::LoadCachedBranches()
{
	auto* CacheManager = FDiffHelperModule::Get().GetCacheManager();
	check(CacheManager);
		
	const auto CachedSourceBranchName = CacheManager->GetSourceBranch();
	const auto CachedTargetBranchName = CacheManager->GetTargetBranch();

	for (const auto& Branch : Model->Branches)
	{
		if (Branch.Name == CachedSourceBranchName)
		{
			Model->SourceBranch = Branch;
		}

		if (Branch.Name == CachedTargetBranchName)
		{
			Model->TargetBranch = Branch;
		}
	}
}

void UDiffHelperTabController::Deinit()
{
	RemoveFromRoot();
	Model = nullptr;
}

void UDiffHelperTabController::SetSourceBranch(const FDiffHelperBranch& InBranch)
{
	Model->SourceBranch = InBranch;

	if (UDiffHelperSettings::IsCachingEnabled())
	{
		auto* Manager = FDiffHelperModule::Get().GetCacheManager();
		Manager->SetSourceBranch(InBranch);
	}
}

void UDiffHelperTabController::SetTargetBranch(const FDiffHelperBranch& InBranch)
{
	Model->TargetBranch = InBranch;

	if (UDiffHelperSettings::IsCachingEnabled())
	{
		auto* Manager = FDiffHelperModule::Get().GetCacheManager();
		Manager->SetTargetBranch(InBranch);
	}
}

void UDiffHelperTabController::SelectDiffItem(const FDiffHelperDiffItem& InDiffItem)
{
	Model->SelectedDiffItem = InDiffItem;
}

void UDiffHelperTabController::CollectDiff()
{
	const auto* Manager = FDiffHelperModule::Get().GetManager();
	auto Diff = Manager->GetDiff(Model->SourceBranch, Model->TargetBranch);
	Diff.Sort([](const FDiffHelperDiffItem& A, const FDiffHelperDiffItem& B)
	{
		return UE::ComparisonUtility::CompareNaturalOrder(A.Path, B.Path) < 0;
	});

	Model->Diff = Diff;

	Model->DiffPanelData.OriginalDiff = UDiffHelperUtils::GenerateList(Model->Diff);
	UDiffHelperUtils::SortDiffList(Model->DiffPanelData.SortMode, Model->DiffPanelData.OriginalDiff);
	Model->DiffPanelData.FilteredDiff = Model->DiffPanelData.OriginalDiff;
	
	Model->DiffPanelData.OriginalTreeDiff = UDiffHelperUtils::GenerateTree(Model->Diff);
	UDiffHelperUtils::SortDiffTree(Model->DiffPanelData.SortMode, Model->DiffPanelData.OriginalTreeDiff);
	Model->DiffPanelData.FilteredTreeDiff = Model->DiffPanelData.OriginalTreeDiff;

	Model->DiffPanelData.SearchFilter = MakeShared<TTextFilter<const FDiffHelperDiffItem&>>(TTextFilter<const FDiffHelperDiffItem&>::FItemToStringArray::CreateUObject(this, &UDiffHelperTabController::PopulateFilterSearchString));
	Model->DiffPanelData.SearchFilter->OnChanged().AddUObject(this, &UDiffHelperTabController::OnFilterChanged);
}

void UDiffHelperTabController::DiffAsset(const FString& InPath, const FDiffHelperCommit& InFirstRevision, const FDiffHelperCommit& InSecondRevision) const
{
	TObjectPtr<UObject> LeftDiffAsset = nullptr;
	TObjectPtr<UObject> RightDiffAsset = nullptr;

	FRevisionInfo LeftVersionInfo;
	LeftVersionInfo.Revision = InFirstRevision.Revision;
	LeftVersionInfo.Date = InFirstRevision.Date;
	// TODO: What is Changelist here? For another VCS? Need research.
	// LeftVersionInfo.Changelist = InFirstRevision.Changelist;

	FRevisionInfo RightVersionInfo;
	RightVersionInfo.Revision = InSecondRevision.Revision;
	RightVersionInfo.Date = InSecondRevision.Date;

	const auto* Manager = FDiffHelperModule::Get().GetManager();
	const auto LeftTempFilename = Manager->GetFile(InPath, InFirstRevision.Revision);
	const auto RightTempFilename = Manager->GetFile(InPath, InSecondRevision.Revision);

	if (!LeftTempFilename.IsSet() || !RightTempFilename.IsSet())
	{
		UE_LOG(LogDiffHelper, Error, TEXT("Failed to get diff file for %s"), *InPath);
		return;
	}

	const auto LeftPackagePath = FPackagePath::FromLocalPath(LeftTempFilename.GetValue());
	const auto RightPackagePath = FPackagePath::FromLocalPath(RightTempFilename.GetValue());
	const auto OriginalPackagePath = FPackagePath::FromLocalPath(InPath);
	
	const auto LeftDiffPackage = DiffUtils::LoadPackageForDiff(LeftPackagePath, OriginalPackagePath);
	const auto RightDiffPackage = DiffUtils::LoadPackageForDiff(RightPackagePath, OriginalPackagePath);

	FString AssetName = FPaths::GetBaseFilename(InPath);
	LeftDiffAsset = FindObject<UObject>(LeftDiffPackage, *AssetName);
	RightDiffAsset = FindObject<UObject>(RightDiffPackage, *AssetName);
	
	if (LeftDiffPackage && !LeftDiffAsset)
	{
		LeftDiffAsset = LeftDiffPackage->FindAssetInPackage();
	}

	if (RightDiffPackage && !RightDiffAsset)
	{
		RightDiffAsset = RightDiffPackage->FindAssetInPackage();
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	if (IsValid(LeftDiffAsset) && IsValid(RightDiffAsset))
	{
		AssetToolsModule.Get().DiffAssets(LeftDiffAsset, RightDiffAsset, LeftVersionInfo, RightVersionInfo);
	}
}

void UDiffHelperTabController::CallModelUpdated() const
{
	Model->OnModelUpdated_Raw.Broadcast();
}

void UDiffHelperTabController::SetSearchFilter(const FText& InText) const
{
	Model->DiffPanelData.SearchFilter->SetRawFilterText(InText);
}

void UDiffHelperTabController::SetSortingMode(const FName& InColumnId, EColumnSortMode::Type InSortMode) const
{
	auto& Data = Model->DiffPanelData;
	Data.SortColumn = InColumnId;
	Data.SortMode = InSortMode;

	UDiffHelperUtils::SortDiffList(Data.SortMode, Data.FilteredDiff);
	UDiffHelperUtils::SortDiffTree(Data.SortMode, Data.FilteredTreeDiff);

	CallModelUpdated();
}

void UDiffHelperTabController::SetActiveWidgetIndex(const int32& InIndex) const
{
	Model->DiffPanelData.CurrentWidgetIndex = InIndex;

	CallModelUpdated();
}

FDiffHelperSimpleDelegate& UDiffHelperTabController::OnModelUpdated() const
{
	return Model->OnModelUpdated_Raw;
}

void UDiffHelperTabController::OnFilterChanged()
{
	auto& Data = Model->DiffPanelData;
	Data.FilteredDiff = Data.OriginalDiff;
	Data.FilteredTreeDiff = Data.OriginalTreeDiff;
	
	UDiffHelperUtils::FilterListItems(Data.SearchFilter, Data.FilteredDiff);
	UDiffHelperUtils::FilterTreeItems(Data.SearchFilter, Data.FilteredTreeDiff);

	UDiffHelperUtils::SortDiffList(Data.SortMode, Data.FilteredDiff);
	UDiffHelperUtils::SortDiffTree(Data.SortMode, Data.FilteredTreeDiff);

	// TODO: We need to add more specific events for model update. Calling global update is not good approach.
	CallModelUpdated();
}

void UDiffHelperTabController::PopulateFilterSearchString(const FDiffHelperDiffItem& InItem, TArray<FString>& OutStrings) const
{
	OutStrings.Add(InItem.Path);
}
