// Copyright 2024 Gradess Games. All Rights Reserved.


#include "DiffHelperGitManager.h"
#include "DiffHelperSettings.h"
#include "DiffHelperTypes.h"
#include "EditorAssetLibrary.h"
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"
#include "SourceControlHelpers.h"
#include "SourceControlOperations.h"
#include "RevisionControlStyle/RevisionControlStyle.h"

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
	FDiffHelperBranch CurrentBranch;

	FString Command = TEXT("rev-parse --abbrev-ref HEAD");
	FString Results;
	FString Errors;

	if (!ExecuteCommand(Command, {}, {}, Results, Errors))
	{
		UE_LOG(LogSourceControl, Error, TEXT("Failed to get current branch: %s"), *Errors);
		return {};
	}

	CurrentBranch.Name = Results.TrimStartAndEnd();
	Results.Empty();

	Command = TEXT("rev-parse --short HEAD");
	if (!ExecuteCommand(Command, {}, {}, Results, Errors))
	{
		UE_LOG(LogSourceControl, Error, TEXT("Failed to get current branch revision: %s"), *Errors);
		return {};
	}

	CurrentBranch.Revision = Results.TrimStartAndEnd();

	return CurrentBranch;
}

TArray<FDiffHelperBranch> UDiffHelperGitManager::GetBranches() const
{
	const auto& RepositoryRoot = GetRepositoryDirectory();
	if (!RepositoryRoot.IsSet())
	{
		UE_LOG(LogSourceControl, Error, TEXT("Failed to get repository root!"));
	}

	TArray<FString> Params;
	Params.Add(TEXT("-v"));
	Params.Add(TEXT("--sort=committerdate"));

	FString Results;
	FString Errors;

	const auto Result = ExecuteCommand(TEXT("branch"), Params, {}, Results, Errors);
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
	const auto Commits = GetDiffCommitsList(InSourceRevision, InTargetRevision);

	TMap<FString, TArray<FDiffHelperCommit>> ChangedFiles;
	for (const auto& Commit : Commits)
	{
		for (const auto& File : Commit.Files)
		{
			if (!ChangedFiles.Contains(File.Path))
			{
				ChangedFiles.Add(File.Path, TArray<FDiffHelperCommit>());
			}

			ChangedFiles[File.Path].Add(Commit);
		}
	}

	TArray<FDiffHelperDiffItem> DiffItems;
	for (const auto& Pair : ChangedFiles)
	{
		FDiffHelperDiffItem DiffItem;
		DiffItem.Path = Pair.Key;

		const auto RelativePath = FPaths::Combine(FPaths::ProjectDir(), DiffItem.Path);
		if (FPaths::IsUnderDirectory(RelativePath, FPaths::ProjectContentDir()))
		{
			FString PackageName;
			if (FPackageName::TryConvertFilenameToLongPackageName(RelativePath, PackageName))
			{
				const auto AssetData = UEditorAssetLibrary::FindAssetData(PackageName);
				if (AssetData.IsValid())
				{
					DiffItem.AssetData = AssetData;
				}
			}
		}

		DiffItem.Commits = Pair.Value;
		DiffItems.Add(DiffItem);
	}

	return DiffItems;
}

TArray<FDiffHelperCommit> UDiffHelperGitManager::GetDiffCommitsList(const FString& InSourceBranch, const FString& InTargetBranch) const
{
	const FString Command = TEXT("log");
	FString Result;
	FString Errors;

	TArray<FString> Params;
	Params.Add(TEXT("--pretty=format:\"<Hash:%h> <Message:%s> <Author:%an> <Date:%ad>\""));
	Params.Add(TEXT("--date=format-local:\"%d/%m/%Y %H:%M\""));
	Params.Add(TEXT("--name-status"));
	Params.Add(InTargetBranch + TEXT("..") + InSourceBranch);

	if (!ExecuteCommand(Command, Params, {}, Result, Errors))
	{
		UE_LOG(LogSourceControl, Error, TEXT("Failed to get diff commits: %s"), *Errors);
		return {};
	}

	TArray<FDiffHelperCommit> Commits = ParseCommits(Result);
	return Commits;
}

FSlateIcon UDiffHelperGitManager::GetStatusIcon(const EDiffHelperFileStatus InStatus) const
{
	switch (InStatus)
	{
	case EDiffHelperFileStatus::Added: return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.OpenForAdd");
	case EDiffHelperFileStatus::Modified: return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.CheckedOut");
	case EDiffHelperFileStatus::Deleted: return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.MarkedForDelete");
	case EDiffHelperFileStatus::Renamed:
	case EDiffHelperFileStatus::Copied: return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.Branched");
	case EDiffHelperFileStatus::Unmerged: return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.Conflicted");
	default: return FSlateIcon();
	}
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

	for (const auto& Parameter : InParameters)
	{
		FullCommand += TEXT(" ");
		FullCommand += Parameter;
	}

	FPlatformProcess::ExecProcess(*GitBinaryPath, *FullCommand, &ReturnCode, &OutResults, &OutErrors, *RepositoryRoot);

	return ReturnCode == 0;
}

