// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.h"


class UDiffHelperTabController;

class DIFFHELPER_API SDiffHelperCommitItem : public SMultiColumnTableRow<TSharedPtr<FDiffHelperCommit>>
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperCommitItem)
			:
			_Controller(nullptr),
			_Item(nullptr)
		{
		}

		SLATE_ARGUMENT(TWeakObjectPtr<UDiffHelperTabController>, Controller)
		SLATE_ARGUMENT(TSharedPtr<FDiffHelperCommit>, Item)

	SLATE_END_ARGS()

protected:
	TWeakObjectPtr<UDiffHelperTabController> Controller;
	TSharedPtr<FDiffHelperCommit> Item;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable);

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;

protected:
	TSharedRef<SWidget> CreateCommitHashColumn() const;
	TSharedRef<SWidget> CreateMessageColumn() const;
	TSharedRef<SWidget> CreateAuthorColumn() const;
	TSharedRef<SWidget> CreateDateColumn() const;

	UE_DEPRECATED(5.4, "CreateDiffButtonColumn is deprecated")
	TSharedRef<SWidget> CreateDiffButtonColumn() const;

	UE_DEPRECATED(5.4, "OnDiffButtonClicked is deprecated")
	FReply OnDiffButtonClicked() const;
};
