// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDiffHelperSimpleDelegate);
DECLARE_LOG_CATEGORY_EXTERN(LogDiffHelper, Log, All);

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

	operator FString() const { return Name; }

	FORCEINLINE bool IsValid() const { return !Name.IsEmpty(); }
	FORCEINLINE bool operator==(const FDiffHelperBranch& Other) const { return Name == Other.Name && Revision == Other.Revision; }
	FORCEINLINE bool operator!=(const FDiffHelperBranch& Other) const { return !(*this == Other); }
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
