// Copyright 2024 Gradess Games. All Rights Reserved.


#include "DiffHelperGitManager.h"
#include "DiffHelperSettings.h"
#include "DiffHelperTypes.h"
#include "DiffHelperUtils.h"
#include "EditorAssetLibrary.h"
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"
#include "SourceControlHelpers.h"
#include "Algo/Accumulate.h"

#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION <= 2
#include "Internationalization/Regex.h"
#include "Misc/FileHelper.h"
#endif

#if ENGINE_MAJOR_VERSION >=5 && ENGINE_MINOR_VERSION >= 2
#include "RevisionControlStyle/RevisionControlStyle.h"
#endif

#define LOCTEXT_NAMESPACE "DiffHelperGitManager"

bool UDiffHelperGitManager::Init()
{
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_Init, FColor::Red);
	
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
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_GetCurrentBranch, FColor::Red);
	
	FDiffHelperBranch CurrentBranch;

	FString Command = TEXT("rev-parse --abbrev-ref HEAD");
	FString Results;
	FString Errors;

	if (!ExecuteCommand(Command, {}, {}, Results, Errors))
	{
		UE_LOG(LogDiffHelper, Error, TEXT("Failed to get current branch: %s"), *Errors);
		return {};
	}

	CurrentBranch.Name = Results.TrimStartAndEnd();
	Results.Empty();

	Command = TEXT("rev-parse --short HEAD");
	if (!ExecuteCommand(Command, {}, {}, Results, Errors))
	{
		UE_LOG(LogDiffHelper, Error, TEXT("Failed to get current branch revision: %s"), *Errors);
		return {};
	}

	CurrentBranch.Revision = Results.TrimStartAndEnd();

	return CurrentBranch;
}

TArray<FDiffHelperBranch> UDiffHelperGitManager::GetBranches() const
{
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_GetBranches, FColor::Red);
	
	const auto& RepositoryRoot = GetRepositoryDirectory();
	if (!RepositoryRoot.IsSet())
	{
		UE_LOG(LogDiffHelper, Error, TEXT("Failed to get repository root!"));
		return {};
	}

	TArray<FString> Params;
	Params.Add(TEXT("-v"));
	Params.Add(TEXT("--sort=committerdate"));

	FString Results;
	FString Errors;

	const auto Result = ExecuteCommand(TEXT("branch"), Params, {}, Results, Errors);
	if (!Result)
	{
		UE_LOG(LogDiffHelper, Error, TEXT("Failed to get branches: %s"), *Errors);
		return {};
	}

	auto Branches = ParseBranches(Results);

	// Auxiliary branch for the current commited state
	FDiffHelperBranch HeadBranch;
	HeadBranch.Name = TEXT("HEAD");
	HeadBranch.Revision = TEXT("HEAD");

	if (Branches.Num() > 0 && Branches[0].Name.Equals(HeadBranch.Name))
	{
		Branches[0] = HeadBranch;
	}
	else
	{
		Branches.Insert(HeadBranch, 0);
	}
	
	return Branches;
}

TArray<FDiffHelperDiffItem> UDiffHelperGitManager::GetDiff(const FString& InSourceRevision, const FString& InTargetRevision) const
{
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_GetDiff, FColor::Red);
	
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

	const auto& Statuses = GetStatuses(InSourceRevision, InTargetRevision);

	TArray<FString> Files;
	ChangedFiles.GetKeys(Files);
	const auto LastCommits = GetLastCommitForFiles(Files, InTargetRevision);

	TArray<FDiffHelperDiffItem> DiffItems;
	for (const auto& Pair : ChangedFiles)
	{
		FDiffHelperDiffItem DiffItem;
		DiffItem.Path = Pair.Key;
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 4
		DiffItem.Status = Statuses.FindRef(DiffItem.Path, EDiffHelperFileStatus::None);
#else
		DiffItem.Status = Statuses.Contains(DiffItem.Path) ? Statuses.FindRef(DiffItem.Path) : EDiffHelperFileStatus::None;
#endif
		

		if (DiffItem.Status == EDiffHelperFileStatus::None)
		{
			UE_LOG(LogDiffHelper, Error, TEXT("Failed to get status for file: %s"), *DiffItem.Path);
		}

		const auto RelativePath = FPaths::Combine(FPaths::ProjectDir(), DiffItem.Path);
		if (FPaths::IsUnderDirectory(RelativePath, FPaths::ProjectContentDir()))
		{
			FString PackageName;
			if (FPackageName::TryConvertFilenameToLongPackageName(RelativePath, PackageName) && UDiffHelperUtils::IsUnrealAsset(PackageName))
			{
				const auto AssetData = UEditorAssetLibrary::FindAssetData(PackageName);
				if (AssetData.IsValid())
				{
					DiffItem.AssetData = AssetData;
				}
			}
		}

		DiffItem.LastTargetCommit = LastCommits.FindRef(DiffItem.Path);
		DiffItem.Commits = Pair.Value;
		DiffItems.Add(DiffItem);
	}

	return DiffItems;
}

