// Copyright 2024 Gradess Games. All Rights Reserved.


#include "Managers/DiffHelperPerforceManager.h"

#include "DiffHelperSettings.h"
#include "DiffHelperTypes.h"

bool UDiffHelperPerforceManager::Init()
{
	AddToRoot();
	return true;
}

void UDiffHelperPerforceManager::Deinit()
{
	RemoveFromRoot();
}

FDiffHelperBranch UDiffHelperPerforceManager::GetCurrentBranch() const
{
	return FDiffHelperBranch();
}

TArray<FDiffHelperBranch> UDiffHelperPerforceManager::GetBranches() const
{
	const FString Command = TEXT("changes");
	FString Result;
	FString Errors;

	if (!ExecuteCommand(Command, {}, {}, Result, Errors))
	{
		return {};
	}

	auto Changes = ParseChanges(Result);
	
	return Changes;
}

TArray<FDiffHelperDiffItem> UDiffHelperPerforceManager::GetDiff(const FString& InSourceRevision, const FString& InTargetRevision) const
{
	return {};

}

TArray<FDiffHelperCommit> UDiffHelperPerforceManager::GetDiffCommitsList(const FString& InSourceBranch, const FString& InTargetBranch) const
{
	return {};

}

FDiffHelperCommit UDiffHelperPerforceManager::GetLastCommitForFile(const FString& InFilePath, const FString& InBranch) const
{
	return FDiffHelperCommit();
}

FSlateIcon UDiffHelperPerforceManager::GetStatusIcon(const EDiffHelperFileStatus InStatus) const
{
	return FSlateIcon();
}

TOptional<FString> UDiffHelperPerforceManager::GetFile(const FString& InFilePath, const FString& InRevision) const
{
	return {};
}

bool UDiffHelperPerforceManager::ExecuteCommand(const FString& InCommand, const TArray<FString>& InParameters, const TArray<FString>& InFiles, FString& OutResults, FString& OutErrors) const
{
	SCOPED_NAMED_EVENT_F(TEXT("UDiffHelperPerforceManager_ExecuteCommand: %s"), FColor::Red, *InCommand);
	
	const auto RepositoryRoot = FPaths::ProjectDir();

	int32 ReturnCode = -1;
	FString FullCommand = InCommand;

	for (const auto& Parameter : InParameters)
	{
		FullCommand += TEXT(" ");
		FullCommand += Parameter;
	}

	FPlatformProcess::ExecProcess(TEXT("p4.exe"), *FullCommand, &ReturnCode, &OutResults, &OutErrors, *RepositoryRoot);

	return ReturnCode == 0;
}

TArray<FDiffHelperBranch> UDiffHelperPerforceManager::ParseChanges(const FString& InChanges) const
{
	SCOPED_NAMED_EVENT(FDiffHelperPerforceManager_ParseChanges, FColor::Red);

	const auto* DiffHelperSettings = GetDefault<UDiffHelperSettings>();
	const auto Pattern = FRegexPattern(DiffHelperSettings->ChangePattern);
	auto Matcher = FRegexMatcher(Pattern, InChanges);

	TArray<FDiffHelperBranch> Changes;
	while (Matcher.FindNext())
	{
		const auto ChangeRevision = Matcher.GetCaptureGroup(1);

		FDiffHelperBranch Change;
		Change.Revision = ChangeRevision;
		Change.Name = ChangeRevision;

		Changes.Add(Change);
	}

	return Changes;
}

