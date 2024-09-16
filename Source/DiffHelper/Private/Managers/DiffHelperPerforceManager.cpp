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

	auto Changes = ParseRevisions(Result);
	
	return Changes;
}

TArray<FDiffHelperDiffItem> UDiffHelperPerforceManager::GetDiff(const FString& InSourceRevision, const FString& InTargetRevision) const
{
	return {};

}

TArray<FDiffHelperCommit> UDiffHelperPerforceManager::GetDiffCommitsList(const FString& InSourceBranch, const FString& InTargetBranch) const
{
	const FString Command = TEXT("changes");
	FString Result;
	FString Errors;

	TArray<FString> Params;
	Params.Add(FString::Printf(TEXT("...@%s,%s"), *InTargetBranch, *InSourceBranch));

	if (!ExecuteCommand(Command, Params, {}, Result, Errors))
	{
		return {};
	}

	auto Changes = ParseRevisions(Result);
	if (Changes.Num() < 0)
	{
		return {};
	}
	
	TArray<FDiffHelperCommit> Commits;
	for (const auto& Change : Changes)
	{
		auto Commit = ParseChanges(Change.Revision);
		if (Commit.IsValid())
		{
			Commits.Add(Commit);
		}
	}

	return Commits;
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
	
	// exec process command has limit of 8192 characters
	// we need to use while loop to execute command with more than 8192 characters
	// we will split command into smaller parts and execute them one by one
	int32 ReturnCode = -1;
	int32 LastParamIndex = 0;
	int32 ReservedSpace = 512;
	int32 CommandCharLimit = 8191 - 6 - InCommand.Len() - ReservedSpace;

	do
	{
		FString PartialResult;
		FString FullCommand = InCommand;
		
		for (int32 ParamId = LastParamIndex; ParamId < InParameters.Num(); ++ParamId)
		{
			// +2 for space and null terminator
			auto NewCommandLength = FullCommand.Len() + InParameters[ParamId].Len() + 2;
			if (NewCommandLength >= CommandCharLimit)
			{
				break;
			}
			
			FullCommand += TEXT(" ");
			FullCommand += InParameters[ParamId];
			LastParamIndex = ParamId + 1;
		}

		FPlatformProcess::ExecProcess(TEXT("p4.exe"), *FullCommand, &ReturnCode, &PartialResult, &OutErrors, *RepositoryRoot);
		if (ReturnCode != 0)
		{
			return false;
		}

		OutResults += PartialResult + TEXT("\n");
	} while (LastParamIndex < InParameters.Num());

	return ReturnCode == 0;
}

EDiffHelperFileStatus UDiffHelperPerforceManager::ConvertFileStatus(const FString& InStatus) const
{
	if (InStatus.Equals(TEXT("add"), ESearchCase::IgnoreCase)) { return EDiffHelperFileStatus::Added; }
	if (InStatus.Equals(TEXT("edit"), ESearchCase::IgnoreCase)) { return EDiffHelperFileStatus::Modified; }
	if (InStatus.Equals(TEXT("delete"), ESearchCase::IgnoreCase)) { return EDiffHelperFileStatus::Deleted; }
	if (InStatus.Equals(TEXT("move"), ESearchCase::IgnoreCase)) { return EDiffHelperFileStatus::Renamed; }
	if (InStatus.Equals(TEXT("move/add"), ESearchCase::IgnoreCase)) { return EDiffHelperFileStatus::Renamed; }
	if (InStatus.Equals(TEXT("move/delete"), ESearchCase::IgnoreCase)) { return EDiffHelperFileStatus::Renamed; }
	return EDiffHelperFileStatus::None;	
}

TMap<FString, FString> UDiffHelperPerforceManager::GetFileLocations(const TSet<FString>& InDepotFiles) const
{
	// p4 -ztag where <depot_path1> <depot_path2> ...
	const FString Command = TEXT("-ztag where");
	FString Result;
	FString Errors;

	TArray<FString> Params;
	for (const auto& DepotPath : InDepotFiles)
	{
		Params.Add(DepotPath);
	}

	if (!ExecuteCommand(Command, Params, {}, Result, Errors))
	{
		return {};
	}

	return ParseFileLocations(Result);
}

TArray<FDiffHelperBranch> UDiffHelperPerforceManager::ParseRevisions(const FString& InChanges) const
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

FDiffHelperCommit UDiffHelperPerforceManager::ParseChanges(const FString& InRevision) const
{
	const FString Command = TEXT("-ztag describe");
	FString Result;
	FString Errors;
	
	TArray<FString> Params;
	Params.Add(InRevision);

	if (!ExecuteCommand(Command, Params, {}, Result, Errors))
	{
		return {};
	}

	const auto* DiffHelperSettings = GetDefault<UDiffHelperSettings>();
	const auto HeaderPattern = FRegexPattern(DiffHelperSettings->DescribeHeaderPattern);
	const auto BodyPattern = FRegexPattern(DiffHelperSettings->DescribeBodyPattern);
	auto HeaderMatcher = FRegexMatcher(HeaderPattern, Result);
	auto FileMatcher = FRegexMatcher(BodyPattern, Result);

	FDiffHelperCommit Commit;

	while (HeaderMatcher.FindNext())
	{
		const auto ChangeRevision = HeaderMatcher.GetCaptureGroup(DiffHelperSettings->DescribeHeaderChangeGroup);
		const auto Author = HeaderMatcher.GetCaptureGroup(DiffHelperSettings->DescribeHeaderUserGroup);
		const auto Timestamp = HeaderMatcher.GetCaptureGroup(DiffHelperSettings->DescribeHeaderTimeGroup);
		const auto Message = HeaderMatcher.GetCaptureGroup(DiffHelperSettings->DescribeHeaderMessageGroup);

		Commit.Revision = ChangeRevision;
		Commit.Author = Author;
		Commit.Date = FDateTime::FromUnixTimestamp(FCString::Atoi(*Timestamp));
		Commit.Message = Message;
	}

	if (!Commit.IsValid())
	{
		return {};
	}

	TSet<FString> DepotPaths;
	TMap<FString, FString> OSPaths;
	while (FileMatcher.FindNext())
	{
		const auto DepotPath = FileMatcher.GetCaptureGroup(DiffHelperSettings->DescribeBodyFileGroup);
		const auto Status = FileMatcher.GetCaptureGroup(DiffHelperSettings->DescribeBodyStatusGroup);

		FDiffHelperFileData FileData;
		FileData.Path = DepotPath;
		FileData.Status = ConvertFileStatus(Status);

		DepotPaths.Add(DepotPath);

		Commit.Files.Add(FileData);
	}

	OSPaths = GetFileLocations(DepotPaths);
	for (auto& File : Commit.Files)
	{
		if (OSPaths.Contains(File.Path))
		{
			File.Path = OSPaths[File.Path];
		}
	}

	return Commit;
}

TMap<FString, FString> UDiffHelperPerforceManager::ParseFileLocations(const FString& InLocations) const
{
	const auto* DiffHelperSettings = GetDefault<UDiffHelperSettings>();
	const auto Pattern = FRegexPattern(DiffHelperSettings->WherePattern);
	auto Matcher = FRegexMatcher(Pattern, InLocations);

	TMap<FString, FString> Locations;
	while (Matcher.FindNext())
	{
		const auto DepotFile = Matcher.GetCaptureGroup(DiffHelperSettings->WhereDepotFileGroup);
		const auto Path = Matcher.GetCaptureGroup(DiffHelperSettings->WherePathGroup);

		Locations.Add(DepotFile, Path);
	}

	return Locations;
}