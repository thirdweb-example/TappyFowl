// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#include "ViewModels/LeaderboardEntryViewModel.h"

FText ULeaderboardEntryViewModel::GetDisplayAddress() const
{
	if (Address.IsEmpty() || Address.Equals(TEXT("0x0000000000000000000000000000000000000000")))
	{
		return FText::GetEmpty();
	}
	if (!EnsName.IsEmpty())
	{
		return EnsName;
	}
	return FText::Format(FText::FromString(TEXT("{0}...{1}")), FText::FromString(Address.Left(6)), FText::FromString(Address.Right(4)));
}

ULeaderboardEntryViewModel* ULeaderboardEntryViewModel::FromJson(UObject* WorldContextObject, const TSharedPtr<FJsonValue>& JsonValue)
{
	if (JsonValue->Type == EJson::Object)
	{
		ULeaderboardEntryViewModel* LeaderboardEntryViewModel = NewObject<ULeaderboardEntryViewModel>(WorldContextObject);
		TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();
		if (JsonObject->HasTypedField<EJson::String>(TEXT("address")))
		{
			LeaderboardEntryViewModel->SetAddress(JsonObject->GetStringField(TEXT("address")));
		}
		if (JsonObject->HasTypedField<EJson::String>(TEXT("name")))
		{
			// ReSharper disable once CppTooWideScopeInitStatement
			FString InName = JsonObject->GetStringField(TEXT("name"));
			if (!InName.Equals(LeaderboardEntryViewModel->GetAddress()))
			{
				LeaderboardEntryViewModel->SetEnsName(FText::FromString(InName));
			}
		}
		if (JsonObject->HasTypedField<EJson::Number>(TEXT("score")))
		{
			LeaderboardEntryViewModel->SetHighScore(JsonObject->GetNumberField(TEXT("score")));
		}
		return LeaderboardEntryViewModel;
	}
	return nullptr;
}

TArray<ULeaderboardEntryViewModel*> ULeaderboardEntryViewModel::FromJson(UObject* WorldContextObject, const TArray<TSharedPtr<FJsonValue>>& JsonValueArray)
{
	TArray<ULeaderboardEntryViewModel*> LeaderboardEntryViewModels;
	for (int32 i = 0; i < JsonValueArray.Num(); i++)
	{
		if (ULeaderboardEntryViewModel* LeaderboardEntryViewModel = FromJson(WorldContextObject, JsonValueArray[i]))
		{
			LeaderboardEntryViewModels.Emplace(LeaderboardEntryViewModel);
		}
	}
	return LeaderboardEntryViewModels;
}
