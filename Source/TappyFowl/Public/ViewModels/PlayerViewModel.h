// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "MVVMViewModelBase.h"
#include "PlayerViewModel.generated.h"

class ULeaderboardEntryViewModel;
class UCharacterDataAsset;
class UTappyFowlSaveGame;
/**
 * 
 */
UCLASS(BlueprintType, DisplayName="Player ViewModel")
class TAPPYFOWL_API UPlayerViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, FieldNotify)
	TArray<ULeaderboardEntryViewModel*> GetLeaderboardEntryViewModels() const { return LeaderboardEntryViewModels; };
	
	void SetLeaderboardEntryViewModels(const TArray<ULeaderboardEntryViewModel*>& NewLeaderboardEntryViewModels);
	
	UFUNCTION(BlueprintPure, FieldNotify)
	FText GetAddress() const;
	
	UFUNCTION(BlueprintPure, FieldNotify)
	FText GetDisplayAddress() const;
	
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetHighScore() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	float GetCoins() const;
	
	UFUNCTION(BlueprintPure, FieldNotify)
	UCharacterDataAsset* GetCharacter() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	TArray<UCharacterDataAsset*> GetOwnedCharacters() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	FText GetHighScoreText() const;
	
	void SetSaveGame(UTappyFowlSaveGame* NewSaveGame);

private:
	UPROPERTY(Transient)
	UTappyFowlSaveGame* SaveGame;

	UPROPERTY(Transient)
	TArray<ULeaderboardEntryViewModel*> LeaderboardEntryViewModels;
};
