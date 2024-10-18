// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#include "TappyFowlPlayerController.h"

#include <Containers/ThirdwebLinkedAccount.h>

#include "CharacterDataAsset.h"
#include "TappyFowl.h"
#include "TappyFowlAssetManager.h"
#include "TappyFowlGameInstance.h"
#include "TappyFowlSaveGame.h"
#include "ThirdwebRuntimeSettings.h"
#include "ThirdwebUtils.h"

#include "Components/SlateWrapperTypes.h"

#include "Containers/ThirdwebSigner.h"

#include "Interfaces/IHttpResponse.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

#include "ViewModels/PlayerViewModel.h"
#include "ViewModels/ViewModelMacros.h"

#define CREATE_ERROR_DELEGATE(FunctionName) \
	FStringDelegate::CreateWeakLambda(this, [](const FString& Error) { \
	  UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::%s::%s"), FunctionName, *Error); \
	})

void ATappyFowlPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SaveGame = UTappyFowlSaveGame::LoadOrCreateSaveGame(this);
	OnSaveGameSet.Broadcast(SaveGame);
	AutoSignIn();
}

FInAppWalletHandle ATappyFowlPlayerController::GetInAppWallet() const
{
	if (UTappyFowlGameInstance* GameInstance = UTappyFowlGameInstance::Get(this))
	{
		return GameInstance->GetInAppWallet();
	}
	UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::GetInAppWallet::GameInstance Invalid"))
	return FInAppWalletHandle();
}

TArray<EThirdwebOAuthProvider> ATappyFowlPlayerController::GetLinkedOAuthProviders() const
{
	TArray<EThirdwebOAuthProvider> LinkedOAuthProviders;
	if (SaveGame)
	{
		// ReSharper disable once CppTooWideScopeInitStatement
		const TArray<FThirdwebLinkedAccount> LinkedAccounts = SaveGame->GetLinkedAccounts();
		for (const FThirdwebLinkedAccount& LinkedAccount : LinkedAccounts)
		{
			if (EThirdwebOAuthProvider Provider = ThirdwebUtils::ToOAuthProvider(LinkedAccount.Type); Provider != EThirdwebOAuthProvider::None)
			{
				LinkedOAuthProviders.Emplace(Provider);
			}
		}
	}
	return LinkedOAuthProviders;
}

FSmartWalletHandle ATappyFowlPlayerController::GetSmartWallet() const
{
	if (UTappyFowlGameInstance* GameInstance = UTappyFowlGameInstance::Get(this))
	{
		return GameInstance->GetSmartWallet();
	}
	UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::GetSmartWallet::GameInstance Invalid"))
	return FSmartWalletHandle();
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
		UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::UnlockCharacter::Invalid Character"))
		return;
	}
	if (NewCharacter->bIncluded)
	{
		return;
	}
	if (NewCharacter->Price > SaveGame->GetCoins())
	{
		UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::UnlockCharacter::Insufficient Funds"))
		return;
	}
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("account"), GetSmartWallet().ToAddress());
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
			UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::UnlockCharacter::Request Failed"))
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

FInAppWalletHandle ATappyFowlPlayerController::SetInAppWallet(const FInAppWalletHandle& Wallet)
{
	if (Wallet.IsValid() && Wallet != GetInAppWallet())
	{
		UTappyFowlGameInstance::Get(this)->SetInAppWallet(Wallet);
		CreateSmartWallet();
	}
	return GetInAppWallet();
}

bool ATappyFowlPlayerController::AutoSignIn()
{
	UE_LOG(LogPlayerController, Log, TEXT("ATappyFowlPlayerController::AutoSignIn::Called"))
	if (!GetInAppWallet().IsValid())
	{
		FInAppWalletHandle::CreateEcosystemGuestWallet(
			TEXT(""),
			FInAppWalletHandle::FCreateInAppWalletDelegate::CreateUObject(this, &ThisClass::HandleInAppWalletCreated),
			CREATE_ERROR_DELEGATE(TEXT("AutoSignIn"))
		);
	} else UE_LOG(LogPlayerController, Log, TEXT("ATappyFowlPlayerController::AutoSignIn::Guest Wallet Already Exists"));
	return true;
}

