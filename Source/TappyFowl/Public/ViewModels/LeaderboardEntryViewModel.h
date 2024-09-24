// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "MVVMViewModelBase.h"
#include "LeaderboardEntryViewModel.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, DisplayName="Leaderboard Entry ViewModel")
class TAPPYFOWL_API ULeaderboardEntryViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Getter, Setter, FieldNotify)
	FString Address;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Getter, Setter, FieldNotify)
	FText EnsName;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Getter, Setter, FieldNotify)
	float HighScore;

public:
	FString GetAddress() const { return Address; }
	void SetAddress(const FString& Value)
	{
		UE_MVVM_SET_PROPERTY_VALUE(Address, Value);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetDisplayAddress);
	}

	FText GetEnsName() const { return EnsName; }
	void SetEnsName(const FText& Value)
	{
		UE_MVVM_SET_PROPERTY_VALUE(EnsName, Value);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetDisplayAddress);
	}

	float GetHighScore() const { return HighScore; }
	void SetHighScore(const float Value) { UE_MVVM_SET_PROPERTY_VALUE(HighScore, Value); }

	UFUNCTION(BlueprintPure, FieldNotify)
	FText GetDisplayAddress() const;

	static ULeaderboardEntryViewModel* FromJson(UObject* WorldContextObject, const TSharedPtr<FJsonValue>& JsonValue);
	static TArray<ULeaderboardEntryViewModel*> FromJson(UObject* WorldContextObject, const TArray<TSharedPtr<FJsonValue>>& JsonValueArray);
};
