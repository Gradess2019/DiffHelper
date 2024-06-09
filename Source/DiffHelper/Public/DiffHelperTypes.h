// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/TextFilter.h"
#include "DiffHelperTypes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDiffHelperSimpleDynamicDelegate);
DECLARE_MULTICAST_DELEGATE(FDiffHelperSimpleDelegate);
DECLARE_DELEGATE(FDiffHelperEvent)

DECLARE_LOG_CATEGORY_EXTERN(LogDiffHelper, Log, All);

namespace SDiffHelperDiffPanelConstants
{
	const FName StatusColumnId(TEXT("State"));
	const FName PathColumnId(TEXT("Path"));

	const int32 ListWidgetIndex = 0;
	const int32 TreeWidgetIndex = 1;
}

namespace SDiffHelperCommitPanelConstants
{
	const FName HashColumnId(TEXT("CommitHash"));
	const FName MessageColumnId(TEXT("CommitMessage"));
	const FName AuthorColumnId(TEXT("CommitAuthor"));
	const FName DateColumnId(TEXT("CommitDate"));
	const FName DiffButtonColumnId(TEXT("DiffButton"));
}


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

	FORCEINLINE bool IsValid() const { return !Revision.IsEmpty(); }
};

USTRUCT(BlueprintType)
struct FDiffHelperDiffItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Path;

	UPROPERTY(BlueprintReadOnly)
	EDiffHelperFileStatus Status = EDiffHelperFileStatus::None;
	
	UPROPERTY(BlueprintReadOnly)
	FAssetData AssetData;

	UPROPERTY(BlueprintReadOnly)
	FDiffHelperCommit LastTargetCommit;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDiffHelperCommit> Commits;

	FORCEINLINE bool IsValid() const { return !Path.IsEmpty(); }
};

USTRUCT()
struct FDiffHelperItemNode
{
	GENERATED_BODY()

	UPROPERTY()
	FString Path;

	UPROPERTY()
	FString Name;

	TSharedPtr<FDiffHelperDiffItem> DiffItem;
	TArray<TSharedPtr<FDiffHelperItemNode>> Children;
};

USTRUCT()
struct FDiffHelperDiffPanelData
{
	GENERATED_BODY()

	int32 CurrentWidgetIndex = 0;
	
	TSharedPtr<TTextFilter<const FDiffHelperDiffItem&>> SearchFilter = nullptr;
	
	TArray<TSharedPtr<FDiffHelperItemNode>> OriginalDiff;
	TArray<TSharedPtr<FDiffHelperItemNode>> FilteredDiff;
	
	TArray<TSharedPtr<FDiffHelperItemNode>> TreeDiff;

	UE_DEPRECATED(5.4, "SortColumn will be removed")
	FName SortColumn = SDiffHelperDiffPanelConstants::PathColumnId;
	EColumnSortMode::Type SortMode = EColumnSortMode::Ascending;
};