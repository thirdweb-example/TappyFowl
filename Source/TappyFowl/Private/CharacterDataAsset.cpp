// Copyright (c) 2024 Thirdweb. All Rights Reserved.


#include "CharacterDataAsset.h"

USkeletalMesh* UCharacterDataAsset::GetMesh()
{
	return Mesh.LoadSynchronous();
}

TSubclassOf<UAnimInstance> UCharacterDataAsset::GetAnimClass()
{
	return AnimClass.LoadSynchronous();
}

UAnimationAsset* UCharacterDataAsset::GetStoreAnimation()
{
	return StoreAnimation.LoadSynchronous();
}

UAnimMontage* UCharacterDataAsset::GetFlapMontage()
{
	return FlapMontage.LoadSynchronous();
}
