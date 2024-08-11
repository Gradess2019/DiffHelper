// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SDiffHelperBaseCommitContextMenu.h"

#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class DIFFHELPER_API SDiffHelperDiffItemContextMenu : public SDiffHelperBaseCommitContextMenu
{
protected:
	virtual void BuildMenu(FMenuBuilder& InMenuBuilder) override;
};
