// Copyright Epic Games, Inc. All Rights Reserved.

#include "DiffHelper.h"
#include "DiffHelperCacheManager.h"
#include "DiffHelperStyle.h"
#include "DiffHelperCommands.h"
#include "DiffHelperGitManager.h"
#include "DiffHelperTypes.h"
#include "ILiveCodingModule.h"
#include "ToolMenus.h"

#include "UI/FDiffHelperCommitPanelToolbar.h"
#include "UI/SDiffHelperWindow.h"

static const FName DiffHelperTabName("DiffHelper");

#define LOCTEXT_NAMESPACE "FDiffHelperModule"

void FDiffHelperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FDiffHelperStyle::Initialize();
	FDiffHelperStyle::ReloadTextures();

	DiffHelperManager = NewObject<UDiffHelperGitManager>();
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
	// TODO: crash on shutdown
	if (DiffHelperManager)
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
		}
	}

	FDiffHelperCommitPanelToolbar::RegisterMenu();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDiffHelperModule, DiffHelper)
