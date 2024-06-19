// Copyright Epic Games, Inc. All Rights Reserved.

#include "DiffHelperStyle.h"
#include "DiffHelper.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FDiffHelperStyle::StyleInstance = nullptr;

void FDiffHelperStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FDiffHelperStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FDiffHelperStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("DiffHelperStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FDiffHelperStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("DiffHelperStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("DiffHelper")->GetBaseDir() / TEXT("Resources"));

	Style->Set("DiffHelper.Diff", new IMAGE_BRUSH_SVG(TEXT("DiffIcon"), Icon20x20));
	Style->Set("DiffHelper.Directory", new IMAGE_BRUSH_SVG(TEXT("DirectoryIcon"), Icon20x20));
	Style->Set("DiffHelper.Directory.Small", new IMAGE_BRUSH_SVG(TEXT("DirectoryIcon"), Icon16x16));

	// Arrows
	Style->Set("DiffHelper.ArrowUp", new IMAGE_BRUSH_SVG(TEXT("ArrowUp"), Icon16x16));
	Style->Set("DiffHelper.ArrowDown", new IMAGE_BRUSH_SVG(TEXT("ArrowDown"), Icon16x16));
	Style->Set("DiffHelper.DoubleArrowUp", new IMAGE_BRUSH_SVG(TEXT("DoubleArrowUp"), Icon16x16));
	Style->Set("DiffHelper.DoubleArrowDown", new IMAGE_BRUSH_SVG(TEXT("DoubleArrowDown"), Icon16x16));
	Style->Set("DiffHelper.DoubleHeadedArrow", new IMAGE_BRUSH_SVG(TEXT("DoubleHeadedArrow"), Icon16x16));
	return Style;
}

void FDiffHelperStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FDiffHelperStyle::Get()
{
	return *StyleInstance;
}
