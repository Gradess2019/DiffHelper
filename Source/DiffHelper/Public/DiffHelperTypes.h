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
