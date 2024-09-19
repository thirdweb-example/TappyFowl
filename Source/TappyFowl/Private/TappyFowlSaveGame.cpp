// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#include "TappyFowlSaveGame.h"

#include "CharacterDataAsset.h"
#include "TappyFowlAssetManager.h"

#include "Kismet/GameplayStatics.h"

const TCHAR* UTappyFowlSaveGame::SlotName = TEXT("PlayerSaveGame");

UTappyFowlSaveGame::UTappyFowlSaveGame()
{
	Coins = 0;
	HighScore = 0;
	bLoggedIn = false;
	OAuthProvider = EThirdwebOAuthProvider::Google;
	Email = TEXT("");
	SelectedCharacterId = -1;
	OwnedCharacterIds = {-1};
}

int32 UTappyFowlSaveGame::GetCoins() const
{
	return Coins;
}

int32 UTappyFowlSaveGame::GetHighScore() const
{
	return HighScore;
}

bool UTappyFowlSaveGame::IsLoggedIn() const
{
	return bLoggedIn;
}

EThirdwebOAuthProvider UTappyFowlSaveGame::GetOAuthProvider() const
{
	return OAuthProvider;
}

FString UTappyFowlSaveGame::GetEmail() const
{
	return Email;
}

TArray<UCharacterDataAsset*> UTappyFowlSaveGame::GetOwnedCharacters() const
{
	TArray<UCharacterDataAsset*> Characters;
	for (int32 i = 0; i < OwnedCharacterIds.Num(); i++)
	{
		Characters.Emplace(UTappyFowlAssetManager::GetCharacterDataAssetByTokenId(OwnedCharacterIds[i]));
	}
	return Characters;
}

UCharacterDataAsset* UTappyFowlSaveGame::GetSelectedCharacter() const
{
	return UTappyFowlAssetManager::GetCharacterDataAssetByTokenId(SelectedCharacterId);
}

void UTappyFowlSaveGame::SaveHighScore(const int32 NewHighScore)
{
	if (NewHighScore > HighScore)
	{
		HighScore = NewHighScore;
		AsyncSaveGameToSlotForLocalPlayer();
	}
}

void UTappyFowlSaveGame::SaveCoins(const int32 NewCoins)
{
	Coins = NewCoins;
	AsyncSaveGameToSlotForLocalPlayer();
}

void UTappyFowlSaveGame::SaveAuthProvider(const EThirdwebOAuthProvider Provider)
{
	OAuthProvider = Provider;
	bLoggedIn = true;
	AsyncSaveGameToSlotForLocalPlayer();
}

void UTappyFowlSaveGame::SaveEmail(const FString& NewEmail)
{
	Email = NewEmail;
	bLoggedIn = true;
	AsyncSaveGameToSlotForLocalPlayer();
}

void UTappyFowlSaveGame::SaveSelectedCharacter(const UCharacterDataAsset* NewCharacter)
{
	if (!NewCharacter)
	{
		if (SelectedCharacterId != -1)
		{
			SelectedCharacterId = -1;
			AsyncSaveGameToSlotForLocalPlayer();
		}
	}
	else if (NewCharacter->Id != SelectedCharacterId)
	{
		SelectedCharacterId = NewCharacter->Id;
		AsyncSaveGameToSlotForLocalPlayer();
	}
}

void UTappyFowlSaveGame::AddOwnedCharacter(const UCharacterDataAsset* NewCharacter)
{
	if (NewCharacter)
	{
		for (int32 i = 0; i < OwnedCharacterIds.Num(); i++)
		{
			if (OwnedCharacterIds[i] == NewCharacter->Id)
			{
				return;
			}
		}
		OwnedCharacterIds.Emplace(NewCharacter->Id);
		AsyncSaveGameToSlotForLocalPlayer();
	}
}

void UTappyFowlSaveGame::SaveGameResults(const int32 NewHighScore, const int32 NewCoins)
{
	bool bDirty = false;
	if (NewHighScore > HighScore)
	{
		HighScore = NewHighScore;
		bDirty = true;
	}
	if (NewCoins > 0)
	{
		Coins = Coins + NewCoins;
		bDirty = true;
	}
	if (bDirty)
	{
		AsyncSaveGameToSlotForLocalPlayer();
	}
}

UTappyFowlSaveGame* UTappyFowlSaveGame::LoadOrCreateSaveGame(APlayerController* PlayerController)
{
	return Cast<UTappyFowlSaveGame>(LoadOrCreateSaveGameForLocalPlayer(StaticClass(), PlayerController, SlotName));
}

void UTappyFowlSaveGame::DeleteSaveGame(const APlayerController* PlayerController)
{
	// ReSharper disable once CppTooWideScopeInitStatement
	const ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr;

	if (!ensure(LocalPlayer))
	{
		UE_LOG(LogPlayerManagement, Error, TEXT("LoadOrCreateSaveGameForLocalPlayer called with null or remote player controller!"));
	}
	else
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, LocalPlayer->GetPlatformUserIndex());
	}
}