TArray<FDiffHelperCommit> UDiffHelperGitManager::GetDiffCommitsList(const FString& InSourceBranch, const FString& InTargetBranch) const
{
	SCOPED_NAMED_EVENT(FDiffHelperGitManager_GetDiffCommitsList, FColor::Red);
	
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
		UE_LOG(LogDiffHelper, Error, TEXT("Failed to get diff commits: %s"), *Errors);
		return {};
	}

	TArray<FDiffHelperCommit> Commits = ParseCommits(Result);
	return Commits;
}

FDiffHelperCommit UDiffHelperGitManager::GetLastCommitForFile(const FString& InFilePath, const FString& InBranch) const
{
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_GetLastCommitForFile, FColor::Red);
	
	const FString Command = TEXT("log");
	FString Result;
	FString Errors;

	TArray<FString> Params;
	Params.Add(InBranch);
	Params.Add(TEXT("-n 1"));
	Params.Add(TEXT("--pretty=format:\"<Hash:%h> <Message:%s> <Author:%an> <Date:%ad>\""));
	Params.Add(TEXT("--date=format-local:\"%d/%m/%Y %H:%M\""));
	Params.Add(TEXT("--name-status"));
	Params.Add(TEXT("-- ") + InFilePath);

	if (!ExecuteCommand(Command, Params, {}, Result, Errors))
	{
		UE_LOG(LogDiffHelper, Error, TEXT("Failed to get last commit for file: %s. Error: %s"), *InFilePath, *Errors);
		return {};
	}

	FDiffHelperCommit Commit = ParseCommit(Result);
	return Commit;
}

FSlateIcon UDiffHelperGitManager::GetStatusIcon(const EDiffHelperFileStatus InStatus) const
{
	switch (InStatus)
	{
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 2
	case EDiffHelperFileStatus::Added: return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.OpenForAdd");
	case EDiffHelperFileStatus::Modified: return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.CheckedOut");
	case EDiffHelperFileStatus::Deleted: return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.MarkedForDelete");
	case EDiffHelperFileStatus::Renamed:
	case EDiffHelperFileStatus::Copied: return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.Branched");
	case EDiffHelperFileStatus::Unmerged: return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.Conflicted");
#else
	case EDiffHelperFileStatus::Added: return FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Add");
	case EDiffHelperFileStatus::Modified: return FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Edit");
	case EDiffHelperFileStatus::Deleted: return FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Delete");
	case EDiffHelperFileStatus::Renamed:
	case EDiffHelperFileStatus::Copied: return FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Branch");
	case EDiffHelperFileStatus::Unmerged: return FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Integrate");
#endif
	default: return FSlateIcon();
	}
}

