// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"

#include "AsyncActionSendGameResults.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup="TappyFowl")
class TAPPYFOWL_API UAsyncTaskSendGameResults : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="TappyFowl")
	static UAsyncTaskSendGameResults* SendGameResults(UObject* WorldContextObject);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResultDelegate);

	UPROPERTY(BlueprintAssignable)
	FResultDelegate Success;

	UPROPERTY(BlueprintAssignable)
	FResultDelegate Failed;

protected:
	TObjectPtr<class ATappyFowlGameMode> GameMode = nullptr;

private:
	UFUNCTION()
	void HandleSuccess()
	{
		UE_LOG(LogTemp, Log, TEXT("AsyncTaskSendGameResults::HandleSuccess::Results Sent!"))
		Success.Broadcast();
		SetReadyToDestroy();
	}

	UFUNCTION()
	void HandleFailed()
	{
		Failed.Broadcast();
		SetReadyToDestroy();
	}
};
