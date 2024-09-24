// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"

#include "TappyFowlEPPUSettings.generated.h"

/**
 * This class acts as a configuration holder to manage various settings required for the runtime environment in the game.
 */
UCLASS(Config=EditorPerProjectUserSettings, meta=(DisplayName="TappyFowl"))
class TAPPYFOWL_API UTappyFowlEPPUSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** API URL to supplementary API */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category=Api)
	FString ApiUrl;

	/** API Key for write calls to API */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category=Api)
	FString ApiKey;

	/** Chain ID */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category=Api)
	int64 ChainId;

	UFUNCTION(BlueprintPure, Category="TappyFowl", DisplayName="Get TappyFowl Settings")
	static const UTappyFowlEPPUSettings* Get() { return GetDefault<UTappyFowlEPPUSettings>(); }

	bool IsValidConfig() const { return !GetApiUrl().IsEmpty() && !GetApiKey().IsEmpty() && ChainId > 0; }

	FString GetApiUrl() const
	{
		FString Url = ApiUrl.TrimStartAndEnd();
		return Url.EndsWith(TEXT("/")) ? Url.LeftChop(1) : Url;
	}

	FString GetApiKey() const { return ApiKey.TrimStartAndEnd(); }

	FString GetChainId() const { return FString::Printf(TEXT("%lld"), ChainId); }

	FString GetApiChainUrl() const { return FString::Printf(TEXT("%s/%lld"), *GetApiUrl(), ChainId); }
};
