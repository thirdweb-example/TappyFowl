// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "ThirdwebCommon.h"
#include "ThirdwebWalletHandle.h"

#include "GameFramework/PlayerController.h"

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
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Settings)
	FWalletHandle InAppWallet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Settings)
	FWalletHandle SmartWallet;

public:
	UFUNCTION(BlueprintPure, Category="TappyFowl")
	FWalletHandle GetInAppWallet() const { return InAppWallet; }
	
	UFUNCTION(BlueprintPure, Category="TappyFowl")
    FWalletHandle GetSmartWallet() const { return SmartWallet; }

	UFUNCTION(BlueprintPure, Category="TappyFowl")
	UTappyFowlSaveGame* GetSaveGame() const { return SaveGame.Get(); }
	
	UFUNCTION(BlueprintPure, Category="TappyFowl", DisplayName="Get TappyFowl Player Controller", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction))
	static ATappyFowlPlayerController* Get(const UObject* WorldContextObject, int32 PlayerIndex = 0);

	UFUNCTION(BlueprintCallable, Category="TappyFowl")
	void SignOut();

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
	
	UFUNCTION(BlueprintPure, Category="TappyFowl")
	bool HasCharacter(const UCharacterDataAsset* InCharacter) const;

	UFUNCTION(BlueprintCallable, Category="TappyFowl")
	FWalletHandle SetInAppWallet(const FWalletHandle& Wallet, const EThirdwebOAuthProvider Provider);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameSet, UTappyFowlSaveGame*, SaveGame);
	UPROPERTY(BlueprintAssignable, Category="TappyFowl")
	FOnSaveGameSet OnSaveGameSet;
	
protected:
	bool AutoSignIn();
	bool CreateSmartWallet();
	void FetchBalances();
};