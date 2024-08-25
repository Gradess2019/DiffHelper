// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SDiffHelperBaseCommitContextMenu.h"

class UDiffHelperTabController;

class DIFFHELPER_API SDiffHelperCommitContextMenu : public SDiffHelperBaseCommitContextMenu
{
protected:
	virtual void BuildMenu(FMenuBuilder& InMenuBuilder) override;
	virtual TSharedPtr<FUICommandList> GetCommandList(const TWeakObjectPtr<UDiffHelperTabController>& InController) override;
};
