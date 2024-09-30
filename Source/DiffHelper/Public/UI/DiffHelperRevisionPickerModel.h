// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.h"

#include "UObject/Object.h"
#include "DiffHelperRevisionPickerModel.generated.h"

UCLASS(BlueprintType)
class DIFFHELPER_API UDiffHelperRevisionPickerModel : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FDiffHelperBranch> Branches;

	UPROPERTY(BlueprintReadOnly)
	FDiffHelperBranch SourceBranch;

	UPROPERTY(BlueprintReadOnly)
	FDiffHelperBranch TargetBranch;
};