TOptional<FString> UDiffHelperGitManager::GetFile(const FString& InFilename, const FString& InRevision) const
{
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_GetFile, FColor::Red);
	
	IFileManager::Get().MakeDirectory(*FPaths::DiffDir(), true);
	const FString TempFileName = FString::Printf(TEXT("%stemp-%s-%s"), *FPaths::DiffDir(), *InRevision, *FPaths::GetCleanFilename(InFilename));
	TOptional<FString> FilePath = FPaths::ConvertRelativePathToFull(TempFileName);

	// Diff against the revision
	const FString Parameter = FString::Printf(TEXT("%s:%s"), *InRevision, *InFilename);

	bool bCommandSuccessful;
	if(FPaths::FileExists(FilePath.GetValue()))
	{
		// if the temp file already exists, reuse it directly
		bCommandSuccessful = true;
	}
	else
	{
		const auto RepositoryRoot = GetRepositoryDirectory();
		// bCommandSuccessful = GitSourceControlUtils::RunDumpToFile(GitBinaryPath, RepositoryRoot, Parameter, FilePath);
		bCommandSuccessful = ExtractFile(Parameter, FilePath.GetValue());
	}
	
	return bCommandSuccessful ? FilePath : TOptional<FString>();
}

TMap<FString, FDiffHelperCommit> UDiffHelperGitManager::GetLastCommitForFiles(const TArray<FString>& InFilePaths, const FString& InBranch) const
{
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_GetLastCommitForFiles, FColor::Red);
	
	const FString Command = TEXT("log");
	FString Result;
	FString Errors;

	TArray<FString> Params;
	Params.Add(InBranch);
	Params.Add(TEXT("--pretty=format:\"<Hash:%h> <Message:%s> <Author:%an> <Date:%ad>\""));
	Params.Add(TEXT("--date=format-local:\"%d/%m/%Y %H:%M\""));
	Params.Add(TEXT("--name-status"));

	const auto FileList = Algo::Accumulate(InFilePaths, FString("-- "), [](FString Accumulator, const FString& FilePath)
	{
		Accumulator += FilePath;
		Accumulator += TEXT(" ");
		return Accumulator;
	});

	Params.Add(FileList);

	if (!ExecuteCommand(Command, Params, {}, Result, Errors))
	{
		UE_LOG(LogDiffHelper, Error, TEXT("Failed to get last commit for files: %s"), *Errors);
		return {};
	}

	TMap<FString, FDiffHelperCommit> LastCommits;
	TArray<FDiffHelperCommit> Commits = ParseCommits(Result);
	for (const auto& Commit : Commits)
	{
		for (const auto& File : Commit.Files)
		{
			if (LastCommits.Contains(File.Path))
			{
				continue;
			}

			LastCommits.Add(File.Path, Commit);
		}
	}
	
	return LastCommits;
}

void UDiffHelperGitManager::LoadGitBinaryPath()
{
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_LoadGitBinaryPath, FColor::Red);
	
	FScopeLock ScopeLock(&CriticalSection);

	static const FString SettingsSection = TEXT("GitSourceControl.GitSourceControlSettings");
	const auto& IniFile = SourceControlHelpers::GetSettingsIni();
	const auto Result = GConfig->GetString(*SettingsSection, TEXT("BinaryPath"), GitBinaryPath, IniFile);
	
	if (Result)
	{
		UDiffHelperUtils::AddErrorNotification(LOCTEXT("GitBinaryPathError", "Couldn't obtain git binary path! Check your Revision Control settings!"));
	}
}

TOptional<FString> UDiffHelperGitManager::GetRepositoryDirectory() const
{
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3
	const auto& Provider = ISourceControlModule::Get().GetProvider();
	const auto Status = Provider.GetStatus();
	return Status.Contains(ISourceControlProvider::EStatus::Repository) ? TOptional<FString>(Status[ISourceControlProvider::EStatus::Repository]) : TOptional<FString>();
#else
	// Repository root is inaccessible in 5.2 and below
	return TOptional<FString>(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()));
#endif
}

bool UDiffHelperGitManager::ExecuteCommand(const FString& InCommand, const TArray<FString>& InParameters, const TArray<FString>& InFiles, FString& OutResults, FString& OutErrors) const
{
	SCOPED_NAMED_EVENT_F(TEXT("UDiffHelperGitManager_ExecuteCommand: %s"), FColor::Red, *InCommand);
	
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
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_GetForkPoint, FColor::Red);
	
	const auto Command = FString::Printf(TEXT("rev-parse --short \"$(git rev-list %s ^%s | tail -n 1)^\""), *InSourceBranch.Name, *InTargetBranch.Name);
	FString Result;
	FString Errors;

	if (!ExecuteCommand(Command, {}, {}, Result, Errors))
	{
		UE_LOG(LogDiffHelper, Error, TEXT("Failed to get fork point: %s"), *Errors);
		return TOptional<FString>();
	}

	return TOptional(Result.TrimStartAndEnd());
}

