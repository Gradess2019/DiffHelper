// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <UObject/Object.h>
#include "DiffHelperSettings.generated.h"

UCLASS(Config=DiffHelper)
class DIFFHELPER_API UDiffHelperSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Git")
	FString BranchParserPattern = TEXT("(?:\n*\\s\\s|(?:\\*\\s))(.+?)\\s+(\\w+)");

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
	FString ChangedFilePattern = TEXT("([a-zA-Z]{1})\\s+(.+?)(?:[\\s\n]|$)");

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 ChangedFileStatusGroup = 1;

	UPROPERTY(Config, EditAnywhere, Category = "Git")
	int32 ChangedFilePathGroup = 2;

};
