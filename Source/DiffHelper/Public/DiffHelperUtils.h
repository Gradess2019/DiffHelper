// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DiffHelperUtils.generated.h"

UCLASS()
class DIFFHELPER_API UDiffHelperUtils : public UObject
{
	GENERATED_BODY()

public:
	template<typename T>
	static TArray<TSharedPtr<T>> ConvertToShared(const TArray<T>& InArray)
	{
		TArray<TSharedPtr<T>> OutArray;
		OutArray.Reserve(InArray.Num());
		for (const auto& Item : InArray)
		{
			OutArray.Add(MakeShared<T>(Item));
		}
		
		return OutArray;
	}

	UFUNCTION(BlueprintPure, Category = "DiffHelper|Utils")
	static TArray<FString> ConvertBranchesToStringArray(const TArray<FDiffHelperBranch>& InBranches);
};
