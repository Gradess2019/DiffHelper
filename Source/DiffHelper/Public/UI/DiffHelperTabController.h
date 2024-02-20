// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.h"

#include "UObject/Object.h"
#include "DiffHelperTabController.generated.h"

class UDiffHelperTabModel;

UCLASS(BlueprintType)
class DIFFHELPER_API UDiffHelperTabController : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintGetter="GetModel")
	TObjectPtr<UDiffHelperTabModel> Model;

public:
	UFUNCTION()
	void Init();

	UFUNCTION()
	void Deinit();

	// TODO: do we really need to expose it to blueprint?
	UFUNCTION(BlueprintGetter)
	const UDiffHelperTabModel* GetModel() const { return Model.Get(); }

	UFUNCTION(BlueprintCallable)
	void SetSourceBranch(const FDiffHelperBranch& InBranch);

	UFUNCTION(BlueprintCallable)
	void SetTargetBranch(const FDiffHelperBranch& InBranch);

	UFUNCTION(BlueprintCallable)
	void SelectDiffItem(const FDiffHelperDiffItem& InDiffItem);

	UFUNCTION(BlueprintCallable)
	void CollectDiff() const;

	UFUNCTION(BlueprintCallable)
	void CallModelUpdated() const;

	FDiffHelperSimpleDelegate& OnModelUpdated() const;
};
