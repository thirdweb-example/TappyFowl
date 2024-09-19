// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#include "ViewModels/PlayerViewModel.h"

#include "TappyFowlAssetManager.h"
#include "TappyFowlPlayerController.h"
#include "TappyFowlSaveGame.h"

#include "Kismet/GameplayStatics.h"

#include "ViewModels/LeaderboardEntryViewModel.h"

#define LOCTEXT_NAMESPACE "TappyFowl"

void UPlayerViewModel::SetLeaderboardEntryViewModels(const TArray<ULeaderboardEntryViewModel*>& NewLeaderboardEntryViewModels)
{
	LeaderboardEntryViewModels = NewLeaderboardEntryViewModels;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetLeaderboardEntryViewModels);
	// ReSharper disable once CppTooWideScopeInitStatement
	FString Address = GetAddress().ToString();
	if (!SaveGame)
	{
		SetSaveGame(Cast<ATappyFowlPlayerController>(UGameplayStatics::GetPlayerController(this, 0))->GetSaveGame());
	}
	if (GetHighScore() == 0 && !Address.IsEmpty())
	{
		for (int32 i = 0; i < LeaderboardEntryViewModels.Num(); i++)
		{
			if (LeaderboardEntryViewModels[i]->GetAddress().Equals(Address, ESearchCase::IgnoreCase))
			{
				
				SaveGame->SaveHighScore(LeaderboardEntryViewModels[i]->GetHighScore());
				UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHighScore);
				UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHighScoreText);
				break;
			}
		}
	}
}

FText UPlayerViewModel::GetAddress() const
{
	if (ATappyFowlPlayerController* PlayerController = Cast<ATappyFowlPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (FWalletHandle Wallet = PlayerController->GetSmartWallet(); Wallet.IsValid())
		{
			return FText::FromString(Wallet.ToAddress());
		}
	}
	return FText::GetEmpty();
}

FText UPlayerViewModel::GetDisplayAddress() const
{
	if (FText Address = GetAddress(); !Address.IsEmpty())
	{
		return FText::Format(FText::FromString(TEXT("{0}...{1}")), FText::FromString(Address.ToString().Left(6)), FText::FromString(Address.ToString().Right(4)));
	}
	return FText::GetEmpty();
}

float UPlayerViewModel::GetHighScore() const
{
	return SaveGame ? SaveGame->GetHighScore() : 0.0f;
}

float UPlayerViewModel::GetCoins() const
{
	return SaveGame ? SaveGame->GetCoins() : 0.0f;
}

UCharacterDataAsset* UPlayerViewModel::GetCharacter() const
{
	return SaveGame ? SaveGame->GetSelectedCharacter() : UTappyFowlAssetManager::GetDefaultCharacter();
}

TArray<UCharacterDataAsset*> UPlayerViewModel::GetOwnedCharacters() const
{
	return SaveGame ? SaveGame->GetOwnedCharacters() : TArray({UTappyFowlAssetManager::GetDefaultCharacter()});
}

FText UPlayerViewModel::GetHighScoreText() const
{
	return GetHighScore() == 0.0f ? FText::GetEmpty() : FText::Format(LOCTEXT("High Score", "High Score: {0}"), FText::FromString(FString::FromInt(GetHighScore())));
}

void UPlayerViewModel::SetSaveGame(UTappyFowlSaveGame* NewSaveGame)
{
	SaveGame = NewSaveGame;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetAddress);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetDisplayAddress);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHighScore);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetCoins);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetCharacter);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetOwnedCharacters);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHighScoreText);
}

#undef LOCTEXT_NAMESPACE