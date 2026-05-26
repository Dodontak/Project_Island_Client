// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ClientPlayer.generated.h"

namespace Protocol
{
	class Position;
	class PlayerInfo;
}

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

UCLASS()
class CLIENT_API AClientPlayer : public ACharacter
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	AClientPlayer();
	virtual ~AClientPlayer() override;
	
public:
	virtual void Tick(float DeltaTime) override;
	
public:
	bool IsMyPlayer();
	
	const Protocol::Position& GetPlayerPosition() const;
	UFUNCTION(BlueprintPure)
	float GetSpeed() const { return Speed; }
	uint64 GetObjectId();
	
	void SetPlayerInfo(const Protocol::PlayerInfo& PlayerInfo_);
	void SetDestInfo(const Protocol::Position& DestInfo_);
	void SetSpeed(float Speed_) { Speed = Speed_; }
	
protected:
	Protocol::PlayerInfo* PlayerInfo;
	Protocol::Position* DestInfo;
	float Speed;
};