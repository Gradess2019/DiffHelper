// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperBranchPicker.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperBranchPicker::Construct(const FArguments& InArgs)
{
	Options.Add(MakeShareable(new FString(TEXT("Option1"))));
	Options.Add(MakeShareable(new FString(TEXT("CustomOption1"))));
	Options.Add(MakeShareable(new FString(TEXT("Option2"))));
	Options.Add(MakeShareable(new FString(TEXT("abc"))));
	Options.Add(MakeShareable(new FString(TEXT("Option3"))));
	Options.Add(MakeShareable(new FString(TEXT("def"))));
	Options.Add(MakeShareable(new FString(TEXT("CustomOption2"))));
	Options.Add(MakeShareable(new FString(TEXT("VERY LONG OPTION NAME FOR TESTING PURPOSES"))));
	Options.Add(MakeShareable(new FString(TEXT("def"))));

	SSearchableComboBox::Construct(
		SSearchableComboBox::FArguments()
		.OptionsSource(&Options)
		.OnGenerateWidget(this, &SDiffHelperBranchPicker::HandleGenerateWidget)
		.OnSelectionChanged(this, &SDiffHelperBranchPicker::HandleSelectionChanged)
		.MaxListHeight(150.f)
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
