// Copyright 2024 Gradess Games. All Rights Reserved.

#include "DiffHelper.h"
#include "DiffHelperCacheManager.h"
#include "DiffHelperStyle.h"
#include "DiffHelperCommands.h"
#include "DiffHelperGitManager.h"
#include "DiffHelperSettings.h"
#include "DiffHelperTypes.h"
#include "DiffHelperUtils.h"
#include "ILiveCodingModule.h"
#include "ISettingsModule.h"
#include "ISourceControlModule.h"
#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/IPluginManager.h"
#include "UI/FDiffHelperCommitPanelToolbar.h"
#include "UI/FDiffHelperDiffPanelToolbar.h"
#include "UI/SDiffHelperPickerPanel.h"

#define LOCTEXT_NAMESPACE "FDiffHelperModule"

void FDiffHelperModule::StartupModule()
{
	InitializeStyle();
	InitializeCacheManager();
	
	RegisterSettings();
	RegisterCommands();
	RegisterTabSpawner();

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDiffHelperModule::RegisterMenus));
	
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
	
	UnregisterSettings();
}

FDiffHelperModule& FDiffHelperModule::Get()
{
	return FModuleManager::LoadModuleChecked<FDiffHelperModule>("DiffHelper");
}

bool FDiffHelperModule::ShouldBindLiveCodingUpdate() const
{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4
	const auto Plugin = IPluginManager::Get().FindEnabledPlugin("DiffHelper");
	return ensure(Plugin.IsValid()) && !Plugin->GetDescriptor().bInstalled;
#else
	const auto Plugin = IPluginManager::Get().FindPlugin("DiffHelper");
	return Plugin.IsValid() && !Plugin->GetDescriptor().bInstalled;
#endif
}

void FDiffHelperModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Editor", "Plugins", "DiffHelper",
			LOCTEXT("DiffHelperSettingsName", "Diff Helper"),
			LOCTEXT("DiffHelperSettingsDescription", "Configure the Diff Helper plugin"),
			GetMutableDefault<UDiffHelperSettings>()
		);
	}
}

void FDiffHelperModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Editor", "Plugins", "DiffHelper");
	}
}

void FDiffHelperModule::InitializeStyle()
{
	FDiffHelperStyle::Initialize();
	FDiffHelperStyle::ReloadTextures();
}

void FDiffHelperModule::InitializeCacheManager()
{
	CacheManager = TStrongObjectPtr(NewObject<UDiffHelperCacheManager>());
	CacheManager->Init();
}

void FDiffHelperModule::RegisterCommands()
{
	FDiffHelperCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FDiffHelperCommands::Get().OpenDiffWindow,
		FExecuteAction::CreateRaw(this, &FDiffHelperModule::ToolbarButtonClicked),
		FCanExecuteAction());
}

void FDiffHelperModule::RegisterTabSpawner()
{
	auto& SpawnerEntry = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		DiffHelperConstants::DiffHelperRevisionPickerId,
		FOnSpawnTab::CreateRaw(this, &FDiffHelperModule::SpawnTab),
		FCanSpawnTab::CreateRaw(this, &FDiffHelperModule::CanSpawnTab)
	);
	
	SpawnerEntry.SetDisplayName(LOCTEXT("MainTabTitle", "Diff Helper"));
	SpawnerEntry.SetTooltipText(LOCTEXT("MainTabTitleToolTip", "Open diff helper window."));
	SpawnerEntry.SetIcon(FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.Diff"));
	SpawnerEntry.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());
}

void FDiffHelperModule::RegisterMenus()
{
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float) { 
		IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
		if (!MainFrameModule.IsWindowInitialized())
		{
			return true;
		}
		
		FToolMenuOwnerScoped OwnerScoped(this);
		if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("MainFrame.MainMenu.Tools"))
		{
			if (FToolMenuSection* Section = Menu->FindSection("Source Control"))
			{
				auto& Entry = Section->AddMenuEntryWithCommandList(FDiffHelperCommands::Get().OpenDiffWindow, PluginCommands);
				Entry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.Diff");
			}
		}
		
		return false;
	}));

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

void FDiffHelperModule::ToolbarButtonClicked()
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

	FGlobalTabmanager::Get()->TryInvokeTab(DiffHelperConstants::DiffHelperRevisionPickerId);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDiffHelperModule, DiffHelper)
