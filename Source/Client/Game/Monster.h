// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Monster.generated.h"

namespace Protocol
{
	class ObjectInfo;
	class Position;
}

UCLASS()
class CLIENT_API AMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonster();
	virtual ~AMonster() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure)
	float GetSpeed() const { return Speed; }

	uint64 GetObjectId() const;

	void SetMonsterInfo(const Protocol::ObjectInfo& MonsterInfo_);
	void SetDestInfo(const Protocol::Position& DestInfo_);
	void SetSpeed(float Speed_) { Speed = Speed_; }

protected:
	Protocol::ObjectInfo* ObjectInfo;
	Protocol::Position* DestInfo;
	float Speed;
};
