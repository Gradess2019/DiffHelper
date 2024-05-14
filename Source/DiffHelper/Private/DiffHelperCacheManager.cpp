// Copyright 2024 Gradess Games. All Rights Reserved.


#include "DiffHelperCacheManager.h"

void UDiffHelperCacheManager::SetSourceBranch(const FDiffHelperBranch& Branch)
{
	SourceBranch = Branch;
	Cache();	
}

void UDiffHelperCacheManager::SetTargetBranch(const FDiffHelperBranch& Branch)
{
	TargetBranch = Branch;
	Cache();
}

void UDiffHelperCacheManager::Cache()
{
	
	const auto ConfigPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("DiffHelperCache.ini"));
	GConfig->SetString(TEXT("DiffHelper"), TEXT("SourceBranch"), *SourceBranch.Name, ConfigPath);
	GConfig->SetString(TEXT("DiffHelper"), TEXT("TargetBranch"), *TargetBranch.Name, ConfigPath);
	GConfig->Flush(false);
}
