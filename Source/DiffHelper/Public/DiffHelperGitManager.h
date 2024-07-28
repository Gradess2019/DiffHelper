// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>
#include "DiffHelperManager.h"
#include "DiffHelperTypes.h"
#include "ISourceControlProvider.h"
#include "DiffHelperGitManager.generated.h"

UCLASS()
class DIFFHELPER_API UDiffHelperGitManager : public UObject, public IDiffHelperManager
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	FString GitBinaryPath;

	mutable FCriticalSection CriticalSection;

public:
#pragma region IDiffHelperManager
	UFUNCTION()
	virtual bool Init() override;

	UFUNCTION()
	virtual void Deinit() override;

	UFUNCTION()
	virtual FDiffHelperBranch GetCurrentBranch() const override;

	UFUNCTION()
	virtual TArray<FDiffHelperBranch> GetBranches() const override;

	UFUNCTION()
	virtual TArray<FDiffHelperDiffItem> GetDiff(const FString& InSourceRevision, const FString& InTargetRevision) const override;

	UFUNCTION()
	virtual TArray<FDiffHelperCommit> GetDiffCommitsList(const FString& InSourceBranch, const FString& InTargetBranch) const override;

	UFUNCTION()
	virtual FDiffHelperCommit GetLastCommitForFile(const FString& InFilePath, const FString& InBranch) const override;

	virtual FSlateIcon GetStatusIcon(const EDiffHelperFileStatus InStatus) const override;
	virtual TOptional<FString> GetFile(const FString& InFilename, const FString& InRevision) const override;
#pragma endregion IDiffHelperManager

protected:
	UFUNCTION()
	void LoadGitBinaryPath();

	TOptional<FString> GetRepositoryDirectory() const;

	bool ExecuteCommand(const FString& InCommand, const TArray<FString>& InParameters, const TArray<FString>& InFiles, FString& OutResults, FString& OutErrors) const;
	TOptional<FString> GetForkPoint(const FDiffHelperBranch& InSourceBranch, const FDiffHelperBranch& InTargetBranch) const;
	TMap<FString, EDiffHelperFileStatus> GetStatuses(const FString& InSourceRevision, const FString& InTargetRevision) const;

	TArray<FDiffHelperBranch> ParseBranches(const FString& InBranches) const;
	TArray<FDiffHelperCommit> ParseCommits(const FString& InCommits) const;
	FDiffHelperCommit ParseCommit(const FString& String) const;
	FDateTime ParseDate(const FString& InDate) const;
	TArray<FDiffHelperFileData> ParseChangedFiles(const FString& InFiles) const;

	EDiffHelperFileStatus ConvertFileStatus(const FString& InStatus) const;

	// Modified copy of GitSourceControlUtils::RunDumpToFile
	bool ExtractFile(const FString& InParameter, const FString& InDumpFileName) const;
};
