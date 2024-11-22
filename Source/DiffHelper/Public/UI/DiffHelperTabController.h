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

public:
	DECLARE_MULTICAST_DELEGATE(FDiffHelperSimpleDelegate);
	FDiffHelperSimpleDelegate OnModelReset;

protected:
	UPROPERTY(BlueprintGetter="GetModel", Category="Diff Helper")
	TObjectPtr<UDiffHelperTabModel> Model;

	TSharedPtr<FUICommandList> MenuCommands;
	TSharedPtr<FUICommandList> DiffPanelCommands;
	TSharedPtr<FUICommandList> CommitPanelCommands;

public:	
	UFUNCTION()
	virtual void Init();
	
	UFUNCTION()
	virtual void Reset();

	UFUNCTION()
	virtual void Deinit();

	// TODO: do we really need to expose it to blueprint?
	UFUNCTION(BlueprintGetter)
	const UDiffHelperTabModel* GetModel() const { return Model.Get(); }

	UFUNCTION(BlueprintCallable, Category="Diff Helper")
	void SetSourceBranch(const FDiffHelperBranch& InBranch);

	UFUNCTION(BlueprintCallable, Category="Diff Helper")
	void SetTargetBranch(const FDiffHelperBranch& InBranch);

	UFUNCTION(BlueprintCallable, Category="Diff Helper")
	void SelectDiffItem(const FDiffHelperDiffItem& InDiffItem);

	UFUNCTION(BlueprintCallable, Category="Diff Helper")
	void CollectDiff();

	UFUNCTION(BlueprintCallable, Category="Diff Helper")
	void DiffAsset(const FString& InPath, const FDiffHelperCommit& InFirstRevision, const FDiffHelperCommit& InSecondRevision) const;

	UFUNCTION(BlueprintCallable, Category="Diff Helper")
	void CallModelUpdated() const;

public:
	// Unfortunately we can't return them as const TSharedPtr<FUICommandList>, because menu's arguments doesn't use const
	TSharedPtr<FUICommandList> GetMenuCommands() const { return MenuCommands; }
	TSharedPtr<FUICommandList> GetDiffPanelCommands() const { return DiffPanelCommands; }
	TSharedPtr<FUICommandList> GetCommitPanelCommands() const { return CommitPanelCommands; }
	
	void UpdateItemsData();
	
	void SetSearchFilter(const FText& InText) const;
	void SetSortingMode(const FName& InColumnId, EColumnSortMode::Type InSortMode) const;
	void SetActiveWidgetIndex(const int32& InIndex) const;

	void SetSelectedCommits(const TArray<TSharedPtr<FDiffHelperCommit>>& InCommits) const;
	void SelectNode(const TSharedPtr<FDiffHelperItemNode>& InNode) const;

	FDiffHelperSimpleDelegate& OnModelUpdated() const;
	FDiffHelperSimpleDelegate& OnPreWidgetIndexChanged() const;
	FDiffHelperSimpleDelegate& OnTreeDiffExpansionUpdated() const;

private:
	int32 GetCommitIndex(const FDiffHelperCommit& InCommit) const;

	void InitModel();
	
	void BindMenuCommands();
	void BindDiffPanelCommands();
	void BindCommitPanelCommands();
	
	void ToggleGroupByDirectory();
	void ExpandAll();
	void CollapseAll();
	void OpenLocation();
	void OpenAsset();
	void ShowInContentBrowser();
	
	bool IsTreeView();
	bool CanOpenLocation();
	bool CanOpenAsset();
	bool CanShowInContentBrowser();

	void ExecuteDiff(const TArray<TSharedPtr<FDiffHelperCommit>>& InCommits, const FString& InPath) const;
	void DiffAgainstTarget();
	void DiffSelectedCommits();
	void DiffSelectedCommitAgainstNext();
	void DiffSelectedCommitAgainstPrevious();
	void DiffSelectedCommitAgainstNewest();
	void DiffSelectedCommitAgainstOldest();

	bool CanDiffAgainstTarget();
	bool CanDiffSelectedCommits();
	bool CanDiffSelectedCommitAgainstNext();
	bool CanDiffSelectedCommitAgainstPrevious();
	
	void PopulateFilterSearchString(const FDiffHelperDiffItem& InItem, TArray<FString>& OutStrings) const;
};
