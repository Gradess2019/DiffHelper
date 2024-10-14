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
	/** Files with these statuses couldn't be used for diff */
	UPROPERTY(Config, EditAnywhere, Category = "General")
	TArray<EDiffHelperFileStatus> StatusBlacklist = {
		EDiffHelperFileStatus::None,
		EDiffHelperFileStatus::Deleted,
		EDiffHelperFileStatus::Unmerged,
	};

	/** If true, the plugin will try to find and reuse opened diff tab. If false, it will always open a new tab */
	UPROPERTY(Config, EditAnywhere, Category = "General")
	bool bReuseDiffTab = true;

	/** Enables opening external diff for non-assets */
	UPROPERTY(Config, EditAnywhere, Category = "Diff")
	bool bEnableExternalDiff = false;

	/** Command that will be used to open diff in external editor for non-assets. By default, it's set to Visual Studio Code
	 * {0} - source file path
	 * {1} - target file path
	 * {2} - source file revision
	 * {3} - target file revision
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Diff", meta = (EditCondition = "bEnableExternalDiff"))
	FString ExternalDiffCommand = TEXT("code --diff {0} {1}");

	/** if true the plugin will cache source and target branches and will put them into selectors on a first Diff Helper view */
	UPROPERTY(Config, EditAnywhere, Category = "Caching")
	bool bEnableCaching = true;

#pragma region Git
	UPROPERTY(Config, EditAnywhere, Category = "Git")
	FString BranchParserPattern = TEXT("(?:\n*\\s\\s|(?:\\*\\s))\\(*(.+?)\\s+(?:detached at \\w+\\)\\s)*(\\w+)");

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 BranchNameGroup = 1;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 BranchRevisionGroup = 2;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	FString CommitBlockPattern = TEXT("<Hash[\\s\\S]+?(?=<Hash)|<Hash[\\s\\S]+(?=$)");

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	FString CommitDataPattern = TEXT("<Hash:(.+?)> <Message:(.+?)> <Author:(.+?)> <Date:(.+?)>\n([\\s\\S]*?)(?=(?:<Hash:\\w+>|$))\n$");

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	FString SingleCommitPattern = TEXT("<Hash:(.+?)> <Message:(.+?)> <Author:(.+?)> <Date:(.+?)>");

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 HashGroup = 1;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 MessageGroup = 2;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 AuthorGroup = 3;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 DateGroup = 4;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 ChangedFilesGroup = 5;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	FString DatePattern = TEXT("(\\d+)/(\\d+)/(\\d+) (\\d+):(\\d+)");

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 DayGroup = 1;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 MonthGroup = 2;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 YearGroup = 3;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 HourGroup = 4;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 MinuteGroup = 5;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	FString ChangedFilePattern = TEXT("(.).*\t(.*)(?:[\\s\n]|$)");

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 ChangedFileStatusGroup = 1;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 ChangedFilePathGroup = 2;
#pragma endregion Git

	UPROPERTY(Config, EditAnywhere, Category = "Appearance")
	TMap<EDiffHelperFileStatus, FLinearColor> StatusColors = {
		{EDiffHelperFileStatus::None, FLinearColor(1.f, 1.f, 1.f)},
		{EDiffHelperFileStatus::Added, FLinearColor(0.384f, 0.8f, 0.333f)},
		{EDiffHelperFileStatus::Modified, FLinearColor(0.408f, 0.592f, 0.95f)},
		{EDiffHelperFileStatus::Deleted, FLinearColor(0.9f, 0.2f, 0.2f)},
		{EDiffHelperFileStatus::Renamed, FLinearColor(0.5f, 0.8f, 0.8f)},
		{EDiffHelperFileStatus::Copied, FLinearColor(0.039f, 0.467f, 0.0f)},
		{EDiffHelperFileStatus::Unmerged, FLinearColor(0.835f, 0.459f, 0.424f)},
	};

	// Color of the hint that is located on the right side of the item name in the Diff List, that displays files / tree structure
	UPROPERTY(Config, EditAnywhere, Category = "Appearance")
	FLinearColor ItemHintColor = FLinearColor(0.380, 0.427, 0.443, 0.5);

	UPROPERTY(Config, EditAnywhere, Category = "Notification")
	bool bUseSuccessFailIcons = true;

	UPROPERTY(Config, EditAnywhere, Category = "Notification")
	float ErrorExpireDuration = 2.f;

	UPROPERTY(Config, EditAnywhere, Category = "Misc")
	FString UnrealDocURL = TEXT("https://dev.epicgames.com/documentation/en-us/unreal-engine/collaboration-and-version-control-in-unreal-engine");

public:
	static bool IsCachingEnabled() { return GetDefault<UDiffHelperSettings>()->bEnableCaching; }
};
