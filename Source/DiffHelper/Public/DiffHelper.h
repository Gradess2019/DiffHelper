// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UDiffHelperCacheManager;
class SDiffHelperWindow;
class IDiffHelperManager;
class FToolBarBuilder;
class FMenuBuilder;

class FDiffHelperModule : public IModuleInterface
{
protected:
	TWeakInterfacePtr<IDiffHelperManager> DiffHelperManager = nullptr;
	TStrongObjectPtr<UDiffHelperCacheManager> CacheManager;
	TSharedPtr<SDiffHelperWindow> DiffHelperWindow;
	
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();

	static FDiffHelperModule& Get();

	TWeakInterfacePtr<IDiffHelperManager> GetManager() const { return DiffHelperManager; }
	UDiffHelperCacheManager* GetCacheManager() const { return CacheManager.Get(); }
	
private:
	void RegisterMenus();


private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
