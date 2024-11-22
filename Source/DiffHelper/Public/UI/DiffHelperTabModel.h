// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.h"

#include "UObject/Object.h"
#include "DiffHelperTabModel.generated.h"

UCLASS(BlueprintType)
class DIFFHELPER_API UDiffHelperTabModel : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FDiffHelperSimpleDynamicDelegate OnModelUpdated;
	FDiffHelperSimpleDelegate OnModelUpdated_Raw;

	UPROPERTY(BlueprintReadOnly, Category = "Diff Helper")
	TArray<FDiffHelperDiffItem> Diff;

	UPROPERTY(BlueprintReadOnly, Category = "Diff Helper")
	FDiffHelperBranch SourceBranch;

	UPROPERTY(BlueprintReadOnly, Category = "Diff Helper")
	FDiffHelperBranch TargetBranch;

	UPROPERTY(BlueprintReadOnly, Category = "Diff Helper")
	FDiffHelperDiffItem SelectedDiffItem;

	UPROPERTY()
	FDiffHelperDiffPanelData DiffPanelData;
	
	UPROPERTY()
	FDiffHelperCommitPanelData CommitPanelData;
};
