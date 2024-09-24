// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#include "TappyFowlPlayerController.h"

#include "CharacterDataAsset.h"
#include "TappyFowl.h"
#include "TappyFowlAssetManager.h"
#include "TappyFowlSaveGame.h"
#include "ThirdwebRuntimeSettings.h"
#include "ThirdwebSigner.h"
#include "ThirdwebUtils.h"
#include "ThirdwebWalletHandle.h"

#include "Components/SlateWrapperTypes.h"

#include "Interfaces/IHttpResponse.h"

#include "Kismet/GameplayStatics.h"

#include "ViewModels/PlayerViewModel.h"
#include "ViewModels/ViewModelMacros.h"

void ATappyFowlPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SaveGame = UTappyFowlSaveGame::LoadOrCreateSaveGame(this);
	OnSaveGameSet.Broadcast(SaveGame);
	AutoSignIn();
}

ATappyFowlPlayerController* ATappyFowlPlayerController::Get(const UObject* WorldContextObject, const int32 PlayerIndex)
{
	return Cast<ATappyFowlPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex));
}

void ATappyFowlPlayerController::SignOut()
{
	UTappyFowlSaveGame::DeleteSaveGame(this);
	SaveGame = nullptr;
}

void ATappyFowlPlayerController::UnlockCharacter(const UCharacterDataAsset* NewCharacter)
{
	if (!NewCharacter)
	{
		UE_LOG(LogPlayerController, Error, TEXT("TappyFowlPlayerController::UnlockCharacter::Invalid Character"))
		return;
	}
	if (NewCharacter->bIncluded)
	{
		return;
	}
	if (NewCharacter->Price > SaveGame->GetCoins())
	{
		UE_LOG(LogPlayerController, Error, TEXT("TappyFowlPlayerController::UnlockCharacter::Insufficient Funds"))
		return;
	}
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("account"), SmartWallet.ToAddress());
	JsonObject->SetNumberField(TEXT("id"), NewCharacter->Id);

	const TSharedRef<IHttpRequest> Request = TappyFowl::HTTP::Post(TEXT("/purchase"), JsonObject);
	Request->OnProcessRequestComplete().BindWeakLambda(this, [&, NewCharacter](const FHttpRequestPtr&, const FHttpResponsePtr&, const bool bWasSuccessful)
	{
		if (bWasSuccessful)
		{
			SaveGame->SaveCoins(SaveGame->GetCoins() - NewCharacter->Price);
			SaveGame->AddOwnedCharacter(NewCharacter);
			if (UPlayerViewModel* PlayerViewModel = GetGlobalViewModel<UPlayerViewModel>(this))
			{
				PlayerViewModel->SetSaveGame(SaveGame);
			}
		}
		else
		{
			UE_LOG(LogPlayerController, Error, TEXT("TappyFowlPlayerController::UnlockCharacter::Request Failed"))
		}
	});
	Request->ProcessRequest();
}

UCharacterDataAsset* ATappyFowlPlayerController::GetSelectedCharacter() const
{
	return SaveGame ? SaveGame->GetSelectedCharacter() : UTappyFowlAssetManager::GetDefaultCharacter();
}

void ATappyFowlPlayerController::SetSelectedCharacter(UCharacterDataAsset* NewCharacter)
{
	if (NewCharacter)
	{
		SaveGame->SaveSelectedCharacter(NewCharacter);
	}
}

TArray<UCharacterDataAsset*> ATappyFowlPlayerController::GetOwnedCharacters() const
{
	return SaveGame->GetOwnedCharacters();
}

int32 ATappyFowlPlayerController::GetCoins() const
{
	return SaveGame->GetCoins();
}

int32 ATappyFowlPlayerController::GetHighScore() const
{
	return SaveGame->GetHighScore();
}

void ATappyFowlPlayerController::SetHighScore(const int32 NewHighScore)
{
	SaveGame->SaveHighScore(NewHighScore);
}

void ATappyFowlPlayerController::AddCoins(const int32 NewCoins)
{
	SaveGame->SaveCoins(SaveGame->GetCoins() + NewCoins);
}

bool ATappyFowlPlayerController::HasCharacter(const UCharacterDataAsset* InCharacter) const
{
	if (!InCharacter)
	{
		return false;
	}
	// ReSharper disable once CppTooWideScopeInitStatement
	TArray<UCharacterDataAsset*> Characters = GetOwnedCharacters();
	for (UCharacterDataAsset* CharacterDataAsset : Characters)
	{
		if (CharacterDataAsset->Id == InCharacter->Id)
		{
			return true;
		}
	}

	return false;
}

FWalletHandle ATappyFowlPlayerController::SetInAppWallet(const FWalletHandle& Wallet, const EThirdwebOAuthProvider Provider)
{
	InAppWallet = Wallet;
	SaveGame->SaveAuthProvider(Provider);
	CreateSmartWallet();
	return InAppWallet;
}

bool ATappyFowlPlayerController::AutoSignIn()
{
	if (SaveGame->IsLoggedIn())
	{
		if (!SaveGame->GetEmail().IsEmpty())
		{
			if (FString Error; !FWalletHandle::CreateInAppEmailWallet(SaveGame->GetEmail(), InAppWallet, Error))
			{
				UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::BeginPlay::%s"), *Error);
				return false;
			}
			UE_LOG(LogPlayerController, Log, TEXT("TappyFowlPlayerController::AutoSignIn::Email InAppWallet Created"))
		}
		else
		{
			if (FString Error; !FWalletHandle::CreateInAppOAuthWallet(SaveGame->GetOAuthProvider(), InAppWallet, Error))
			{
				UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::BeginPlay::%s"), *Error);
				return false;
			}
			UE_LOG(LogPlayerController, Log, TEXT("TappyFowlPlayerController::AutoSignIn::OAuth (%s) InAppWallet Created"), *ThirdwebUtils::ToString(SaveGame->GetOAuthProvider()))
		}
		CreateSmartWallet();
	}
	return true;
}

