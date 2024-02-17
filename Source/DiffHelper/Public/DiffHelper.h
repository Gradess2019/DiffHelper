// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class SDiffHelperWindow;
class IDiffHelperManager;
class FToolBarBuilder;
class FMenuBuilder;

class FDiffHelperModule : public IModuleInterface
{
protected:
	IDiffHelperManager* DiffHelperManager = nullptr;
	TSharedPtr<SDiffHelperWindow> DiffHelperWindow;
	
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();

	static FDiffHelperModule& Get();

	const IDiffHelperManager* GetManager() const { return DiffHelperManager; }
	
private:

	void RegisterMenus();


private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
