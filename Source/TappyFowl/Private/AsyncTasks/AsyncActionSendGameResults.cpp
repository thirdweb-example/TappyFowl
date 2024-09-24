// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#include "AsyncTasks/AsyncActionSendGameResults.h"

#include "TappyFowl.h"
#include "TappyFowlGameMode.h"
#include "TappyFowlSaveGame.h"

#include "Interfaces/IHttpResponse.h"

#include "Kismet/GameplayStatics.h"

void UAsyncTaskSendGameResults::Activate()
{
	UE_LOG(LogTemp, Log, TEXT("AsyncTaskSendGameResults::Activate::Sending Score"))
	if (GameMode->Score == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("AsyncTaskSendGameResults::Activate::Score is 0"))
		return HandleSuccess();
	}
	if (GameMode->WalletAddress.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("AsyncTaskSendGameResults::Activate::Empty Wallet"))
		return HandleFailed();
	}

	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("address"), GameMode->WalletAddress);
	JsonObject->SetNumberField(TEXT("highScore"), GameMode->Score);
	JsonObject->SetNumberField(TEXT("tokens"), GameMode->Coins);

	const TSharedRef<IHttpRequest> Request = TappyFowl::HTTP::Post(TEXT("/result"), JsonObject);
	UE_LOG(LogTemp, Log, TEXT("AsyncTaskSendGameResults::Activate::Payload::%s"), *TappyFowl::JSON::JsonObjectToString(JsonObject))
	Request->OnProcessRequestComplete()
	       .BindWeakLambda(this, [&](const FHttpRequestPtr&, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	       {
		       if (bWasSuccessful)
		       {
			       UE_LOG(LogGameMode, Log, TEXT("AsyncTaskSendGameResults::SendGameResults::Success::%s"), *Response->GetContentAsString())
			       HandleSuccess();
		       }
		       else
		       {
			       UE_LOG(LogGameMode, Error, TEXT("AsyncTaskSendGameResults::SendGameResults::Request Failed"))
			       HandleFailed();
		       }
	       });
	Request->ProcessRequest();

	UTappyFowlSaveGame::SaveGameResult(GameMode, GameMode->Score, GameMode->Coins);
}

UAsyncTaskSendGameResults* UAsyncTaskSendGameResults::SendGameResults(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		UE_LOG(LogTemp, Error, TEXT("AsyncTaskSendGameResults::SendGameResults::No World Context"))
		return nullptr;
	}
	UAsyncTaskSendGameResults* Task = NewObject<UAsyncTaskSendGameResults>(WorldContextObject);
	Task->GameMode = Cast<ATappyFowlGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	Task->RegisterWithGameInstance(WorldContextObject);
	return Task;
}
