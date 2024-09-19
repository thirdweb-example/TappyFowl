// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "GameFramework/Pawn.h"
#include "Fowl.generated.h"

class UCharacterDataAsset;

UCLASS(Blueprintable, BlueprintType)
class TAPPYFOWL_API AFowl : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFowl();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Bools, meta=(ExposeOnSpawn=true))
	bool bRespawned = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Data, meta=(ExposeOnSpawn=true))
	UCharacterDataAsset* Data;
};
