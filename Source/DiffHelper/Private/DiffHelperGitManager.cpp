// Copyright 2024 Gradess Games. All Rights Reserved.


#include "DiffHelperGitManager.h"

#include "DiffHelperTypes.h"
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"
#include "SourceControlHelpers.h"
#include "SourceControlOperations.h"

#include "Misc/ScopedSlowTask.h"

// define localization namespace
#define LOCTEXT_NAMESPACE "DiffHelperGitManager"

bool UDiffHelperGitManager::Init()
{
	AddToRoot();
	LoadGitBinaryPath();
	
	return !GitBinaryPath.IsEmpty();
}

void UDiffHelperGitManager::Deinit()
{
	RemoveFromRoot();
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
	FScopedSlowTask SlowTask(1.f, LOCTEXT("GetBranches", "Obtaining branches..."));
	SlowTask.MakeDialogDelayed(1.f);

	const auto& RepositoryRoot = GetRepositoryDirectory();
	if (!RepositoryRoot.IsSet())
	{
		UE_LOG(LogSourceControl, Error, TEXT("Failed to get repository root!"));
	}
	
	FString Results;
	FString Errors;
	
	const auto Result = ExecuteCommand(TEXT("branch"), {}, {}, Results, Errors);
	if (!Result)
	{
		UE_LOG(LogSourceControl, Error, TEXT("Failed to get branches: %s"), *Errors);
		return {};
	}

	const auto Branches = ParseBranches(Results);
	return Branches;
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

TOptional<FString> UDiffHelperGitManager::GetRepositoryDirectory() const
{
	const auto& Provider = ISourceControlModule::Get().GetProvider();
	const auto Status = Provider.GetStatus();
	return Status.Contains(ISourceControlProvider::EStatus::Repository) ? TOptional<FString>(Status[ISourceControlProvider::EStatus::Repository]) : TOptional<FString>();
}

bool UDiffHelperGitManager::ExecuteCommand(const FString& InCommand, const TArray<FString>& InParameters, const TArray<FString>& InFiles, FString& OutResults, FString& OutErrors) const
{
	const auto RepositoryRoot = GetRepositoryDirectory().GetValue();
	
	int32 ReturnCode = -1;
	FString FullCommand = InCommand;

	for(const auto& Parameter : InParameters)
	{
		FullCommand += TEXT(" ");
		FullCommand += Parameter;
	}

	FPlatformProcess::ExecProcess(*GitBinaryPath, *InCommand, &ReturnCode, &OutResults, &OutErrors, *RepositoryRoot);
	
	return ReturnCode == 0;
}

TArray<FDiffHelperBranch> UDiffHelperGitManager::ParseBranches(const FString& InBranches) const
{
	TArray<FDiffHelperBranch> Branches;
	TArray<FString> Lines;
	InBranches.ParseIntoArrayLines(Lines);
	
	for (const auto& Line : Lines)
	{
		Branches.Emplace(Line.RightChop(2));
	}
	
	return Branches;
}

#undef LOCTEXT_NAMESPACE