// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintPlatformLibrary.h"

#include "Wallets/ThirdwebInAppWalletHandle.h"
#include "Wallets/ThirdwebSmartWalletHandle.h"

#include "TappyFowlGameInstance.generated.h"

class UTappyFowlSaveGame;
/**
 * 
 */
UCLASS()
class TAPPYFOWL_API UTappyFowlGameInstance : public UPlatformGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	
	UFUNCTION(BlueprintPure, Category=TappyFowl, DisplayName="Get TappyFowl Game Instance", meta=(WorldContext="WorldContextObject"))
	static UTappyFowlGameInstance* Get(const UObject* WorldContextObject);

	void GetHighScores();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category=Audio, meta=(BlueprintCosmetic, UnsafeDuringActorConstruction))
	void StartMusic(USoundBase* InMusic);

	FInAppWalletHandle GetInAppWallet() const { return InAppWallet; }
	void SetInAppWallet(const FInAppWalletHandle& NewInAppWallet);
	FSmartWalletHandle GetSmartWallet() const { return SmartWallet; }
	void SetSmartWallet(const FSmartWalletHandle& NewSmartWallet);

	void Reset();
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Persistant)
	FInAppWalletHandle InAppWallet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Persistant)
	FSmartWalletHandle SmartWallet;
	
	UPROPERTY(Transient)
	FTimerHandle BackgroundFetchHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Audio)
	TObjectPtr<USoundBase> DefaultMusic;
};
