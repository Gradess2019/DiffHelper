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
	void LoadCachedBranches();

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
	void CollectDiff();

	UFUNCTION(BlueprintCallable)
	void DiffAsset(const FString& InPath, const FDiffHelperCommit& InFirstRevision, const FDiffHelperCommit& InSecondRevision) const;

	UFUNCTION(BlueprintCallable)
	void CallModelUpdated() const;

public:
	void SetSearchFilter(const FText& InText) const;
	void SetSortingMode(const FName& InColumnId, EColumnSortMode::Type InSortMode) const;
	void SetActiveWidgetIndex(const int32& InIndex) const;

	void SetSelectedCommits(const TArray<TSharedPtr<FDiffHelperCommit>>& InCommits) const;

	void BindCommitPanelCommands();

	FDiffHelperSimpleDelegate& OnModelUpdated() const;

	void ExecuteDiff(const TArray<TSharedPtr<FDiffHelperCommit>>& InCommits, const FString& InPath) const;
	void DiffAgainstTarget();
	void DiffSelectedCommits();
	void DiffSelectedCommitAgainstNext();
	void DiffSelectedCommitAgainstPrevious();
	void DiffSelectedCommitAgainstNewest();
	void DiffSelectedCommitAgainstOldest();
	
	bool CanDiffSelectedCommits();
	bool CanDiffSelectedCommitAgainstNext();
	bool CanDiffSelectedCommitAgainstPrevious();

	int32 GetCommitIndex(const FDiffHelperCommit& InCommit) const;

private:
	void OnFilterChanged();
	void PopulateFilterSearchString(const FDiffHelperDiffItem& InItem, TArray<FString>& OutStrings) const;
};