bool ATappyFowlPlayerController::CreateSmartWallet()
{
	if (!IsInGameThread())
	{
		TWeakObjectPtr<ATappyFowlPlayerController> WeakThis = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->CreateSmartWallet();
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);
		return true;
	}
	if (GetInAppWallet().IsValid())
	{
		FSmartWalletHandle::Create(
			GetInAppWallet(),
			421614,
			true,
			TEXT(""),
			TEXT(""),
			FSmartWalletHandle::FCreateSmartWalletDelegate::CreateUObject(this, &ThisClass::HandleSmartWalletCreated),
			CREATE_ERROR_DELEGATE(TEXT("CreateSmartWallet"))
		);
	}
	else UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::CreateSmartWallet::In App Wallet Invalid"));

	return true;
}

void ATappyFowlPlayerController::CreateSessionKey(const FString& EngineSigner)
{
	if (!IsInGameThread())
	{
		TWeakObjectPtr<ATappyFowlPlayerController> WeakThis = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, EngineSigner]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->CreateSessionKey(EngineSigner);
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);
		return;
	}
	GetSmartWallet().CreateSessionKey(
		EngineSigner,
		{},
		TEXT("0"),
		FDateTime::MinValue(),
		FDateTime::MinValue(),
		FDateTime::MinValue(),
		FDateTime::MinValue(),
		FStringDelegate::CreateUObject(this, &ATappyFowlPlayerController::HandleSessionKeyCreated),
		CREATE_ERROR_DELEGATE(TEXT("HandleSmartWalletIsDeployed"))
	);
}

void ATappyFowlPlayerController::FetchBalances()
{
	if (!IsInGameThread())
	{
		TWeakObjectPtr<ATappyFowlPlayerController> WeakThis = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->FetchBalances();
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);
		return;
	}
	if (!GetSmartWallet().IsValid())
	{
		return;
	}
	FHttpModule& HttpModule = FHttpModule::Get();
	const TSharedRef<IHttpRequest> Request = TappyFowl::HTTP::Get(FString::Format(TEXT("/{0}/balances"), {GetSmartWallet().ToAddress()}));
	Request->OnProcessRequestComplete().BindWeakLambda(this, [&](const FHttpRequestPtr&, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (bWasSuccessful)
		{
			UE_LOG(LogPlayerController, Log, TEXT("ATappyFowlPlayerController::FetchBalances::%s"), *Response->GetContentAsString())
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
		}
		else
		{
			UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::FetchBalances::Request Failed"))
		}
	});
	Request->ProcessRequest();
}

void ATappyFowlPlayerController::HandleInAppWalletCreated(const FInAppWalletHandle& Wallet)
{
	UE_LOG(LogPlayerController, Log, TEXT("ATappyFowlPlayerController::HandleInAppWalletCreated::Called"))
	if (!IsInGameThread())
	{
		UE_LOG(LogPlayerController, Log, TEXT("ATappyFowlPlayerController::HandleInAppWalletCreated::Rescheduling on Game Thread"))
		TWeakObjectPtr<ATappyFowlPlayerController> WeakThis = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, Wallet]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->HandleInAppWalletCreated(Wallet);
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);
		return;
	}
	UE_LOG(LogPlayerController, Log, TEXT("ATappyFowlPlayerController::HandleInAppWalletCreated::WalletIsValid=%s"), Wallet.IsValid() ? TEXT("true") : TEXT("false"))
	UTappyFowlGameInstance::Get(this)->SetInAppWallet(Wallet);
	UE_LOG(LogPlayerController, Log, TEXT("ATappyFowlPlayerController::HandleInAppWalletCreated::Wallet=%s-%lld"), GetInAppWallet().GetSourceString(), GetInAppWallet().GetID())
	FInAppWalletHandle Handle = GetInAppWallet();
	Handle.SignInWithGuest(
		FStreamableDelegate::CreateWeakLambda(this, [this, Handle]()
		{
			UTappyFowlGameInstance::Get(this)->SetInAppWallet(Handle);
			UE_LOG(LogPlayerController, Log, TEXT("ATappyFowlPlayerController::AutoSignIn::Signed In with Guest"))
			GetInAppWallet().GetLinkedAccounts(
	FInAppWalletHandle::FGetLinkedAccountsDelegate::CreateUObject(this, &ATappyFowlPlayerController::HandleFetchLinkedAccounts),
	CREATE_ERROR_DELEGATE(TEXT("HandleSessionKeyCreated")));
			CreateSmartWallet();
		}), CREATE_ERROR_DELEGATE(TEXT("HandleInAppWalletCreated::SignInWithGuest"))
	);
}

