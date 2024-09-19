// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "CharacterDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TAPPYFOWL_API UCharacterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General")
	int32 Id = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General", DisplayName="Name")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General")
	bool bIncluded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General", meta=(EditCondition="!bIncluded", EditConditionHides))
	int32 Price = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assets", meta=(AssetBundles="Mesh"))
	TSoftObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assets")
	FTransform SpawnTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animations", meta=(AssetBundles="Animations"))
	TSoftClassPtr<UAnimInstance> AnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animations", meta=(AssetBundles="Animations"))
	TSoftObjectPtr<UAnimationAsset> StoreAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animations", meta=(AssetBundles="Animations"))
	TSoftObjectPtr<UAnimMontage> FlapMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TObjectPtr<UMaterialInterface> RenderMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	FTransform StoreTransform;

	UFUNCTION(BlueprintPure, Category="TappyFowl|Assets")
	USkeletalMesh* GetMesh();
	
	UFUNCTION(BlueprintPure, Category="TappyFowl|Assets")
	TSubclassOf<UAnimInstance> GetAnimClass();
	
	UFUNCTION(BlueprintPure, Category="TappyFowl|Assets")
	UAnimationAsset* GetStoreAnimation();
	
	UFUNCTION(BlueprintPure, Category="TappyFowl|Assets")
	UAnimMontage* GetFlapMontage();

	
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Character", GetFName());
	}
};
