// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once


#include "GameFramework/SaveGame.h"

#include "TappyFowlSaveGame.generated.h"

struct FInAppWalletHandle;
enum class EThirdwebOAuthProvider : uint8;
struct FThirdwebLinkedAccount;
class UCharacterDataAsset;

UCLASS()
class TAPPYFOWL_API UTappyFowlSaveGame : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	UTappyFowlSaveGame();

	static const TCHAR* SlotName;
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Coins;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 HighScore;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FThirdwebLinkedAccount> LinkedAccounts;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<int32> OwnedCharacterIds;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 SelectedCharacterId;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 InAppWalletSource;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EThirdwebOAuthProvider OAuthProvider;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString AuthInput;
	
public:
	UFUNCTION(BlueprintPure, Category="TappyFowl|SaveGame")
	int32 GetCoins() const;

	UFUNCTION(BlueprintPure, Category="TappyFowl|SaveGame")
	int32 GetHighScore() const;

	UFUNCTION(BlueprintPure, Category="TappyFowl|SaveGame")
	TArray<FThirdwebLinkedAccount> GetLinkedAccounts() const;

	UFUNCTION(BlueprintPure, Category="TappyFowl|SaveGame")
	TArray<UCharacterDataAsset*> GetOwnedCharacters() const;
	
	UFUNCTION(BlueprintPure, Category="TappyFowl|SaveGame")
	UCharacterDataAsset* GetSelectedCharacter() const;
	
	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void SaveHighScore(const int32 NewHighScore);

	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void SaveCoins(const int32 NewCoins);

	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void SaveSelectedCharacter(const UCharacterDataAsset* NewCharacter);

	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void AddOwnedCharacter(const UCharacterDataAsset* NewCharacter);

	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void SaveLinkedAccounts(const TArray<FThirdwebLinkedAccount>& NewLinkedAccounts);

	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void AddLinkedAccount(const FThirdwebLinkedAccount& NewLinkedAccount);
	
	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void SaveGameResults(const int32 NewHighScore, const int32 NewCoins);

	UFUNCTION(BlueprintCallable, Category="TappyFowl|SaveGame")
	void SaveWalletAuth(const FInAppWalletHandle& Wallet, const FString& Input);

	bool IsGuestAuth() const { return InAppWalletSource == 5; }
	bool IsOAuth() const { return InAppWalletSource == 0; }
	bool IsEmail() const { return InAppWalletSource == 1; }
	bool IsPhone() const { return InAppWalletSource == 2; }
	EThirdwebOAuthProvider GetOAuthProvider() const { return OAuthProvider; }
	FString GetAuthInput() const { return AuthInput; }
	
	static UTappyFowlSaveGame* LoadOrCreateSaveGame(APlayerController* PlayerController);

	static void DeleteSaveGame(const APlayerController* PlayerController);

	static void SaveGameResult(const UObject* WorldContextObject, const int32 NewHighScore, const int32 NewCoins);

	virtual bool AsyncSaveGameToSlotForLocalPlayer() override;
	virtual bool SaveGameToSlotForLocalPlayer() override;
private:
	void UpdateViewModel(const UObject* WorldContextObject);
};
