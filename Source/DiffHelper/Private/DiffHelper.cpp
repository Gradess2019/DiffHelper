// Copyright Epic Games, Inc. All Rights Reserved.

#include "DiffHelper.h"
#include "DiffHelperStyle.h"
#include "DiffHelperCommands.h"
#include "DiffHelperGitManager.h"
#include "DiffHelperTypes.h"
#include "ToolMenus.h"

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

	FDiffHelperCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FDiffHelperCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FDiffHelperModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDiffHelperModule::RegisterMenus));
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
	FSlateApplication::Get().AddWindow(DiffHelperWindow.ToSharedRef());
}

FDiffHelperModule& FDiffHelperModule::Get()
{
	return FModuleManager::LoadModuleChecked<FDiffHelperModule>("DiffHelper");
}

void FDiffHelperModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FDiffHelperCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FDiffHelperCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDiffHelperModule, DiffHelper)
