// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "ThirdwebCommon.h"

#include "GameFramework/PlayerController.h"

#include "Wallets/ThirdwebInAppWalletHandle.h"
#include "Wallets/ThirdwebSmartWalletHandle.h"

#include "TappyFowlPlayerController.generated.h"

class UCharacterDataAsset;
class UTappyFowlSaveGame;
/**
 * 
 */
UCLASS()
class TAPPYFOWL_API ATappyFowlPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Settings)
	TObjectPtr<UTappyFowlSaveGame> SaveGame;

public:
	UFUNCTION(BlueprintPure, Category="TappyFowl")
	FInAppWalletHandle GetInAppWallet() const;

	UFUNCTION(BlueprintPure, Category="TappyFowl")
	TArray<EThirdwebOAuthProvider> GetLinkedOAuthProviders() const;

	UFUNCTION(BlueprintPure, Category="TappyFowl")
	void GetLinkedOTPProviders(bool& bEmail, bool& bPhone) const;
	
	UFUNCTION(BlueprintPure, Category="TappyFowl")
    FSmartWalletHandle GetSmartWallet() const;

	UFUNCTION(BlueprintPure, Category="TappyFowl")
	UTappyFowlSaveGame* GetSaveGame() const { return SaveGame.Get(); }
	
	UFUNCTION(BlueprintPure, Category="TappyFowl", DisplayName="Get TappyFowl Player Controller", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction))
	static ATappyFowlPlayerController* Get(const UObject* WorldContextObject, int32 PlayerIndex = 0);

	UFUNCTION(BlueprintCallable, Category="TappyFowl")
	void SignOut();

	UFUNCTION(BlueprintCallable, Category="TappyFowl")
	void SwitchAccounts(const FInAppWalletHandle& Wallet, const FString& AuthInput);
	
	UFUNCTION(BlueprintCallable, Category="TappyFowl")
	void UnlockCharacter(const UCharacterDataAsset* NewCharacter);

	UFUNCTION(BlueprintPure, Category="TappyFowl")
	UCharacterDataAsset* GetSelectedCharacter() const;

	UFUNCTION(BlueprintCallable, Category="TappyFowl")
	void SetSelectedCharacter(UCharacterDataAsset* NewCharacter);

	UFUNCTION(BlueprintPure, Category="TappyFowl")
	TArray<UCharacterDataAsset*> GetOwnedCharacters() const;

	UFUNCTION(BlueprintPure, Category="TappyFowl")
	int32 GetCoins() const;
	
	UFUNCTION(BlueprintPure, Category="TappyFowl")
	int32 GetHighScore() const;

	UFUNCTION(BlueprintCallable, Category="TappyFowl")
	void SetHighScore(const int32 NewHighScore);

	UFUNCTION(BlueprintCallable, Category="TappyFowl")
	void AddCoins(const int32 NewCoins);

	UFUNCTION(BlueprintCallable, Category="TappyFowl")
	void FetchLinkedWallets();
	
	UFUNCTION(BlueprintPure, Category="TappyFowl")
	bool HasCharacter(const UCharacterDataAsset* InCharacter) const;

	UFUNCTION(BlueprintCallable, Category="TappyFowl")
	FInAppWalletHandle SetInAppWallet(const FInAppWalletHandle& Wallet);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameSet, UTappyFowlSaveGame*, SaveGame);
	UPROPERTY(BlueprintAssignable, Category="TappyFowl")
	FOnSaveGameSet OnSaveGameSet;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoggedIn, FString, Address);
	UPROPERTY(BlueprintAssignable, Category="TappyFowl")
	FOnLoggedIn OnLoggedIn;
protected:
	bool AutoSignIn();
	bool CreateSmartWallet();
	void CreateSessionKey(const FString& EngineSigner);
	void FetchBalances();

	void HandleInAppWalletCreated(const FInAppWalletHandle& Wallet);
	void HandleSmartWalletCreated(const FSmartWalletHandle& Wallet);
	void HandleSmartWalletIsDeployed(const bool& bIsDeployed);
	void HandleSessionKeyCreated(const FString& TxHash);
	void HandleFetchLinkedAccounts(const TArray<FThirdwebLinkedAccount>& LinkedAccounts);
};
