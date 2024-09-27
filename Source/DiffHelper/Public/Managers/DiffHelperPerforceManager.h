// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperManager.h"
#include "DiffHelperTypes.h"

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

	UE_DEPRECATED(5.4, "GetCurrentBranch is not used in Perforce")
	virtual FDiffHelperBranch GetCurrentBranch() const override;
	
	virtual TArray<FDiffHelperBranch> GetBranches() const override;
	virtual TArray<FDiffHelperDiffItem> GetDiff(const FString& InSourceRevision, const FString& InTargetRevision) const override;

	UE_DEPRECATED(5.4, "GetDiffCommitsList is not used in Perforce")
	virtual TArray<FDiffHelperCommit> GetDiffCommitsList(const FString& InSourceBranch, const FString& InTargetBranch) const override;

	UE_DEPRECATED(5.4, "GetLastCommitForFile is not used in Perforce")
	virtual FDiffHelperCommit GetLastCommitForFile(const FString& InFilePath, const FString& InBranch) const override;
	virtual FSlateIcon GetStatusIcon(const EDiffHelperFileStatus InStatus) const override;
	virtual TOptional<FString> GetFile(const FString& InFilePath, const FString& InRevision) const override;
	//~ End IDiffHelperManager

private:
	TMap<FString, FString> GetFileLocations(const TSet<FString>& InDepotFiles) const;
	
	bool ExecuteCommand(const FString& InCommand, const TArray<FString>& InParameters, const TArray<FString>& InFiles, FString& OutResults, FString& OutErrors) const;
	EDiffHelperFileStatus ConvertFileStatus(const FString& InStatus) const;
	EDiffHelperFileStatus ConvertDiffFileStatus(const FString& InStatus) const;

	TArray<FDiffHelperBranch> ParseRevisions(const FString& InChanges) const;
	FDiffHelperCommit ParseChanges(const FString& InRevision) const;
	TMap<FString, FString> ParseFileLocations(const FString& InLocations) const;

};
