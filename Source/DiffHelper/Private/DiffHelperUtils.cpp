// Copyright 2024 Gradess Games. All Rights Reserved.


#include "DiffHelperUtils.h"
#include "DiffHelperTypes.h"

TArray<FString> UDiffHelperUtils::ConvertBranchesToStringArray(const TArray<FDiffHelperBranch>& InBranches)
{
	TArray<FString> OutArray;
	OutArray.Reserve(InBranches.Num());
	for (const auto& Branch : InBranches)
	{
		OutArray.Add(Branch.Name);
	}

	return OutArray;
}

bool UDiffHelperUtils::CompareStatus(const EDiffHelperFileStatus InStatusA, const EDiffHelperFileStatus InStatusB)
{
	return static_cast<uint8>(InStatusA) < static_cast<uint8>(InStatusB);
}

TArray<TSharedPtr<FDiffHelperItemNode>> UDiffHelperUtils::GenerateTree(const TArray<TSharedPtr<FDiffHelperDiffItem>>& InItems)
{
	TArray<TSharedPtr<FDiffHelperItemNode>> OutArray;

	TSet<FString> Paths;
	for (const auto DiffItem : InItems)
	{
		const auto& Path = DiffItem->Path;
		Paths.Add(Path);
	}

	TSharedPtr<FDiffHelperItemNode> Root = MakeShared<FDiffHelperItemNode>();
	for (const auto& Path : Paths)
	{
		TArray<FString> PathComponents;
		Path.ParseIntoArray(PathComponents, TEXT("/"), true);

		TSharedPtr<FDiffHelperItemNode> Current = Root;
		FString CurrentPath = "";

		for (const auto& PathComponent : PathComponents)
		{
			CurrentPath = CurrentPath + "/" + PathComponent;

			TSharedPtr<FDiffHelperItemNode> Child;
			for (const auto& CurrentChil : Current->Children)
			{
				if (CurrentChil->Path == CurrentPath)
				{
					Child = CurrentChil;
					break;
				}
			}

			if (!Child.IsValid())
			{
				Child = MakeShared<FDiffHelperItemNode>(CurrentPath);
				Current->Children.Add(Child);
			}

			Current = Child;
		}
	}

	return Root->Children;
}