void ATappyFowlPlayerController::HandleSmartWalletCreated(const FSmartWalletHandle& Wallet)
{
	if (!IsInGameThread())
	{
		TWeakObjectPtr<ATappyFowlPlayerController> WeakThis = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, Wallet]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->HandleSmartWalletCreated(Wallet);
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);
		return;
	}
	if (Wallet.IsValid())
	{
		UTappyFowlGameInstance::Get(this)->SetSmartWallet(Wallet);
		GetSmartWallet().IsDeployed(
			FBoolDelegate::CreateUObject(this, &ThisClass::HandleSmartWalletIsDeployed),
			CREATE_ERROR_DELEGATE(TEXT("HandleSmartWalletCreated"))
		);
	}
	else UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::HandleSmartWalletCreated::Invalid Wallet Handle..."));
}

void ATappyFowlPlayerController::HandleSmartWalletIsDeployed(const bool& bIsDeployed)
{
	if (!IsInGameThread())
	{
		TWeakObjectPtr<ATappyFowlPlayerController> WeakThis = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, bIsDeployed]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->HandleSmartWalletIsDeployed(bIsDeployed);
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);
		return;
	}

	bool bFound;
	FString EngineSigner = UThirdwebRuntimeSettings::GetThirdwebGlobalEngineSigner(bFound);
	UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::HandleSmartWalletIsDeployed::EngineSigner=%s|Deployed=%s"), *EngineSigner, bIsDeployed ? TEXT("true") : TEXT("false"));
	if (bIsDeployed)
	{
		GetSmartWallet().GetActiveSigners(
			FSmartWalletHandle::FGetActiveSignersDelegate::CreateWeakLambda(this, [this, EngineSigner](const TArray<FSigner>& Signers)
			{
				TArray<FString> Addresses;
				bool bIsActiveSigner = false;
				for (const FSigner& Signer : Signers)
				{
					Addresses.Emplace(Signer.Address);
					if (Signer.Address.Equals(EngineSigner, ESearchCase::IgnoreCase))
					{
						bIsActiveSigner = true;
						break;
					}
				}
				UE_LOG(LogPlayerController, Error, TEXT("ATappyFowlPlayerController::HandleSmartWalletIsDeployed::Signers=%s|IsActiveSigner=%s"), *UKismetStringLibrary::JoinStringArray(Addresses, TEXT("")), bIsActiveSigner ? TEXT("true") : TEXT("false"));
				if (!bIsActiveSigner || !GetInAppWallet().IsConnected())
				{
					UE_LOG(LogPlayerController, Log, TEXT("ATappyFowlPlayerController::CreateSmartWallet::%s Smart Wallet"), bIsActiveSigner ? TEXT("Connecting") : TEXT("Enabling"));
					CreateSessionKey(EngineSigner);
				}
				else HandleSessionKeyCreated(TEXT("AlreadyConnected"));
			}),
			CREATE_ERROR_DELEGATE(TEXT("HandleSmartWalletIsDeployed"))
		);
	} else
	{
		UE_LOG(LogPlayerController, Log, TEXT("ATappyFowlPlayerController::CreateSmartWallet::Deploying Smart Wallet"));
		CreateSessionKey(EngineSigner);
	}
}

void ATappyFowlPlayerController::HandleSessionKeyCreated(const FString& TxHash)
{
	if (!IsInGameThread())
	{
		TWeakObjectPtr<ATappyFowlPlayerController> WeakThis = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, TxHash]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->HandleSessionKeyCreated(TxHash);
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);
		return;
	}
	UE_LOG(LogPlayerController, Log, TEXT("ATappyFowlPlayerController::HandleSessionKeyCreated::TxHash=%s"), *TxHash);
	UTappyFowlGameInstance::Get(this)->SetSmartWallet(UTappyFowlGameInstance::Get(this)->GetSmartWallet());
	
	OnLoggedIn.Broadcast(GetSmartWallet().ToAddress());
	FetchBalances();
}

void ATappyFowlPlayerController::HandleFetchLinkedAccounts(const TArray<FThirdwebLinkedAccount>& LinkedAccounts)
{
	if (!IsInGameThread())
	{
		TWeakObjectPtr<ATappyFowlPlayerController> WeakThis = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, LinkedAccounts]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->HandleFetchLinkedAccounts(LinkedAccounts);
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);
		return;
	}
	if (SaveGame)
	{
		SaveGame->SaveLinkedAccounts(LinkedAccounts);
		UE_LOG(LogPlayerController, Log, TEXT("ATappyFowlPlayerController::HandleFetchLinkedAccounts::Updated"));
	}
}
