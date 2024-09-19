// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#include "TappyFowlGameInstance.h"

#include "HttpModule.h"
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
	FHttpModule& HttpModule = FHttpModule::Get();
	const TSharedRef<IHttpRequest> Request = HttpModule.CreateRequest();
	Request->SetVerb("GET");
	Request->SetURL("https://thirdweb-example-tappyfowl-api-9rgm.zeet-nftlabs.zeet.app/421614/high-scores");
	Request->SetHeader("Content-Type", "application/json");
	Request->OnProcessRequestComplete().BindWeakLambda(this, [&](const FHttpRequestPtr&, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (bWasSuccessful)
		{
			// ReSharper disable once CppTooWideScopeInitStatement
			const TSharedPtr<FJsonObject>& Json = TappyFowl::Json::StringToJsonObject(Response->GetContentAsString());
			if (Json->HasTypedField<EJson::Array>(TEXT("result")))
			{
				TArray<TSharedPtr<FJsonValue>> Results = Json->GetArrayField(TEXT("result"));
				TArray<ULeaderboardEntryViewModel*> LeaderboardEntryViewModels;
				for (int32 i = 0; i < Results.Num(); i++)
				{
					if (ULeaderboardEntryViewModel* LeaderboardEntryViewModel = ULeaderboardEntryViewModel::FromJson(this, Results[i]))
					{
						LeaderboardEntryViewModels.Emplace(LeaderboardEntryViewModel);
					}
				}
				if (UPlayerViewModel* PlayerViewModel = GetGlobalViewModel<UPlayerViewModel>(this))
				{
					PlayerViewModel->SetLeaderboardEntryViewModels(LeaderboardEntryViewModels);
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