bool ATappyFowlPlayerController::CreateSmartWallet()
{
	if (InAppWallet.IsValid())
	{
		if (FString Error; !InAppWallet.CreateSmartWallet(421614, true, TEXT(""), TEXT(""), SmartWallet, Error))
		{
			UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::BeginPlay::%s"), *Error);
			return false;
		}
		UE_LOG(LogPlayerController, Log, TEXT("TappyFowlPlayerController::CreateSmartWallet::Smart Wallet Created"))

		bool bDeployed = false;
		if (FString Error; !SmartWallet.IsDeployed(bDeployed, Error))
		{
			UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::BeginPlay::%s"), *Error);
			return false;
		}

		bool bGlobalEngineSignerFound;
		FString EngineSigner = UThirdwebRuntimeSettings::GetThirdwebGlobalEngineSigner(bGlobalEngineSignerFound);

		bool bIsActiveSigner = false;
		if (bDeployed)
		{
			TArray<FSigner> Signers;
			if (FString Error; !SmartWallet.GetActiveSigners(Signers, Error))
			{
				UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::BeginPlay::%s"), *Error);
			}
			else
			{
				for (FSigner& Signer : Signers)
				{
					if (Signer.Address.Equals(EngineSigner, ESearchCase::IgnoreCase))
					{
						bIsActiveSigner = true;
					}
				}
			}
		}
		if (!bDeployed || !bIsActiveSigner || !InAppWallet.IsConnected())
		{
			UE_LOG(LogPlayerController, Log, TEXT("TappyFowlPlayerController::CreateSmartWallet::%s Smart Wallet"),
			       bDeployed ? bIsActiveSigner ? TEXT("Connecting") : TEXT("Enabling") : TEXT("Deploying"));
			FDateTime Mv = FDateTime::MinValue();
			FString TxHash;
			if (FString Error; !SmartWallet.CreateSessionKey(EngineSigner, {}, TEXT("0"), Mv, Mv, Mv, Mv, TxHash, Error))
			{
				UE_LOG(LogPlayerController, Error, TEXT("TappyFowlPlayerController::BeginPlay::%s"), *Error);
				return false;
			}
			UE_LOG(LogPlayerController, Log, TEXT("TappyFowlPlayerController::CreateSmartWallet::Smart Wallet Session Key Created - TX: %s"), *TxHash);
		}
		else UE_LOG(LogPlayerController, Log, TEXT("TappyFowlPlayerController::CreateSmartWallet::Smart Wallet Deployed"));
		OnLoggedIn.Broadcast(SmartWallet.ToAddress());
	}
	else UE_LOG(LogPlayerController, Error, TEXT("TappyFowlPlayerController::CreateSmartWallet::In App Wallet Invalid"));
	FetchBalances();
	return true;
}

void ATappyFowlPlayerController::FetchBalances()
{
	if (!SmartWallet.IsValid())
	{
		return;
	}
	FHttpModule& HttpModule = FHttpModule::Get();
	const TSharedRef<IHttpRequest> Request = TappyFowl::HTTP::Get(FString::Format(TEXT("/{0}/balances"), {SmartWallet.ToAddress()}));
	Request->OnProcessRequestComplete().BindWeakLambda(this, [&](const FHttpRequestPtr&, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (bWasSuccessful)
		{
			UE_LOG(LogPlayerController, Log, TEXT("TappyFowlPlayerController::FetchBalances::%s"), *Response->GetContentAsString())
			UPlayerViewModel* PlayerViewModel = GetGlobalViewModel<UPlayerViewModel>(this);
			const TSharedPtr<FJsonObject> JsonObject = TappyFowl::JSON::StringToJsonObject(Response->GetContentAsString());
			if (JsonObject->HasTypedField<EJson::String>(TEXT("tokens")))
			{
				SaveGame->SaveCoins(FCString::Atoi(*JsonObject->GetStringField(TEXT("tokens"))));
			}
			if (JsonObject->HasTypedField<EJson::String>(TEXT("name")))
			{
				// PlayerViewModel->SetEnsName(JsonObject->GetStringField(TEXT("name")))
			}
			if (JsonObject->HasTypedField<EJson::Array>(TEXT("characters")))
			{
				TArray<TSharedPtr<FJsonValue>> CharacterIds = JsonObject->GetArrayField(TEXT("characters"));
				for (int i = 0; i < CharacterIds.Num(); i++)
				{
					if (CharacterIds[i]->Type == EJson::Number && CharacterIds[i]->AsNumber() > 0)
					{
						SaveGame->AddOwnedCharacter(UTappyFowlAssetManager::GetCharacterDataAssetByTokenId(i));
					}
				}
			}
			PlayerViewModel->SetSaveGame(SaveGame);
		}
		else
		{
			UE_LOG(LogPlayerController, Error, TEXT("TappyFowlPlayerController::FetchBalances::Request Failed"))
		}
	});
	Request->ProcessRequest();
}
