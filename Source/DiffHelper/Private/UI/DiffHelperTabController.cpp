// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/DiffHelperTabController.h"

#include "DiffHelper.h"
#include "DiffHelperManager.h"

#include "UI/DiffHelperTabModel.h"

void UDiffHelperTabController::Init()
{
	AddToRoot();
	Model = NewObject<UDiffHelperTabModel>(this);

	const auto* Manager = FDiffHelperModule::Get().GetManager();
	Model->Branches = Manager->GetBranches();
}

void UDiffHelperTabController::Deinit()
{
	RemoveFromRoot();
	Model = nullptr;
}

void UDiffHelperTabController::SetSourceBranch(const FDiffHelperBranch& InBranch)
{
	Model->SourceBranch = InBranch;
}

void UDiffHelperTabController::SetTargetBranch(const FDiffHelperBranch& InBranch)
{
	Model->TargetBranch = InBranch;
}

void UDiffHelperTabController::CollectDiff() const
{
	const auto* Manager = FDiffHelperModule::Get().GetManager();
	Model->Diff = Manager->GetDiff(Model->SourceBranch, Model->TargetBranch);
}

void UDiffHelperTabController::CallModelUpdated() const
{
	Model->OnModelUpdated.Broadcast();
}
