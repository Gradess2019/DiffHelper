// Copyright 2024 Gradess Games. All Rights Reserved.


#include "DiffHelperGitManager.h"

#include "DiffHelperTypes.h"
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"
#include "SourceControlHelpers.h"
#include "SourceControlOperations.h"

#include "Misc/ScopedSlowTask.h"

bool UDiffHelperGitManager::Init()
{
	LoadGitBinaryPath();
	
	return !GitBinaryPath.IsEmpty();
}

FDiffHelperBranch UDiffHelperGitManager::GetCurrentBranch() const
{
	const auto& Provider = ISourceControlModule::Get().GetProvider();
	const auto Status = Provider.GetStatus();
	if (Status.Contains(ISourceControlProvider::EStatus::Branch))
	{
		return FDiffHelperBranch{ Status[ISourceControlProvider::EStatus::Branch] };
	}

	ensureMsgf(false, TEXT("Failed to get current branch"));
	return {};
}

TArray<FDiffHelperBranch> UDiffHelperGitManager::GetBranches() const
{
	FScopedSlowTask SlowTask(1.f, NSLOCTEXT("DiffHelper", "GetBranches", "Obtaining branches..."));

	const auto& Provider = ISourceControlModule::Get().GetProvider();
	const auto Status = Provider.GetStatus();

	if (!ensureMsgf(Status.Contains(ISourceControlProvider::EStatus::Repository), TEXT("Failed to get repository location")))
	{
		return {};
	}
	
	const auto& RepositoryRoot = Status[ISourceControlProvider::EStatus::Repository];
	
	
	return {};
}

TArray<FDiffHelperDiffItem> UDiffHelperGitManager::GetDiff(const FString& InSourceRevision, const FString& InTargetRevision) const
{
	return {};
}

TArray<FDiffHelperCommit> UDiffHelperGitManager::GetDiffCommitsList(const FDiffHelperBranch& InSourceBranch, const FDiffHelperBranch& InTargetBranch) const
{
	return {};
}

void UDiffHelperGitManager::LoadGitBinaryPath()
{
	FScopeLock ScopeLock(&CriticalSection);
	
	static const FString SettingsSection = TEXT("GitSourceControl.GitSourceControlSettings");
	const auto& IniFile = SourceControlHelpers::GetSettingsIni();
	GConfig->GetString(*SettingsSection, TEXT("BinaryPath"), GitBinaryPath, IniFile);
}

bool UDiffHelperGitManager::ExecuteCommand(const FString& InCommand, const TArray<FString>& InParameters, const TArray<FString>& InFiles, FString& OutResults, FString& OutErrors)
{
	
	return false;
}
