// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#include "TappyFowlSaveGame.h"

#include <Containers/ThirdwebLinkedAccount.h>

#include "CharacterDataAsset.h"
#include "TappyFowlAssetManager.h"

#include "Kismet/GameplayStatics.h"

#include "ViewModels/PlayerViewModel.h"
#include "ViewModels/ViewModelMacros.h"

const TCHAR* UTappyFowlSaveGame::SlotName = TEXT("PlayerSaveGame");

UTappyFowlSaveGame::UTappyFowlSaveGame()
{
	Coins = 0;
	HighScore = 0;
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

TArray<FThirdwebLinkedAccount> UTappyFowlSaveGame::GetLinkedAccounts() const
{
	return LinkedAccounts;
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
	if (NewHighScore != HighScore && (NewHighScore > HighScore || NewHighScore == 0))
	{
		HighScore = NewHighScore;
		AsyncSaveGameToSlotForLocalPlayer();
	}
}

void UTappyFowlSaveGame::SaveCoins(const int32 NewCoins)
{
	if (NewCoins != Coins)
	{
		Coins = NewCoins;
		AsyncSaveGameToSlotForLocalPlayer();
	}
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

void UTappyFowlSaveGame::SaveLinkedAccounts(const TArray<FThirdwebLinkedAccount>& NewLinkedAccounts)
{
	LinkedAccounts = NewLinkedAccounts;
	AsyncSaveGameToSlotForLocalPlayer();
}

void UTappyFowlSaveGame::AddLinkedAccount(const FThirdwebLinkedAccount& NewLinkedAccount)
{
	for (const FThirdwebLinkedAccount& LinkedAccount : LinkedAccounts)
	{
		if (LinkedAccount.Id == NewLinkedAccount.Id)
		{
			return;
		}
	}
	LinkedAccounts.Emplace(NewLinkedAccount);
	AsyncSaveGameToSlotForLocalPlayer();
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

void UTappyFowlSaveGame::SaveGameResult(const UObject* WorldContextObject, const int32 NewHighScore, const int32 NewCoins)
{
	if (constexpr int32 UserIndex = 0; UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		if (UTappyFowlSaveGame* SaveGame = Cast<UTappyFowlSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex)))
		{
			bool bDirty = false;
			if (NewHighScore > SaveGame->HighScore)
			{
				SaveGame->HighScore = NewHighScore;
				bDirty = true;
			}
			if (NewCoins > 0)
			{
				SaveGame->Coins = SaveGame->Coins + NewCoins;
				bDirty = true;
			}
			if (bDirty)
			{
				UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex);
				SaveGame->UpdateViewModel(WorldContextObject);
			}
		}
	}
}

bool UTappyFowlSaveGame::AsyncSaveGameToSlotForLocalPlayer()
{
	bool bSuccess = Super::AsyncSaveGameToSlotForLocalPlayer();
	UpdateViewModel(GetLocalPlayer());
	return bSuccess;
}

bool UTappyFowlSaveGame::SaveGameToSlotForLocalPlayer()
{
	bool bSuccess = Super::SaveGameToSlotForLocalPlayer();
	UpdateViewModel(GetLocalPlayer());
	return bSuccess;
}

void UTappyFowlSaveGame::UpdateViewModel(const UObject* WorldContextObject)
{
	if (UPlayerViewModel* PlayerViewModel = GetGlobalViewModel<UPlayerViewModel>(WorldContextObject))
	{
		PlayerViewModel->SetSaveGame(this);
	}
}
