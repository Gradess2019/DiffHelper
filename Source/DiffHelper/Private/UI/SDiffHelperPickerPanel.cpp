// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperPickerPanel.h"
#include "UI/SDiffHelperBranchPicker.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperRevisionPickerController.h"
#include "UI/DiffHelperRevisionPickerModel.h"
#include "UI/SDiffHelperDiffViewer.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SDiffHelperPickerPanel::~SDiffHelperPickerPanel()
{
	if (Controller.IsValid())
	{
		Controller->Deinit();
		Controller.Reset();
	}

	FGlobalTabmanager::Get()->UnregisterTabSpawner(DiffHelperConstants::DiffHelperDiffViewerId);
}

void SDiffHelperPickerPanel::Construct(const FArguments& InArgs)
{
	Controller = NewObject<UDiffHelperRevisionPickerController>();
	Controller->Init();

	FGlobalTabmanager::Get()->RegisterTabSpawner(
		DiffHelperConstants::DiffHelperDiffViewerId,
		FOnSpawnTab::CreateRaw(this, &SDiffHelperPickerPanel::SpawnTab),
		FCanSpawnTab::CreateRaw(this, &SDiffHelperPickerPanel::CanSpawnTab)
	);

	OnShowDiff = InArgs._OnShowDiff;

	TSharedPtr<FDiffHelperBranch> CachedSourceBranch = MakeShareable<FDiffHelperBranch>(new FDiffHelperBranch(Controller->GetModel()->SourceBranch));
	TSharedPtr<FDiffHelperBranch> CachedTargetBranch = MakeShareable<FDiffHelperBranch>(new FDiffHelperBranch(Controller->GetModel()->TargetBranch));
	
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		  .HAlign(HAlign_Center)
		  .VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(350.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				[
					SAssignNew(SourceBranchPicker, SDiffHelperBranchPicker)
					.Controller(Controller)
					.InitiallySelectedBranch(CachedSourceBranch)
					.Hint(LOCTEXT("DiffHelperSourceBranchHint", "Select source branch..."))
				]
				+ SVerticalBox::Slot()
				[
					SAssignNew(TargetBranchPicker, SDiffHelperBranchPicker)
					.Controller(Controller)
					.InitiallySelectedBranch(CachedTargetBranch)
					.Hint(LOCTEXT("DiffHelperTargetBranchHint", "Select target branch..."))
				]
				// add "Show diff" button here
				+ SVerticalBox::Slot()
				[
					SNew(SButton)
					.OnClicked(this, &SDiffHelperPickerPanel::OnShowDiffClicked)
					.IsEnabled(this, &SDiffHelperPickerPanel::CanShowDiff)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("DiffHelperShowDiffButton", "Show diff"))
						.Justification(ETextJustify::Center)
					]
				]
			]
		]
	];
}

FReply SDiffHelperPickerPanel::OnShowDiffClicked() const
{
	const auto& SourceBranch = SourceBranchPicker->GetSelectedBranch();
	const auto& TargetBranch = TargetBranchPicker->GetSelectedBranch();

	Controller->SetSourceBranch(SourceBranch);
	Controller->SetTargetBranch(TargetBranch);

	FGlobalTabmanager::Get()->TryInvokeTab(DiffHelperConstants::DiffHelperDiffViewerId);

	OnShowDiff.ExecuteIfBound();
	
	return FReply::Handled();
}

bool SDiffHelperPickerPanel::CanShowDiff() const
{
	const auto& SourceBranch = SourceBranchPicker->GetSelectedBranch();
	const auto& TargetBranch = TargetBranchPicker->GetSelectedBranch();

	return SourceBranch.IsValid() && TargetBranch.IsValid() && SourceBranch != TargetBranch;
}

TSharedRef<SDockTab> SDiffHelperPickerPanel::SpawnTab(const FSpawnTabArgs& InSpawnTabArgs)
{
	const auto& SourceBranch = Controller->GetModel()->SourceBranch;
	const auto& TargetBranch = Controller->GetModel()->TargetBranch;
	const auto NewTitle = SourceBranch.Name + " -> " + TargetBranch.Name;
	
	const auto Tab = SNew(SDockTab)
		.Label(FText::FromString(TEXT("Diff Helper Tab")))
		[
			SNew(SDiffHelperDiffViewer)
				.SourceBranch(&SourceBranch)
				.TargetBranch(&TargetBranch)
		];
	
	return Tab;
}


bool SDiffHelperPickerPanel::CanSpawnTab(const FSpawnTabArgs& InSpawnTabArgs) const
{
	return Controller.IsValid() && Controller->GetModel()->SourceBranch.IsValid() && Controller->GetModel()->TargetBranch.IsValid();
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
