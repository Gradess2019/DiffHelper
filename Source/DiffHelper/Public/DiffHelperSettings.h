// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>
#include "DiffHelperTypes.h"
#include "DiffHelperSettings.generated.h"

UCLASS(Config=DiffHelper)
class DIFFHELPER_API UDiffHelperSettings : public UObject
{
	GENERATED_BODY()

public:
	/** If true, the plugin will try to find and reuse opened diff tab. If false, it will always open a new tab */
	UPROPERTY(Config, EditAnywhere, Category = "General")
	bool bReuseDiffTab = true;

	/** if true the plugin will cache source and target branches and will put them into selectors on a first Diff Helper view */
	UPROPERTY(Config, EditAnywhere, Category = "General")
	bool bEnableCaching = true;
	
	/** Enables opening external diff for non-assets */
	UPROPERTY(Config, EditAnywhere, Category = "General")
	bool bEnableExternalDiff = false;

	/** Command that will be used to open diff in external editor for non-assets. By default, it's set to Visual Studio Code
	* {0} - source file path
	* {1} - target file path
	* {2} - source file revision
	* {3} - target file revision
	*/
	UPROPERTY(Config, EditAnywhere, Category = "General", meta = (EditCondition = "bEnableExternalDiff"))
	FString ExternalDiffCommand = TEXT("code --diff {0} {1}");
	
	/** Files with these statuses couldn't be used for diff */
	UPROPERTY(Config, EditAnywhere, Category = "General")
	TArray<EDiffHelperFileStatus> StatusBlacklist = {
		EDiffHelperFileStatus::None,
		EDiffHelperFileStatus::Deleted,
		EDiffHelperFileStatus::Unmerged,
	};

	UPROPERTY(Config, EditAnywhere, Category = "Appearance|Revision Picker")
	float PickerPanelWidth = 350.f;
	
	UPROPERTY(Config, EditAnywhere, Category = "Appearance|Diff Viewer")
	float DiffPanelMinSize = 300.f;

	UPROPERTY(Config, EditAnywhere, Category = "Appearance|Diff Viewer")
	float CommitPanelMinSize = 200.f;
	
	UPROPERTY(Config, EditAnywhere, Category = "Appearance|Diff Viewer", meta = (ClampMin = "0.1", ClampMax = "0.9", UIMin = "0.1", UIMax = "0.9", Delta = "0.05"))
	float DiffPanelRatio = 0.5f;

	UPROPERTY(Config, EditAnywhere, Category = "Appearance|Diff Viewer", meta = (ClampMin = "0.1", ClampMax = "0.9", UIMin = "0.1", UIMax = "0.9", Delta = "0.05"))
	float CommitPanelRatio = 0.5f;

	UPROPERTY(Config, EditAnywhere, Category = "Appearance|Diff Viewer", meta = (ClampMin = "0.05", ClampMax = "0.95", UIMin = "0.05", UIMax = "0.95", Delta = "0.01"))
	float HashColumnRatio = 0.15f;

	UPROPERTY(Config, EditAnywhere, Category = "Appearance|Diff Viewer", meta = (ClampMin = "0.05", ClampMax = "0.95", UIMin = "0.05", UIMax = "0.95", Delta = "0.01"))
	float MessageColumnRatio = 0.5f;

	UPROPERTY(Config, EditAnywhere, Category = "Appearance|Diff Viewer", meta = (ClampMin = "0.05", ClampMax = "0.95", UIMin = "0.05", UIMax = "0.95", Delta = "0.01"))
	float AuthorColumnRatio = 0.15f;

	UPROPERTY(Config, EditAnywhere, Category = "Appearance|Diff Viewer", meta = (ClampMin = "0.05", ClampMax = "0.95", UIMin = "0.05", UIMax = "0.95", Delta = "0.01"))
	float DateColumnRatio = 0.2f;

	/** Color of the hint that is located on the right side of the item name in the Diff List, that displays files / tree structure */
	UPROPERTY(Config, EditAnywhere, Category = "Appearance|Diff Viewer")
	FLinearColor ItemHintColor = FLinearColor(0.380, 0.427, 0.443, 0.5);

	/** Color of file in the List / Tree in Diff Viewer */
	UPROPERTY(Config, EditAnywhere, Category = "Appearance|Diff Viewer")
	TMap<EDiffHelperFileStatus, FLinearColor> StatusColors = {
		{EDiffHelperFileStatus::None, FLinearColor(1.f, 1.f, 1.f)},
		{EDiffHelperFileStatus::Added, FLinearColor(0.384f, 0.8f, 0.333f)},
		{EDiffHelperFileStatus::Modified, FLinearColor(0.408f, 0.592f, 0.95f)},
		{EDiffHelperFileStatus::Deleted, FLinearColor(0.9f, 0.2f, 0.2f)},
		{EDiffHelperFileStatus::Renamed, FLinearColor(0.5f, 0.8f, 0.8f)},
		{EDiffHelperFileStatus::Copied, FLinearColor(0.039f, 0.467f, 0.0f)},
		{EDiffHelperFileStatus::Unmerged, FLinearColor(0.835f, 0.459f, 0.424f)},
	};

	UPROPERTY(Config, EditAnywhere, Category = "Notification")
	bool bUseSuccessFailIcons = true;

	UPROPERTY(Config, EditAnywhere, Category = "Notification")
	float ErrorExpireDuration = 2.f;

	UPROPERTY(Config, EditAnywhere, Category = "Misc")
	FString UnrealDocURL = TEXT("https://dev.epicgames.com/documentation/en-us/unreal-engine/collaboration-and-version-control-in-unreal-engine");

#pragma region Git
	// TODO: First - use parsing instead of regex, second - make it configurable 
	/** Dev mode activates additional settings for Git parsing. DO NOT USE UNTIL YOU KNOW WHAT YOU'RE DOING. */
	UPROPERTY()
	bool bDevMode = false;
	
	FString BranchParserPattern = TEXT("(?:\n*\\s\\s|(?:\\*\\s))\\(*(.+?)\\s+(?:detached at \\w+\\)\\s)*(\\w+)");
	int32 BranchNameGroup = 1;
	int32 BranchRevisionGroup = 2;

	FString CommitBlockPattern = TEXT("<Hash[\\s\\S]+?(?=<Hash)|<Hash[\\s\\S]+(?=$)");
	FString CommitDataPattern = TEXT("<Hash:(.+?)> <Message:(.+?)> <Author:(.+?)> <Date:(.+?)>\n([\\s\\S]*?)(?=(?:<Hash:\\w+>|$))\n$");

	FString SingleCommitPattern = TEXT("<Hash:(.+?)> <Message:(.+?)> <Author:(.+?)> <Date:(.+?)>");
	int32 HashGroup = 1;
	int32 MessageGroup = 2;
	int32 AuthorGroup = 3;
	int32 DateGroup = 4;
	int32 ChangedFilesGroup = 5;

	FString DatePattern = TEXT("(\\d+)/(\\d+)/(\\d+) (\\d+):(\\d+)");
	int32 DayGroup = 1;
	int32 MonthGroup = 2;
	int32 YearGroup = 3;
	int32 HourGroup = 4;
	int32 MinuteGroup = 5;

	FString ChangedFilePattern = TEXT("(.).*\t(.*)(?:[\\s\n]|$)");
	int32 ChangedFileStatusGroup = 1;
	int32 ChangedFilePathGroup = 2;
#pragma endregion Git

public:
	static bool IsCachingEnabled() { return GetDefault<UDiffHelperSettings>()->bEnableCaching; }

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	
};