TMap<FString, EDiffHelperFileStatus> UDiffHelperGitManager::GetStatuses(const FString& InSourceRevision, const FString& InTargetRevision) const
{
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_GetStatuses, FColor::Red);
	
	const auto Command = TEXT("diff");
	FString Result;
	FString Errors;

	TArray<FString> Params;
	Params.Add(TEXT("--name-status"));
	Params.Add(InTargetRevision + TEXT("..") + InSourceRevision);

	if (!ExecuteCommand(Command, Params, {}, Result, Errors))
	{
		UE_LOG(LogDiffHelper, Error, TEXT("Failed to get statuses: %s"), *Errors);
		return {};
	}

	const auto* Settings = GetDefault<UDiffHelperSettings>();
	const auto Pattern = FRegexPattern(Settings->ChangedFilePattern);
	auto Matcher = FRegexMatcher(Pattern, Result);

	TMap<FString, EDiffHelperFileStatus> Statuses;
	while (Matcher.FindNext())
	{
		const auto Status = Matcher.GetCaptureGroup(Settings->ChangedFileStatusGroup);
		const auto Path = Matcher.GetCaptureGroup(Settings->ChangedFilePathGroup);

		Statuses.Add(Path, ConvertFileStatus(Status));
	}

	return Statuses;
}

TArray<FDiffHelperBranch> UDiffHelperGitManager::ParseBranches(const FString& InBranches) const
{
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_ParseBranches, FColor::Red);
	
	const auto* DiffHelperSettings = GetDefault<UDiffHelperSettings>();
	const auto Pattern = FRegexPattern(DiffHelperSettings->BranchParserPattern);
	auto Matcher = FRegexMatcher(Pattern, InBranches);

	TArray<FDiffHelperBranch> Branches;
	while (Matcher.FindNext())
	{
		const auto BranchName = Matcher.GetCaptureGroup(DiffHelperSettings->BranchNameGroup);
		const auto BranchRevision = Matcher.GetCaptureGroup(DiffHelperSettings->BranchRevisionGroup);

#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3
		Branches.Emplace(BranchName, BranchRevision);
#else
		FDiffHelperBranch Branch;
		Branch.Name = BranchName;
		Branch.Revision = BranchRevision;
		Branches.Emplace(Branch);
#endif
	}

	return Branches;
}

TArray<FDiffHelperCommit> UDiffHelperGitManager::ParseCommits(const FString& InCommits) const
{
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_ParseCommits, FColor::Red);
	
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

FDiffHelperCommit UDiffHelperGitManager::ParseCommit(const FString& String) const
{
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_ParseCommit, FColor::Red);
	
	const auto* Settings = GetDefault<UDiffHelperSettings>();
	const auto Pattern = FRegexPattern(Settings->CommitDataPattern);
	auto Matcher = FRegexMatcher(Pattern, String);

	FDiffHelperCommit Commit;
	if (Matcher.FindNext())
	{
		Commit.Revision = Matcher.GetCaptureGroup(Settings->HashGroup);
		Commit.Message = Matcher.GetCaptureGroup(Settings->MessageGroup);
		Commit.Author = Matcher.GetCaptureGroup(Settings->AuthorGroup);
		Commit.Date = ParseDate(Matcher.GetCaptureGroup(Settings->DateGroup));

		const auto ChangedFiles = Matcher.GetCaptureGroup(Settings->ChangedFilesGroup);
		Commit.Files = ParseChangedFiles(ChangedFiles);
	}

	return Commit;
}

FDateTime UDiffHelperGitManager::ParseDate(const FString& InDate) const
{
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_ParseDate, FColor::Red);
	
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
	SCOPED_NAMED_EVENT(UDiffHelperGitManager_ParseChangedFiles, FColor::Red);
	
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

