// Copyright 2024 Gradess Games. All Rights Reserved.


#include "DiffHelperCacheManager.h"

const FString UDiffHelperCacheManager::ConfigSection = TEXT("DiffHelperCache");
const FString UDiffHelperCacheManager::ConfigSourceBranchKey = TEXT("SourceBranch");
const FString UDiffHelperCacheManager::ConfigTargetBranchKey = TEXT("TargetBranch");

void UDiffHelperCacheManager::SetSourceBranch(const FString& Branch)
{
	SourceBranchName = Branch;
	Cache();
}

void UDiffHelperCacheManager::SetTargetBranch(const FString& Branch)
{
	TargetBranchName = Branch;
	Cache();
}

void UDiffHelperCacheManager::Init()
{
	const auto ConfigPath = GetConfigPath();
	GConfig->GetString(*ConfigSection, *ConfigSourceBranchKey, SourceBranchName, ConfigPath);
	GConfig->GetString(*ConfigSection, *ConfigTargetBranchKey, TargetBranchName, ConfigPath);
}

void UDiffHelperCacheManager::Cache()
{
	const auto ConfigPath = GetConfigPath();
	GConfig->SetString(*ConfigSection, *ConfigSourceBranchKey, *SourceBranchName, ConfigPath);
	GConfig->SetString(*ConfigSection, *ConfigTargetBranchKey, *TargetBranchName, ConfigPath);
	GConfig->Flush(false);
}

FString UDiffHelperCacheManager::GetConfigPath() const
{
	return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("DiffHelperCache.ini"));
}
