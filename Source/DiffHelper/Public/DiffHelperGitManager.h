// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>
#include "DiffHelperManager.h"
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
	virtual FDiffHelperBranch GetCurrentBranch() const override;

	UFUNCTION()
	virtual TArray<FDiffHelperBranch> GetBranches() const override;

	UFUNCTION()
	virtual TArray<FDiffHelperDiffItem> GetDiff(const FString& InSourceRevision, const FString& InTargetRevision) const override;
	
	UFUNCTION()
	virtual TArray<FDiffHelperCommit> GetDiffCommitsList(const FDiffHelperBranch& InSourceBranch, const FDiffHelperBranch& InTargetBranch) const override;
#pragma endregion IDiffHelperManager

protected:
	UFUNCTION()
	void LoadGitBinaryPath();

	UFUNCTION()
	bool ExecuteCommand(const FString& InCommand, const TArray<FString>& InParameters, const TArray<FString>& InFiles, FString& OutResults, FString& OutErrors);
};
