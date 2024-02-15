// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>
#include "DiffHelperSettings.generated.h"

UCLASS(Config=DiffHelper)
class DIFFHELPER_API UDiffHelperSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Git")
	FString BranchParserPattern = TEXT("(?:\n*\\s\\s|(?:\\*\\s))(.+?)\\s+(\\w+)");

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 BranchNameGroup = 1;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 BranchRevisionGroup = 2;
};
