// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperBranchPicker.h"

#include "DiffHelperUtils.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperBranchPicker::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	bSourceBranch = InArgs._bSourceBranch;

	ensure(Controller.IsValid());

	if (!InArgs._Options)
	{
		const auto* Model = Controller->GetModel();
		if (ensure(IsValid(Model)))
		{
			const auto Branches = UDiffHelperUtils::ConvertBranchesToStringArray(Model->Branches);
			Options = MakeShared<const TArray<TSharedPtr<FString>>>(UDiffHelperUtils::ConvertToShared(Branches));
		}
	}
	else
	{
		Options = MakeShared<TArray<TSharedPtr<FString>>>(*InArgs._Options);
	}

	SSearchableComboBox::Construct(
		SSearchableComboBox::FArguments()
		.OptionsSource(Options.Get())
		.OnGenerateWidget(this, &SDiffHelperBranchPicker::HandleGenerateWidget)
		.OnSelectionChanged(this, &SDiffHelperBranchPicker::HandleSelectionChanged)
		.Content()
		[
			// SNew(STextBlock).Text_Lambda([this]()
			// {
				// return FText::FromString(GetSelectedItem().IsValid() ? *GetSelectedItem() : TEXT("Select branch..."));
			// })
			SNew(STextBlock).Text(this, &SDiffHelperBranchPicker::GetSelectedItemText)
		]
	);
}

TSharedRef<SWidget> SDiffHelperBranchPicker::HandleGenerateWidget(TSharedPtr<FString> String)
{
	return SNew(STextBlock).Text(FText::FromString(*String));
}

void SDiffHelperBranchPicker::HandleSelectionChanged(TSharedPtr<FString, ESPMode::ThreadSafe> String, ESelectInfo::Type Arg)
{
	if (!ensure(String.IsValid()) || !ensure(Controller.IsValid())) { return; }

	const auto& Branches = Controller->GetModel()->Branches;
	const auto* FoundBranch = Branches.FindByPredicate([String](const FDiffHelperBranch& Branch)
	{
		return Branch.Name == *String;
	});

	if (!FoundBranch)
	{
		UE_LOG(LogDiffHelper, Error, TEXT("Branch %s not found in the model"), **String);
	}

	if (bSourceBranch)
	{
		Controller->SetSourceBranch(*FoundBranch);
	}
	else
	{
		Controller->SetTargetBranch(*FoundBranch);
	}

	Controller->CallModelUpdated();
}

FText SDiffHelperBranchPicker::GetSelectedItemText() const
{
	if (!ensure(Controller.IsValid())) { return FText::GetEmpty(); }

	const auto* Model = Controller->GetModel();
	
	if (bSourceBranch)
	{
		return FText::FromString(Model->SourceBranch.Name);
	}

	return FText::FromString(Model->TargetBranch.Name);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
