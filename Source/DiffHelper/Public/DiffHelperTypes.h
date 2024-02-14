// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.generated.h"

USTRUCT(BlueprintType)
struct FDiffHelperBranch
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString BranchName;

	UPROPERTY(BlueprintReadOnly)
	FString Revision;
};

USTRUCT(BlueprintType)
struct FDiffHelperCommit
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Revision;

	UPROPERTY(BlueprintReadOnly)
	FString Message;

	UPROPERTY(BlueprintReadOnly)
	FString Author;

	UPROPERTY(BlueprintReadOnly)
	FDateTime Date;

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> Files;
};

USTRUCT(BlueprintType)
struct FDiffHelperDiffItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<UObject> Asset;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDiffHelperCommit> Commits;
};
