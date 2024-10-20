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

#include "Interfaces/IPluginManager.h"

#include "UI/FDiffHelperCommitPanelToolbar.h"
#include "UI/FDiffHelperDiffPanelToolbar.h"
#include "UI/SDiffHelperPickerPanel.h"

#include "Widgets/Testing/SStarshipSuite.h"

#define LOCTEXT_NAMESPACE "FDiffHelperModule"

void FDiffHelperModule::StartupModule()
{
	FDiffHelperStyle::Initialize();
	FDiffHelperStyle::ReloadTextures();

	CacheManager = TStrongObjectPtr(NewObject<UDiffHelperCacheManager>());
	CacheManager->Init();
	
	FDiffHelperCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FDiffHelperCommands::Get().OpenDiffWindow,
		FExecuteAction::CreateRaw(this, &FDiffHelperModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDiffHelperModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(DiffHelperConstants::DiffHelperRevisionPickerId,
		FOnSpawnTab::CreateRaw(this, &FDiffHelperModule::SpawnTab),
		FCanSpawnTab::CreateRaw(this, &FDiffHelperModule::CanSpawnTab)
	);
	
	if (ShouldBindLiveCodingUpdate())
	{
		BindLiveCodingUpdate();
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

	// TODO: Check revision control was set up properly, if it changed, then manager should be changed as well
	if (!DiffHelperManager.IsValid())
	{
		DiffHelperManager = TWeakInterfacePtr<IDiffHelperManager>(NewObject<UDiffHelperGitManager>());
		DiffHelperManager->Init();
	}

	FGlobalTabmanager::Get()->InsertNewDocumentTab(DiffHelperConstants::DiffHelperRevisionPickerId, FTabManager::FLiveTabSearch(DiffHelperConstants::DiffHelperRevisionPickerId), SpawnTab(FSpawnTabArgs(nullptr, FName())));
}

FDiffHelperModule& FDiffHelperModule::Get()
{
	return FModuleManager::LoadModuleChecked<FDiffHelperModule>("DiffHelper");
}

bool FDiffHelperModule::ShouldBindLiveCodingUpdate() const
{
	const auto Plugin = IPluginManager::Get().FindEnabledPlugin("DiffHelper");
	return ensure(Plugin.IsValid()) && !Plugin->GetDescriptor().bInstalled;
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

void FDiffHelperModule::BindLiveCodingUpdate()
{
	if (FModuleManager::Get().IsModuleLoaded("LiveCoding"))
	{
		auto& LiveCodingModule = FModuleManager::GetModuleChecked<ILiveCodingModule>("LiveCoding");
		LiveCodingModule.GetOnPatchCompleteDelegate().AddRaw(this, &FDiffHelperModule::UpdateSlateStyle);
	}
}

void FDiffHelperModule::UpdateSlateStyle()
{
	FDiffHelperStyle::ReloadStyles();
	FDiffHelperStyle::ReloadTextures();
}

TSharedRef<SDockTab> FDiffHelperModule::SpawnTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(LOCTEXT("DiffHelperRevisionPickerTabTitle", "Revision Picker"))
		[
			SNew(SDiffHelperPickerPanel)
		];
}

bool FDiffHelperModule::CanSpawnTab(const FSpawnTabArgs& Args) const
{
	return DiffHelperManager.IsValid();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDiffHelperModule, DiffHelper)
