// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DiffHelperTabController.generated.h"

class UDiffHelperTabModel;
/**
 * 
 */
UCLASS(BlueprintType)
class DIFFHELPER_API UDiffHelperTabController : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintGetter="GetModel")
	TObjectPtr<UDiffHelperTabModel> Model;

public:
	UFUNCTION()
	void Init();

	UFUNCTION()
	void Deinit();
	
	UFUNCTION(BlueprintGetter)
	const UDiffHelperTabModel* GetModel() const { return Model.Get(); }
};
