// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/DiffHelperRevisionPickerController.h"

#include "DiffHelper.h"
#include "DiffHelperCacheManager.h"
#include "DiffHelperManager.h"
#include "DiffHelperSettings.h"

#include "UI/DiffHelperRevisionPickerModel.h"
#include "UI/SDiffHelperDiffViewer.h"

TMap<FDiffHelperDiffTabData, TWeakPtr<SDockTab>> UDiffHelperRevisionPickerModel::OpenedTabs = {};

TSharedPtr<SDockTab> FDiffHelperTabSerchPreference::Search(const FTabManager& Manager, FName PlaceholderId, const TSharedRef<SDockTab>& UnmanagedTab) const
{
	return Manager.FindExistingLiveTab(FTabId(DiffHelperConstants::DiffHelperRevisionPickerId));
}

void UDiffHelperRevisionPickerController::Init()
{
	AddToRoot();
	InitModel();
	
	auto& TabSpawnerEntry = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		DiffHelperConstants::DiffHelperDiffViewerId,
		FOnSpawnTab::CreateUObject(this, &UDiffHelperRevisionPickerController::SpawnTab),
		FCanSpawnTab::CreateUObject(this, &UDiffHelperRevisionPickerController::CanSpawnTab)
	);

	const auto* Settings = GetDefault<UDiffHelperSettings>();
	if (Settings->bReuseDiffTab)
	{
		TabSpawnerEntry.SetReuseTabMethod(FOnFindTabToReuse::CreateUObject(this, &UDiffHelperRevisionPickerController::FindTabToReuse));
	}
}

void UDiffHelperRevisionPickerController::Deinit()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(DiffHelperConstants::DiffHelperDiffViewerId);

	RemoveFromRoot();
	Model = nullptr;
}

void UDiffHelperRevisionPickerController::SetSourceBranch(const FDiffHelperBranch& InBranch)
{
	Model->SourceBranch = InBranch;

	if (UDiffHelperSettings::IsCachingEnabled())
	{
		auto* Manager = FDiffHelperModule::Get().GetCacheManager();
		Manager->SetSourceBranch(InBranch);
	}
}

void UDiffHelperRevisionPickerController::SetTargetBranch(const FDiffHelperBranch& InBranch)
{
	Model->TargetBranch = InBranch;

	if (UDiffHelperSettings::IsCachingEnabled())
	{
		auto* Manager = FDiffHelperModule::Get().GetCacheManager();
		Manager->SetTargetBranch(InBranch);
	}
}

void UDiffHelperRevisionPickerController::OpenDiffTab()
{
	FDiffHelperDiffTabData TabData;
	TabData.SourceBranchName = Model->SourceBranch.Name;
	TabData.TargetBranchName = Model->TargetBranch.Name;

	if (Model->OpenedTabs.Contains(TabData))
	{
		auto TabWidgetPtr = Model->OpenedTabs[TabData];
		if (TabWidgetPtr.IsValid())
		{
			FGlobalTabmanager::Get()->DrawAttention(TabWidgetPtr.Pin().ToSharedRef());
		}
	}
	else
	{
		auto NewTab = SpawnTab();
		FGlobalTabmanager::Get()->InsertNewDocumentTab(DiffHelperConstants::DiffHelperDiffViewerId, FDiffHelperTabSerchPreference(), NewTab);
		
		Model->OpenedTabs.Add(TabData, NewTab.ToWeakPtr());
	}

}

void UDiffHelperRevisionPickerController::InitModel()
{
	Model = NewObject<UDiffHelperRevisionPickerModel>(this);

	const auto Manager = FDiffHelperModule::Get().GetManager();
	Model->Branches = Manager->GetBranches();

	if (UDiffHelperSettings::IsCachingEnabled())
	{
		LoadCachedBranches();
	}
}

void UDiffHelperRevisionPickerController::LoadCachedBranches()
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

TSharedRef<SDockTab> UDiffHelperRevisionPickerController::SpawnTab(const FSpawnTabArgs& InSpawnTabArgs)
{
	const auto& SourceBranch = Model->SourceBranch;
	const auto& TargetBranch = Model->TargetBranch;
	const auto NewTitle = SourceBranch.Name + " -> " + TargetBranch.Name;

	const auto Tab = SNew(SDockTab)
		.TabRole(ETabRole::MajorTab)
		.Label(FText::FromString(NewTitle))
		[
			SNew(SDiffHelperDiffViewer)
			.SourceBranch(&SourceBranch)
			.TargetBranch(&TargetBranch)
		];

	return Tab;
}

TSharedRef<SDockTab> UDiffHelperRevisionPickerController::SpawnTab()
{
	const auto& SourceBranch = Model->SourceBranch;
	const auto& TargetBranch = Model->TargetBranch;
	const auto NewTitle = SourceBranch.Name + " -> " + TargetBranch.Name;

	const auto Tab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(FText::FromString(NewTitle))
		[
			SNew(SDiffHelperDiffViewer)
			.SourceBranch(&SourceBranch)
			.TargetBranch(&TargetBranch)
		];

	return Tab;
}

bool UDiffHelperRevisionPickerController::CanSpawnTab(const FSpawnTabArgs& InSpawnTabArgs) const
{
	return Model->SourceBranch.IsValid() && Model->TargetBranch.IsValid();
}

TSharedPtr<SDockTab> UDiffHelperRevisionPickerController::FindTabToReuse(const FTabId& InTabId)
{
	if (InTabId != DiffHelperConstants::DiffHelperDiffViewerId)
	{
		return nullptr;
	}
	
	FDiffHelperDiffTabData TabData;
	TabData.SourceBranchName = Model->SourceBranch.Name;
	TabData.TargetBranchName = Model->TargetBranch.Name;

	if (auto* TabWidgetPtr = Model->OpenedTabs.Find(TabData))
	{
		return TabWidgetPtr->IsValid() ? TabWidgetPtr->Pin() : TSharedPtr<SDockTab>();
	}

	return TSharedPtr<SDockTab>();
}