bool UDiffHelperGitManager::ExtractFile(const FString& InParameter, const FString& InDumpFileName) const
{
	SCOPED_NAMED_EVENT_F(TEXT("UDiffHelperGitManager_ExtractFile: %s"), FColor::Red, *InParameter);
	
	// Modified copy of GitSourceControlUtils::RunDumpToFile
	// TODO: DIFF-24 - cleanup this method
	int32 ReturnCode = -1;
	FString FullCommand;

	const auto RepositoryRoot = GetRepositoryDirectory();
	if (!ensure(RepositoryRoot.IsSet()))
	{
		return false;
	}
	
	// FGitSourceControlModule& GitSourceControl = FModuleManager::LoadModuleChecked<FGitSourceControlModule>("GitSourceControl");
	// const FGitVersion& GitVersion = GitSourceControl.GetProvider().GetGitVersion();

	if(!RepositoryRoot->IsEmpty())
	{
		// Specify the working copy (the root) of the git repository (before the command itself)
		FullCommand  = TEXT("-C \"");
		FullCommand += *RepositoryRoot;
		FullCommand += TEXT("\" ");
	}

	// then the git command itself
	// if(GitVersion.bHasCatFileWithFilters)
	// {
		// Newer versions (2.9.3.windows.2) support smudge/clean filters used by Git LFS, git-fat, git-annex, etc
		FullCommand += TEXT("cat-file --filters ");
	// }
	// else
	// {
		// Previous versions fall-back on "git show" like before
		// FullCommand += TEXT("show ");
	// }

	// Append to the command the parameter
	FullCommand += InParameter;

	const bool bLaunchDetached = false;
	const bool bLaunchHidden = true;
	const bool bLaunchReallyHidden = bLaunchHidden;

	void* PipeRead = nullptr;
	void* PipeWrite = nullptr;

	verify(FPlatformProcess::CreatePipe(PipeRead, PipeWrite));

	FProcHandle ProcessHandle = FPlatformProcess::CreateProc(*GitBinaryPath, *FullCommand, bLaunchDetached, bLaunchHidden, bLaunchReallyHidden, nullptr, 0, **RepositoryRoot, PipeWrite, nullptr, nullptr);
	if(ProcessHandle.IsValid())
	{
		FPlatformProcess::Sleep(0.01);

		TArray<uint8> BinaryFileContent;
		while(FPlatformProcess::IsProcRunning(ProcessHandle))
		{
			TArray<uint8> BinaryData;
			FPlatformProcess::ReadPipeToArray(PipeRead, BinaryData);
			if(BinaryData.Num() > 0)
			{
				BinaryFileContent.Append(MoveTemp(BinaryData));
			}
		}
		TArray<uint8> BinaryData;
		FPlatformProcess::ReadPipeToArray(PipeRead, BinaryData);
		if(BinaryData.Num() > 0)
		{
			BinaryFileContent.Append(MoveTemp(BinaryData));
		}

		FPlatformProcess::GetProcReturnCode(ProcessHandle, &ReturnCode);
		if(ReturnCode == 0)
		{
			// Save buffer into temp file
			if(FFileHelper::SaveArrayToFile(BinaryFileContent, *InDumpFileName))
			{
				UE_LOG(LogDiffHelper, Log, TEXT("Writed '%s' (%do)"), *InDumpFileName, BinaryFileContent.Num());
			}
			else
			{
				UE_LOG(LogDiffHelper, Error, TEXT("Could not write %s"), *InDumpFileName);
				ReturnCode = -1;
			}
		}
		else
		{
			UE_LOG(LogDiffHelper, Error, TEXT("DumpToFile: ReturnCode=%d"), ReturnCode);
		}

		FPlatformProcess::CloseProc(ProcessHandle);
	}
	else
	{
		UE_LOG(LogDiffHelper, Error, TEXT("Failed to launch 'git cat-file'"));
	}

	FPlatformProcess::ClosePipe(PipeRead, PipeWrite);

	return (ReturnCode == 0);
}

#undef LOCTEXT_NAMESPACE
