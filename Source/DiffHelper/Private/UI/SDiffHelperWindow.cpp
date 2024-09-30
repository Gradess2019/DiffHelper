// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperWindow.h"
#include "DiffHelperCommands.h"
#include "UI/SDiffHelperPickerPanel.h"
#include "UI/DiffHelperTabController.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabModel.h"
#include "UI/SDiffHelperDiffViewer.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperWindow::Construct(const FArguments& InArgs)
{
	const auto DiffPicker = CreateDiffPicker();
	SWindow::Construct(
		SWindow::FArguments()
		.Title(LOCTEXT("DiffHelperWindowTitle", "Diff Helper"))
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 2
		.ClientSize(FVector2f(800, 600))
#else
		.ClientSize(FVector2D(800, 600))
#endif
		.MinWidth(300)
		.MinHeight(200)
		[
			DiffPicker
		]
	);
}

TSharedRef<SWidget> SDiffHelperWindow::CreateStartupView()
{
	const auto DiffPicker = CreateDiffPicker();
	return DiffPicker;
}

TSharedRef<SWidget> SDiffHelperWindow::CreateMenu()
{
	// FMenuBarBuilder MenuBarBuilder(Controller->GetMenuCommands());
	// MenuBarBuilder.AddPullDownMenu(
	// 	LOCTEXT("MenuLabel", "Diff"),
	// 	FText::GetEmpty(),
	// 	FNewMenuDelegate::CreateLambda([](FMenuBuilder& Builder)
	// 	{
	// 		Builder.AddMenuEntry(FDiffHelperCommands::Get().CreateNewDiff);
	// 	})
	// );

	// TSharedRef<SWidget> MenuBarWidget = MenuBarBuilder.MakeWidget();
	// return MenuBarWidget;
	return SNullWidget::NullWidget;
}

TSharedRef<SWidget> SDiffHelperWindow::CreateDiffPicker()
{
	return SNew(SDiffHelperPickerPanel);
}

void SDiffHelperWindow::Reset()
{
	const auto& StartupView = CreateStartupView();
	SetContent(StartupView);

	SetTitle(LOCTEXT("DiffHelperWindowTitle", "Diff Helper"));
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
