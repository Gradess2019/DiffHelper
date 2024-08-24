// Copyright Epic Games, Inc. All Rights Reserved.

#include "DiffHelper.h"
#include "DiffHelperCacheManager.h"
#include "DiffHelperStyle.h"
#include "DiffHelperCommands.h"
#include "DiffHelperGitManager.h"
#include "DiffHelperSettings.h"
#include "DiffHelperTypes.h"
#include "DiffHelperUtils.h"
#include "ILiveCodingModule.h"
#include "ISourceControlModule.h"
#include "ToolMenus.h"

#include "UI/FDiffHelperCommitPanelToolbar.h"
#include "UI/FDiffHelperDiffPanelToolbar.h"
#include "UI/SDiffHelperWindow.h"

#include "Widgets/Testing/SStarshipSuite.h"

static const FName DiffHelperTabName("DiffHelper");

#define LOCTEXT_NAMESPACE "FDiffHelperModule"

void FDiffHelperModule::StartupModule()
{
	FDiffHelperStyle::Initialize();
	FDiffHelperStyle::ReloadTextures();

	DiffHelperManager = TWeakInterfacePtr<IDiffHelperManager>(NewObject<UDiffHelperGitManager>());
	DiffHelperManager->Init();

	CacheManager = TStrongObjectPtr(NewObject<UDiffHelperCacheManager>());
	CacheManager->Init();
	
	FDiffHelperCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FDiffHelperCommands::Get().OpenDiffWindow,
		FExecuteAction::CreateRaw(this, &FDiffHelperModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDiffHelperModule::RegisterMenus));

	// Reload slate style
	if (FModuleManager::Get().IsModuleLoaded("LiveCoding"))
	{
		auto& LiveCodingModule = FModuleManager::GetModuleChecked<ILiveCodingModule>("LiveCoding");
		LiveCodingModule.GetOnPatchCompleteDelegate().AddLambda([]
		{
			FDiffHelperStyle::ReloadStyles();
			FDiffHelperStyle::ReloadTextures();
		});
	}
}

void FDiffHelperModule::ShutdownModule()
{
	if (DiffHelperManager.IsValid())
	{
		DiffHelperManager->Deinit();
	}

	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FDiffHelperStyle::Shutdown();
	FDiffHelperCommands::Unregister();
}

void FDiffHelperModule::PluginButtonClicked()
{
	if (!ISourceControlModule::Get().IsEnabled())
	{
		UDiffHelperUtils::AddErrorNotificationWithLink(
			LOCTEXT("RevisionControlDisabled", "Revision control is disabled."),
			LOCTEXT("RevisionControlDisabled_HyperLink", "Open Unreal Documentation"),
			FSimpleDelegate::CreateLambda([]() { FPlatformProcess::LaunchURL(*GetDefault<UDiffHelperSettings>()->UnrealDocURL, nullptr, nullptr); })
		);
		
		return;
	}
	
	SAssignNew(DiffHelperWindow, SDiffHelperWindow);
	DiffHelperWindow->GetOnWindowClosedEvent().AddLambda([this](const TSharedRef<SWindow>& Window)
	{
		DiffHelperWindow.Reset();
	});
	
	FSlateApplication::Get().AddWindow(DiffHelperWindow.ToSharedRef());
}

FDiffHelperModule& FDiffHelperModule::Get()
{
	return FModuleManager::LoadModuleChecked<FDiffHelperModule>("DiffHelper");
}

void FDiffHelperModule::RegisterMenus()
{
	// TODO: Move this auto-generated code to a different place
	
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			auto& Entry = Section.AddMenuEntryWithCommandList(FDiffHelperCommands::Get().OpenDiffWindow, PluginCommands);
			Entry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.Diff");
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FDiffHelperCommands::Get().OpenDiffWindow));
				Entry.SetCommandList(PluginCommands);
				Entry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.Diff");
			}

			// TODO: Temporary added for testing purposes
			FUIAction OpenStarshipSuiteAction;
			OpenStarshipSuiteAction.ExecuteAction = FExecuteAction::CreateLambda([this]()
			{
				RestoreStarshipSuite();
			});

			Section.AddEntry(FToolMenuEntry::InitToolBarButton(
					"OpenStarshipSuite",
					OpenStarshipSuiteAction,
					LOCTEXT("OpenStarshipSuite", "Starship Test Suite"),
					LOCTEXT("OpenStarshipSuite_ToolTip", "Opens the Starship UX test suite."),
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "PlacementBrowser.Icons.Testing"))
			);
		}
	}

	FDiffHelperDiffPanelToolbar::RegisterMenu();
	FDiffHelperCommitPanelToolbar::RegisterMenu();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDiffHelperModule, DiffHelper)
