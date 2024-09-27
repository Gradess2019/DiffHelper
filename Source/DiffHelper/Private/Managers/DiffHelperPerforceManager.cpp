// Copyright 2024 Gradess Games. All Rights Reserved.


#include "Managers/DiffHelperPerforceManager.h"

#include "DiffHelperSettings.h"
#include "DiffHelperTypes.h"
#include "DiffHelperUtils.h"
#include "EditorAssetLibrary.h"

#include "Algo/ForEach.h"

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
	// ===========================
	// TODO: REFACTOR THIS METHOD. Also we got crush we are trying to diff asset.
	// ===========================
	
	TSet<FString> DepotFiles;
	TMap<FString, TArray<FDiffHelperCommit>> FileCommits;
	TMap<FString, EDiffHelperFileStatus> FileStatuses;
	TArray<FDiffHelperDiffItem> DiffItems;
	TMap<FString, FString> OnDiskLocations;


	// Step 1: Obtaining diff file list
	// p4 -ztag diff2 -q ...@4 ...@7
	{
		const FString Command = TEXT("-ztag -F \"Depot1=%depotFile%;Depot2=%depotFile2%;Status='%status%'\" diff2");
		FString Result;
		FString Errors;

		TArray<FString> Params;
		Params.Add("-q");
		Params.Add(FString::Printf(TEXT("...@%s ...@%s"), *InTargetRevision, *InSourceRevision));

		if (!ExecuteCommand(Command, Params, {}, Result, Errors))
		{
			return {};
		}

		const auto* DiffHelperSettings = GetDefault<UDiffHelperSettings>();
		const auto Pattern = FRegexPattern(DiffHelperSettings->ChangelistDiffPattern);
		auto Matcher = FRegexMatcher(Pattern, Result);

		while (Matcher.FindNext())
		{
			const auto Depot1 = Matcher.GetCaptureGroup(DiffHelperSettings->ChangelistDiffDepot1Group);
			const auto Depot2 = Matcher.GetCaptureGroup(DiffHelperSettings->ChangelistDiffDepot2Group);
			const auto Status = Matcher.GetCaptureGroup(DiffHelperSettings->ChangelistDiffStatusGroup);

			if (!Depot1.IsEmpty())
			{
				DepotFiles.Add(Depot1);
				FileStatuses.Add(Depot1, ConvertDiffFileStatus(Status));
			}
			else
			{
				DepotFiles.Add(Depot2);
				FileStatuses.Add(Depot2, ConvertDiffFileStatus(Status));
			}
		}

		OnDiskLocations = GetFileLocations(DepotFiles);
	}

	// Step 2: Obtaining file commits
	{
		// call p4 filelog with for all depot files
		const FString Command = TEXT("filelog");
		FString Result;
		FString Errors;

		if (!ExecuteCommand(Command, DepotFiles.Array(), {}, Result, Errors))
		{
			return {};
		}

		const auto* DiffHelperSettings = GetDefault<UDiffHelperSettings>();
		const auto FileBlockPattern = FRegexPattern(DiffHelperSettings->FilelogBlockPattern);
		auto FileBlockMatcher = FRegexMatcher(FileBlockPattern, Result);

		const auto FilenamePattern = FRegexPattern(DiffHelperSettings->FilelogFilePattern);
		const auto ChangePattern = FRegexPattern(DiffHelperSettings->FilelogChangePattern);

		while (FileBlockMatcher.FindNext())
		{
			auto FileBlock = FileBlockMatcher.GetCaptureGroup(0);
			auto FilenameMatcher = FRegexMatcher(FilenamePattern, FileBlock);

			if (FilenameMatcher.FindNext())
			{
				const auto Filename = FilenameMatcher.GetCaptureGroup(DiffHelperSettings->FilelogFileGroup);
				auto ChangeMatcher = FRegexMatcher(ChangePattern, FileBlock);

				while (ChangeMatcher.FindNext())
				{
					const auto Change = ChangeMatcher.GetCaptureGroup(DiffHelperSettings->FilelogChangeGroup);
					const auto Author = ChangeMatcher.GetCaptureGroup(DiffHelperSettings->FilelogUserGroup);
					const auto Date = ChangeMatcher.GetCaptureGroup(DiffHelperSettings->FilelogDateGroup);
					const auto Message = ChangeMatcher.GetCaptureGroup(DiffHelperSettings->FilelogDescriptionGroup);

					FDiffHelperCommit Commit;
					Commit.Revision = Change;
					Commit.Author = Author;
					Commit.Date = FDateTime::FromUnixTimestamp(FCString::Atoi(*Date));
					Commit.Message = Message;
					
					if (!FileCommits.Contains(Filename))
					{
						FileCommits.Add(Filename, TArray<FDiffHelperCommit>());
					}

					FileCommits[Filename].Add(Commit);
				}
			}
		}
	}

	// Step 3: Creating diff items
	{
		// for each file in DepotFiles
		for (const auto& DepotFile : DepotFiles)
		{
			FDiffHelperDiffItem NewDiffItem;
			NewDiffItem.Path = OnDiskLocations[DepotFile];
			NewDiffItem.Status = FileStatuses[DepotFile];
			NewDiffItem.Commits = FileCommits[DepotFile];
			if (ensure(NewDiffItem.Commits.Num() > 0))
			{
				NewDiffItem.LastTargetCommit = NewDiffItem.Commits[0].Revision == InTargetRevision ? NewDiffItem.Commits[0] : FDiffHelperCommit();
			}

			FPaths::NormalizeFilename(NewDiffItem.Path);
			FString ProjectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
			NewDiffItem.Path.ReplaceInline(*ProjectDir, TEXT(""));

			// for each Commits in NewDiffItem add FileData
			Algo::ForEach(NewDiffItem.Commits, [&NewDiffItem, &FileStatuses, &OnDiskLocations, &DepotFile] (FDiffHelperCommit& Commit)
			{
				FDiffHelperFileData FileData;
				FileData.Path = NewDiffItem.Path;
				FileData.Status = FileStatuses[DepotFile];
				Commit.Files.Add(FileData);
			});
			
			if (FPaths::IsUnderDirectory(OnDiskLocations[DepotFile], FPaths::ProjectContentDir()))
			{
				FString PackageName;
				if (FPackageName::TryConvertFilenameToLongPackageName(OnDiskLocations[DepotFile], PackageName) && UDiffHelperUtils::IsUnrealAsset(OnDiskLocations[DepotFile]))
				{
					const auto AssetData = UEditorAssetLibrary::FindAssetData(PackageName);
					if (AssetData.IsValid())
					{
						NewDiffItem.AssetData = AssetData;
					}
				}
			}

			DiffItems.Add(NewDiffItem);
		}
	}
	
	return DiffItems;
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

EDiffHelperFileStatus UDiffHelperPerforceManager::ConvertDiffFileStatus(const FString& InStatus) const
{
	if (InStatus.Equals(TEXT("left only"), ESearchCase::IgnoreCase)) { return EDiffHelperFileStatus::Deleted; }
	if (InStatus.Equals(TEXT("right only"), ESearchCase::IgnoreCase)) { return EDiffHelperFileStatus::Added; }
	if (InStatus.Equals(TEXT("content"), ESearchCase::IgnoreCase)) { return EDiffHelperFileStatus::Modified; }
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

	const auto ProjectPath = FPaths::ProjectDir();
	OSPaths = GetFileLocations(DepotPaths);
	for (auto& File : Commit.Files)
	{
		if (OSPaths.Contains(File.Path))
		{
			File.Path = OSPaths[File.Path].Replace(*ProjectPath, TEXT(""));
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