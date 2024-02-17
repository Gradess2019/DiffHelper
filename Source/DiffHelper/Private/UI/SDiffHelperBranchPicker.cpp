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
			SNew(STextBlock).Text_Lambda([this]()
			{
				return FText::FromString(GetSelectedItem().IsValid() ? *GetSelectedItem() : TEXT("Select branch..."));
			})
		]
	);
}

TSharedRef<SWidget> SDiffHelperBranchPicker::HandleGenerateWidget(TSharedPtr<FString> String)
{
	return SNew(STextBlock).Text(FText::FromString(*String));
}

void SDiffHelperBranchPicker::HandleSelectionChanged(TSharedPtr<FString, ESPMode::ThreadSafe> String, ESelectInfo::Type Arg)
{
	
	UE_LOG(LogTemp, Warning, TEXT("Selected: %s, Arg: %d"), **String, (int)Arg);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
