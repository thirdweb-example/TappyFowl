// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#include "TappyFowlAssetManager.h"

#include "CharacterDataAsset.h"

TArray<UCharacterDataAsset*> UTappyFowlAssetManager::GetCharacterDataAssets()
{
	TArray<UObject*> Loaded;
	Get().GetPrimaryAssetObjectList(TEXT("Character"), Loaded);
	TArray<UCharacterDataAsset*> DataAssets;
	for (UObject* LoadedObject : Loaded)
	{
		if (UCharacterDataAsset* CharacterDataAsset = Cast<UCharacterDataAsset>(LoadedObject))
		{
			DataAssets.Emplace(CharacterDataAsset);
		}
	}
	return DataAssets;
}

UCharacterDataAsset* UTappyFowlAssetManager::GetCharacterDataAssetByTokenId(const int32 TokenId)
{
	// ReSharper disable once CppTooWideScopeInitStatement
	TArray<UCharacterDataAsset*> CharacterDataAssets = GetCharacterDataAssets();
	for (UCharacterDataAsset* CharacterDataAsset : CharacterDataAssets)
	{
		if (CharacterDataAsset->Id == TokenId)
		{
			return CharacterDataAsset;
		}
	}
	return nullptr;
}

UCharacterDataAsset* UTappyFowlAssetManager::GetDefaultCharacter()
{
	return GetCharacterDataAssetByTokenId(-1);
}
