// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DiffHelperRevisionPickerController.generated.h"

struct FDiffHelperBranch;
class UDiffHelperRevisionPickerModel;

class FDiffHelperTabSerchPreference : public FTabManager::FSearchPreference
{
	virtual TSharedPtr<SDockTab> Search(const FTabManager& Manager, FName PlaceholderId, const TSharedRef<SDockTab>& UnmanagedTab) const override;
};

UCLASS()
class DIFFHELPER_API UDiffHelperRevisionPickerController : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintGetter="GetModel")
	TObjectPtr<UDiffHelperRevisionPickerModel> Model;

public:
	UFUNCTION()
	virtual void Init();

	UFUNCTION()
	virtual void Deinit();
	
	UFUNCTION(BlueprintGetter)
	const UDiffHelperRevisionPickerModel* GetModel() const { return Model.Get(); }

	UFUNCTION()
	void SetSourceBranch(const FDiffHelperBranch& InBranch);

	UFUNCTION()
	void SetTargetBranch(const FDiffHelperBranch& InBranch);

	UFUNCTION()
	void OpenDiffTab();

private:
	void InitModel();
	void LoadCachedBranches();

	TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& InSpawnTabArgs);
	TSharedRef<SDockTab> SpawnTab();
	bool CanSpawnTab(const FSpawnTabArgs& InSpawnTabArgs) const;
	TSharedPtr<SDockTab> FindTabToReuse(const FTabId& InTabId);

};
