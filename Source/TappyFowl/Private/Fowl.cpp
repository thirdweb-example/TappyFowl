// Copyright (c) 2024 Thirdweb. All Rights Reserved.


#include "Fowl.h"

// Sets default values
AFowl::AFowl()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFowl::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFowl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFowl::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

