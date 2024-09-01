// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperManager.h"

#include "UObject/Object.h"
#include "DiffHelperPerforceManager.generated.h"

/**
 * 
 */
UCLASS()
class DIFFHELPER_API UDiffHelperPerforceManager : public UObject, public IDiffHelperManager
{
	GENERATED_BODY()

public:
	//~ Begin IDiffHelperManager
	virtual bool Init() override;
	virtual void Deinit() override;
	virtual FDiffHelperBranch GetCurrentBranch() const override;
	virtual TArray<FDiffHelperBranch> GetBranches() const override;
	virtual TArray<FDiffHelperDiffItem> GetDiff(const FString& InSourceRevision, const FString& InTargetRevision) const override;
	virtual TArray<FDiffHelperCommit> GetDiffCommitsList(const FString& InSourceBranch, const FString& InTargetBranch) const override;
	virtual FDiffHelperCommit GetLastCommitForFile(const FString& InFilePath, const FString& InBranch) const override;
	virtual FSlateIcon GetStatusIcon(const EDiffHelperFileStatus InStatus) const override;
	virtual TOptional<FString> GetFile(const FString& InFilePath, const FString& InRevision) const override;
	//~ End IDiffHelperManager

private:
	bool ExecuteCommand(const FString& InCommand, const TArray<FString>& InParameters, const TArray<FString>& InFiles, FString& OutResults, FString& OutErrors) const;

	TArray<FDiffHelperBranch> ParseChanges(const FString& InChanges) const;
};
