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
	FDiffHelperSimpleDelegate OnModelUpdated;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FDiffHelperBranch> Branches;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDiffHelperDiffItem> Diff;

	UPROPERTY(BlueprintReadOnly)
	FDiffHelperBranch SourceBranch;

	UPROPERTY(BlueprintReadOnly)
	FDiffHelperBranch TargetBranch;

	UPROPERTY(BlueprintReadOnly)
	FDiffHelperDiffItem SelectedDiffItem;
};
