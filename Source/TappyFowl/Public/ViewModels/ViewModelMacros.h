// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "MVVMGameSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "Types/MVVMViewModelCollection.h"

inline UMVVMViewModelCollectionObject* GetGlobalViewModelCollection(const UObject* WorldContextObject)
{
	if (const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
		if (const UMVVMGameSubsystem* MvvmSubsystem = GameInstance->GetSubsystem<UMVVMGameSubsystem>()) return MvvmSubsystem->GetViewModelCollection();
		else UE_LOG(LogTemp, Error, TEXT("GetGlobalViewModelCollection::No MVVM Game Subsystem"))
	else UE_LOG(LogTemp, Error, TEXT("GetGlobalViewModelCollection::No Game Instance"))
	return nullptr;
}

template <typename TViewModelClass>
FMVVMViewModelContext CreateViewModelContext(const FName InName = NAME_None)
{
	const FName Name = InName.IsNone() ? FName(*TViewModelClass::StaticClass()->GetName()) : InName;
	return {TViewModelClass::StaticClass(), Name};
}

template<typename TViewModelClass>
TObjectPtr<TViewModelClass> GetCollectionViewModel(UMVVMViewModelCollectionObject* Collection, const FName InName = NAME_None)
{
	return Cast<TViewModelClass>(Collection->FindViewModelInstance(CreateViewModelContext<TViewModelClass>(InName)));
}

template <typename TViewModelClass>
TViewModelClass* GetGlobalViewModel(const UObject* WorldContextObject)
{
	if (UMVVMViewModelCollectionObject* Collection = GetGlobalViewModelCollection(WorldContextObject))
		if (TViewModelClass* ViewModel = GetCollectionViewModel<TViewModelClass>(Collection)) return ViewModel;
		else UE_LOG(LogTemp, Error, TEXT("GetGlobalViewModel::%s::No View Model"), *TViewModelClass::StaticClass()->GetName())
	else UE_LOG(LogTemp, Error, TEXT("GetGlobalViewModel::%s::No Global View Model Collection"), *TViewModelClass::StaticClass()->GetName())
	return nullptr;
}

template <typename TViewModelClass>
TViewModelClass* AddGlobalViewModelChecked(const UObject* WorldContextObject)
{
	FString Name = TViewModelClass::StaticClass()->GetName();
	if (UMVVMViewModelCollectionObject* Collection = GetGlobalViewModelCollection(WorldContextObject))
	{
		
		if (TViewModelClass* ViewModel = GetCollectionViewModel<TViewModelClass>(Collection))
		{
			UE_LOG(LogTemp, Warning, TEXT("AddGlobalViewModelChecked::%s::Already exists"), *Name);
			return ViewModel;
		}
		TViewModelClass* ViewModel = NewObject<TViewModelClass>(UGameplayStatics::GetGameInstance(WorldContextObject));
		if (Collection->AddViewModelInstance(CreateViewModelContext<TViewModelClass>(), ViewModel))
		{
			return ViewModel;
		}
		UE_LOG(LogTemp, Error, TEXT("AddGlobalViewModelChecked::%s::Adding failed"), *Name);
		
	} else UE_LOG(LogTemp, Error, TEXT("AddGlobalViewModelChecked::%s::No Global View Model Collection"), *Name);
	return nullptr;
}
