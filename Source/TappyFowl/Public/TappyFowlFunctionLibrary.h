// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "TappyFowlFunctionLibrary.generated.h"

class UPaperFlipbookComponent;
class UPaperSpriteComponent;
/**
 * 
 */
UCLASS()
class TAPPYFOWL_API UTappyFowlFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject", CallableWithoutWorldContext, Keywords="log warn console", DevelopmentOnly), Category="Development")
	static void PrintDevWarning(const UObject* WorldContextObject, const FText InText = INVTEXT("DEVWARN"));
	
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject", CallableWithoutWorldContext, Keywords="log error console", DevelopmentOnly), Category="Development")
    static void PrintDevError(const UObject* WorldContextObject, const FText InText = INVTEXT("DEVERR"));

	UFUNCTION(BlueprintPure, Category="Utilities|Text")
	static bool IsValidEmail(const FText& Email);

	UFUNCTION(BlueprintPure, meta=(ReturnDisplayName="bChanged"), Category="Utilities|Text")
	static bool OnlyNumeric(const FText& Input, FText& Output);
	
	UFUNCTION(BlueprintCallable, meta=(WorldContext="Actor"))
	static UMaterialInstanceDynamic* CreateActorMaterialInstance(AActor* Actor, UPaperSpriteComponent* Sprite, UPaperFlipbookComponent* Flipbook,  UStaticMeshComponent* Static, USkeletalMeshComponent* Skeletal);
};
