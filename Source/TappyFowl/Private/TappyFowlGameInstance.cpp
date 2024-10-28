// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#include "TappyFowlGameInstance.h"

#include "GameMapsSettings.h"
#include "TappyFowl.h"
#include "TappyFowlAssetManager.h"
#include "TappyFowlSaveGame.h"

#include "Interfaces/IHttpResponse.h"

#include "Kismet/GameplayStatics.h"

#include "ViewModels/LeaderboardEntryViewModel.h"
#include "ViewModels/PlayerViewModel.h"
#include "ViewModels/ViewModelMacros.h"

void UTappyFowlGameInstance::Init()
{
	Super::Init();
	
	UTappyFowlAssetManager::LoadCharacters();
	AddGlobalViewModelChecked<UPlayerViewModel>(this);
	GetTimerManager().SetTimer(BackgroundFetchHandle, FTimerDelegate::CreateUObject(this, &ThisClass::GetHighScores), 300, true, 0);
}

UTappyFowlGameInstance* UTappyFowlGameInstance::Get(const UObject* WorldContextObject)
{
	return Cast<UTappyFowlGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

void UTappyFowlGameInstance::GetHighScores()
{
	const TSharedRef<IHttpRequest> Request = TappyFowl::HTTP::Get(TEXT("/high-scores"));
	Request->OnProcessRequestComplete().BindWeakLambda(this, [&](const FHttpRequestPtr&, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (bWasSuccessful)
		{
			// ReSharper disable once CppTooWideScopeInitStatement
			const TSharedPtr<FJsonObject>& Json = TappyFowl::JSON::StringToJsonObject(Response->GetContentAsString());
			if (Json->HasTypedField<EJson::Array>(TEXT("result")))
			{
				if (UPlayerViewModel* PlayerViewModel = GetGlobalViewModel<UPlayerViewModel>(this))
				{
					PlayerViewModel->SetLeaderboardEntryViewModels(ULeaderboardEntryViewModel::FromJson(this, Json->GetArrayField(TEXT("result"))));
				}
			}
		} else
		{
			UE_LOG(LogTemp, Error, TEXT("TappyFowlGameInstance::GetHighScores::Request Failed"))
		}
	});
	Request->ProcessRequest();
}

void UTappyFowlGameInstance::StartMusic(USoundBase* InMusic)
{
	USoundBase* Music = InMusic ? InMusic : DefaultMusic.Get();
	if (Music)
	{
		UGameplayStatics::PlaySound2D(this, Music);
	}
}

void UTappyFowlGameInstance::SetInAppWallet(const FInAppWalletHandle& NewInAppWallet)
{
	InAppWallet = NewInAppWallet;
	if (InAppWallet.IsValid() && InAppWallet.IsConnected())
	{
		InAppWallet.CacheAddress();
	}
}

void UTappyFowlGameInstance::SetSmartWallet(const FSmartWalletHandle& NewSmartWallet)
{
	SmartWallet = NewSmartWallet;
	if (SmartWallet.IsValid())
	{
		SmartWallet.CacheAddress();
	}
}

void UTappyFowlGameInstance::Reset()
{
	InAppWallet.Invalidate();
	SmartWallet.Invalidate();
	BackgroundFetchHandle.Invalidate();
	UGameplayStatics::OpenLevel(this, FName(UGameMapsSettings::GetGameDefaultMap()));
}
