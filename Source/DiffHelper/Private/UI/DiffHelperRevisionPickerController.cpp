// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/DiffHelperRevisionPickerController.h"

#include "DiffHelper.h"
#include "DiffHelperCacheManager.h"
#include "DiffHelperManager.h"
#include "DiffHelperSettings.h"

#include "UI/DiffHelperRevisionPickerModel.h"

void UDiffHelperRevisionPickerController::Init()
{
	AddToRoot();
	InitModel();
}

void UDiffHelperRevisionPickerController::Deinit()
{
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
