// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UDiffHelperCacheManager;
class IDiffHelperManager;
class FToolBarBuilder;
class FMenuBuilder;

class FDiffHelperModule : public IModuleInterface
{
protected:
	TWeakInterfacePtr<IDiffHelperManager> DiffHelperManager = nullptr;
	TStrongObjectPtr<UDiffHelperCacheManager> CacheManager;
	
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	static FDiffHelperModule& Get();

	TWeakInterfacePtr<IDiffHelperManager> GetManager() const { return DiffHelperManager; }
	UDiffHelperCacheManager* GetCacheManager() const { return CacheManager.Get(); }
	
private:
	bool ShouldBindLiveCodingUpdate() const;

	void RegisterSettings();
	void UnregisterSettings();

	void InitializeStyle();
	void InitializeCacheManager();
	void RegisterCommands();
	void RegisterTabSpawner();
	void RegisterMenus();
	
	void BindLiveCodingUpdate();
	void UpdateSlateStyle();

	TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& Args);
	bool CanSpawnTab(const FSpawnTabArgs& Args) const;

	void ToolbarButtonClicked();

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
