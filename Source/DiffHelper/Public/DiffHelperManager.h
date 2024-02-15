// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DiffHelperManager.generated.h"

struct FDiffHelperBranch;
UINTERFACE(NotBlueprintable)
class UDiffHelperManager : public UInterface
{
	GENERATED_BODY()
};

class DIFFHELPER_API IDiffHelperManager
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual bool Init() = 0;

	UFUNCTION()
	virtual void Deinit() = 0;
	
	UFUNCTION(BlueprintCallable, Category = "DiffHelperManager")
	virtual FDiffHelperBranch GetCurrentBranch() const = 0;

	UFUNCTION(BlueprintCallable, Category = "DiffHelperManager")
	virtual TArray<FDiffHelperBranch> GetBranches() const = 0;

	UFUNCTION(BlueprintCallable, Category = "DiffHelperManager")
	virtual TArray<FDiffHelperDiffItem> GetDiff(const FString& InSourceRevision, const FString& InTargetRevision) const = 0;
	
	UFUNCTION(BlueprintCallable, Category = "DiffHelperManager")
	virtual TArray<FDiffHelperCommit> GetDiffCommitsList(const FString& InSourceBranch, const FString& InTargetBranch) const = 0;
};