TOptional<FString> UDiffHelperGitManager::GetForkPoint(const FDiffHelperBranch& InSourceBranch, const FDiffHelperBranch& InTargetBranch) const
{
	const auto Command = FString::Printf(TEXT("rev-parse --short \"$(git rev-list %s ^%s | tail -n 1)^\""), *InSourceBranch.Name, *InTargetBranch.Name);
	FString Result;
	FString Errors;

	if (!ExecuteCommand(Command, {}, {}, Result, Errors))
	{
		UE_LOG(LogSourceControl, Error, TEXT("Failed to get fork point: %s"), *Errors);
		return TOptional<FString>();
	}

	return TOptional(Result.TrimStartAndEnd());
}

TArray<FDiffHelperBranch> UDiffHelperGitManager::ParseBranches(const FString& InBranches) const
{
	const auto* DiffHelperSettings = GetDefault<UDiffHelperSettings>();
	const auto Pattern = FRegexPattern(DiffHelperSettings->BranchParserPattern);
	auto Matcher = FRegexMatcher(Pattern, InBranches);

	TArray<FDiffHelperBranch> Branches;
	while (Matcher.FindNext())
	{
		const auto BranchName = Matcher.GetCaptureGroup(DiffHelperSettings->BranchNameGroup);
		const auto BranchRevision = Matcher.GetCaptureGroup(DiffHelperSettings->BranchRevisionGroup);

		Branches.Emplace(BranchName, BranchRevision);
	}

	return Branches;
}

TArray<FDiffHelperCommit> UDiffHelperGitManager::ParseCommits(const FString& InCommits) const
{
	TArray<FDiffHelperCommit> Commits;

	const auto* Settings = GetDefault<UDiffHelperSettings>();
	const auto CommitBlockPattern = FRegexPattern(Settings->CommitBlockPattern);
	const auto CommitDataPattern = FRegexPattern(Settings->CommitDataPattern);

	auto BlockMatcher = FRegexMatcher(CommitBlockPattern, InCommits);
	while (BlockMatcher.FindNext())
	{
		const auto CommitData = BlockMatcher.GetCaptureGroup(0);
		auto DataMatcher = FRegexMatcher(CommitDataPattern, CommitData);

		FDiffHelperCommit Commit;
		if (DataMatcher.FindNext())
		{
			Commit.Revision = DataMatcher.GetCaptureGroup(Settings->HashGroup);
			Commit.Message = DataMatcher.GetCaptureGroup(Settings->MessageGroup);
			Commit.Author = DataMatcher.GetCaptureGroup(Settings->AuthorGroup);
			Commit.Date = ParseDate(DataMatcher.GetCaptureGroup(Settings->DateGroup));

			const auto ChangedFiles = DataMatcher.GetCaptureGroup(Settings->ChangedFilesGroup);
			Commit.Files = ParseChangedFiles(ChangedFiles);
		}

		Commits.Add(Commit);
	}

	return Commits;
}

FDateTime UDiffHelperGitManager::ParseDate(const FString& InDate) const
{
	const auto* Settings = GetDefault<UDiffHelperSettings>();
	const auto Pattern = FRegexPattern(Settings->DatePattern);
	auto Matcher = FRegexMatcher(Pattern, InDate);

	if (Matcher.FindNext())
	{
		const auto Day = FCString::Atoi(*Matcher.GetCaptureGroup(Settings->DayGroup));
		const auto Month = FCString::Atoi(*Matcher.GetCaptureGroup(Settings->MonthGroup));
		const auto Year = FCString::Atoi(*Matcher.GetCaptureGroup(Settings->YearGroup));
		const auto Hour = FCString::Atoi(*Matcher.GetCaptureGroup(Settings->HourGroup));
		const auto Minute = FCString::Atoi(*Matcher.GetCaptureGroup(Settings->MinuteGroup));

		return FDateTime(Year, Month, Day, Hour, Minute);
	}

	return {};
}

TArray<FDiffHelperFileData> UDiffHelperGitManager::ParseChangedFiles(const FString& InFiles) const
{
	const auto* Settings = GetDefault<UDiffHelperSettings>();
	const auto Pattern = FRegexPattern(Settings->ChangedFilePattern);
	auto Matcher = FRegexMatcher(Pattern, InFiles);

	TArray<FDiffHelperFileData> Files;
	while (Matcher.FindNext())
	{
		const auto Status = Matcher.GetCaptureGroup(Settings->ChangedFileStatusGroup);
		const auto Path = Matcher.GetCaptureGroup(Settings->ChangedFilePathGroup);

		FDiffHelperFileData FileData;
		FileData.Path = Path;
		FileData.Status = ConvertFileStatus(Status);
		Files.Add(FileData);
	}

	return Files;
}

EDiffHelperFileStatus UDiffHelperGitManager::ConvertFileStatus(const FString& InStatus) const
{
	if (InStatus == TEXT("A")) { return EDiffHelperFileStatus::Added; }
	if (InStatus == TEXT("M")) { return EDiffHelperFileStatus::Modified; }
	if (InStatus == TEXT("D")) { return EDiffHelperFileStatus::Deleted; }
	if (InStatus == TEXT("R")) { return EDiffHelperFileStatus::Renamed; }
	if (InStatus == TEXT("C")) { return EDiffHelperFileStatus::Copied; }
	if (InStatus == TEXT("U")) { return EDiffHelperFileStatus::Unmerged; }

	return EDiffHelperFileStatus::None;
}

#undef LOCTEXT_NAMESPACE
