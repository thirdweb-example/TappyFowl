// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "GameFramework/SaveGame.h"
#include "ThirdwebCommon.h"
#include "TappyFowlSaveGame.generated.h"

class UCharacterDataAsset;
/**
 * 
 */
UCLASS()
class TAPPYFOWL_API UTappyFowlSaveGame : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	UTappyFowlSaveGame();
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Coins;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 HighScore;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bLoggedIn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EThirdwebOAuthProvider OAuthProvider;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Email;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<int32> OwnedCharacterIds;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 SelectedCharacterId;

public:
	UFUNCTION(BlueprintPure, Category="TappyFowl|SaveGame")
	int32 GetCoins() const;

	UFUNCTION(BlueprintPure, Category="TappyFowl|SaveGame")
	int32 GetHighScore() const;

	UFUNCTION(BlueprintPure, Category="TappyFowl|SaveGame")
	bool IsLoggedIn() const;

	UFUNCTION(BlueprintPure, Category="TappyFowl|SaveGame")
	EThirdwebOAuthProvider GetOAuthProvider() const;

	UFUNCTION(BlueprintPure, Category="TappyFowl|SaveGame")
	FString GetEmail() const;

	UFUNCTION(BlueprintPure, Category="TappyFowl|SaveGame")
	TArray<UCharacterDataAsset*> GetOwnedCharacters() const;
	
	UFUNCTION(BlueprintPure, Category="TappyFowl|SaveGame")
	UCharacterDataAsset* GetSelectedCharacter() const;
	
	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void SaveHighScore(const int32 NewHighScore);

	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void SaveCoins(const int32 NewCoins);

	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void SaveAuthProvider(const EThirdwebOAuthProvider Provider);

	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void SaveEmail(const FString& NewEmail);

	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void SaveSelectedCharacter(const UCharacterDataAsset* NewCharacter);

	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void AddOwnedCharacter(const UCharacterDataAsset* NewCharacter);

	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void SaveGameResults(const int32 NewHighScore, const int32 NewCoins);
	
	static UTappyFowlSaveGame* LoadOrCreateSaveGame(APlayerController* PlayerController);

	static void DeleteSaveGame(const APlayerController* PlayerController);

private:
	static const TCHAR* SlotName;
};
