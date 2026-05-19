// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectWidget.generated.h"

UCLASS()
class CLIENT_API UCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	int32 GetIndex() const { return Index; }
	UFUNCTION(BlueprintPure)
	FString GetNickname() { return Nickname; }
	
public:
	int32 Index;
	FString Nickname;
};
