// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.h"

#include "UObject/Object.h"
#include "DiffHelperCacheManager.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DIFFHELPER_API UDiffHelperCacheManager : public UObject
{
	GENERATED_BODY()

private:
	// Cached branches
	UPROPERTY()
	FDiffHelperBranch SourceBranch;

	UPROPERTY()
	FDiffHelperBranch TargetBranch;

public:
	UFUNCTION(BlueprintPure, Category = "Diff Helper")
	const FDiffHelperBranch& GetSourceBranch() const { return SourceBranch; }
	
	UFUNCTION(BlueprintPure, Category = "Diff Helper")
	const FDiffHelperBranch& GetTargetBranch() const { return TargetBranch; }

	UFUNCTION(BlueprintCallable, Category = "Diff Helper")
	void SetSourceBranch(const FDiffHelperBranch& Branch);

	UFUNCTION(BlueprintCallable, Category = "Diff Helper")
	void SetTargetBranch(const FDiffHelperBranch& Branch);

private:
	void Cache();
};
