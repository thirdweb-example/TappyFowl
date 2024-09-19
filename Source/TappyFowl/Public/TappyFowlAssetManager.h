// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "Engine/AssetManager.h"

#include "TappyFowlAssetManager.generated.h"

class UCharacterDataAsset;

/**
 * 
 */
UCLASS(BlueprintType)
class TAPPYFOWL_API UTappyFowlAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static void LoadCharacters()
	{
		Get().LoadPrimaryAssetsWithType(TEXT("Character"), {}, FStreamableDelegate::CreateLambda([&]()
		{
			UE_LOG(LogTemp, Log, TEXT("UTappyFowlAssetManager::LoadCharacters::Loaded %d"), GetCharacterDataAssets().Num())
		}));
	}
	
	UFUNCTION(BlueprintPure, Category=TappyFowl)
	static TArray<UCharacterDataAsset*> GetCharacterDataAssets();
	
	UFUNCTION(BlueprintPure, Category=TappyFowl)
	static UCharacterDataAsset* GetCharacterDataAssetByTokenId(const int32 TokenId);
	
	UFUNCTION(BlueprintPure, Category=TappyFowl)
	static UCharacterDataAsset* GetDefaultCharacter();
};
