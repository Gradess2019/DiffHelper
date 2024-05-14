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

public:
	static const FString ConfigSection;
	static const FString ConfigSourceBranchKey;
	static const FString ConfigTargetBranchKey;

private:
	// Cached branches
	UPROPERTY()
	FString SourceBranchName;

	UPROPERTY()
	FString TargetBranchName;

public:
	UFUNCTION(BlueprintPure, Category = "Diff Helper")
	const FString& GetSourceBranch() const { return SourceBranchName; }
	
	UFUNCTION(BlueprintPure, Category = "Diff Helper")
	const FString& GetTargetBranch() const { return TargetBranchName; }

	UFUNCTION(BlueprintCallable, Category = "Diff Helper")
	void SetSourceBranch(const FString& Branch);

	UFUNCTION(BlueprintCallable, Category = "Diff Helper")
	void SetTargetBranch(const FString& Branch);

	UFUNCTION()
	void Init();

private:
	void Cache();
	FString GetConfigPath() const;
};
