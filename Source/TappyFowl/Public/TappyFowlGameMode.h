// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ThirdwebWalletHandle.h"

#include "Engine/StreamableManager.h"

#include "GameFramework/GameMode.h"
#include "TappyFowlGameMode.generated.h"

class ATappyFowlPlayerController;

/**
 * 
 */
UCLASS()
class TAPPYFOWL_API ATappyFowlGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Score = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Coins = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString WalletAddress;
};
