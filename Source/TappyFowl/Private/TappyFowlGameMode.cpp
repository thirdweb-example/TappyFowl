// Copyright (c) 2024 Thirdweb. All Rights Reserved.


#include "TappyFowlGameMode.h"

#include "HttpModule.h"
#include "TappyFowl.h"
#include "TappyFowlPlayerController.h"

void ATappyFowlGameMode::SendGameResults(const ATappyFowlPlayerController* PlayerController)
{
	if (!PlayerController)
	{
		UE_LOG(LogGameMode, Error, TEXT("TappyFowlGameMode::SendGameResults::Invalid PlayerController"))
		return;
	}
	if (Score == 0)
	{
		return;
	}
	
	FHttpModule& HttpModule = FHttpModule::Get();
	const TSharedRef<IHttpRequest> Request = HttpModule.CreateRequest();
	Request->SetVerb("POST");
	Request->SetURL("https://thirdweb-example-tappyfowl-api-9rgm.zeet-nftlabs.zeet.app/421614/result");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", "e5447b62-09ed-44db-a843-904a5e387041");

	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("address"), PlayerController->GetSmartWallet().ToAddress());
	JsonObject->SetNumberField(TEXT("highScore"), Score);
	JsonObject->SetNumberField(TEXT("tokens"), Coins);
	Request->SetContentAsString(TappyFowl::Json::JsonObjectToString(JsonObject));
	Request->OnProcessRequestComplete().BindWeakLambda(this, [&](const FHttpRequestPtr&, const FHttpResponsePtr&, const bool bWasSuccessful)
	{
		if (bWasSuccessful)
		{
			OnResultsSent();
			UE_LOG(LogGameMode, Error, TEXT("TappyFowlGameMode::SendGameResults::Sent!"))
		} else
		{
			UE_LOG(LogGameMode, Error, TEXT("TappyFowlGameMode::SendGameResults::Request Failed"))
		}
	});
	Request->ProcessRequest();
}
