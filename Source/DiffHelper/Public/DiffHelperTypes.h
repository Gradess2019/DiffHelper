﻿// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.generated.h"

UENUM()
enum class EDiffHelperFileStatus : uint8
{
	None,
	Added,
	Modified,
	Deleted,
	Renamed,
	Copied,
	Unmerged
};

USTRUCT(BlueprintType)
struct FDiffHelperBranch
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString Revision;
};

USTRUCT(BlueprintType)
struct FDiffHelperFileData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Path;

	UPROPERTY(BlueprintReadOnly)
	EDiffHelperFileStatus Status = EDiffHelperFileStatus::None;
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
	TArray<FDiffHelperFileData> Files;
};

USTRUCT(BlueprintType)
struct FDiffHelperDiffItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Path;

	UPROPERTY(BlueprintReadOnly)
	FAssetData AssetData;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDiffHelperCommit> Commits;
};
