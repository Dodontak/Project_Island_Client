// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ClientPlayer.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "ClientMyPlayer.h"
#include "Struct.pb.h"


AClientPlayer::AClientPlayer()
{
	// Set size for collision capsule
	// GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	ObjectInfo = new Protocol::ObjectInfo();
	DestInfo = new Protocol::Position();
}

AClientPlayer::~AClientPlayer()
{
	delete ObjectInfo;
	delete DestInfo;
	ObjectInfo = nullptr;
	DestInfo = nullptr;
}

void AClientPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsMyPlayer() == false)
	{
		FVector Location = GetActorLocation();
		FVector DestLocation = FVector(DestInfo->x(), DestInfo->y(), DestInfo->z());
		FRotator DestRotation = FRotator(DestInfo->pitch(), DestInfo->yaw(), DestInfo->roll());
    
		// Location 보간
		FVector MoveDir = (DestLocation - Location);
		const float DistToDest = MoveDir.Length();
		MoveDir.Normalize();
    
		float MoveDist = (MoveDir * FMath::Max(Speed, 450.0f) * DeltaTime).Length();
		MoveDist = FMath::Min(MoveDist, DistToDest);
		SetActorLocation(Location + MoveDir * MoveDist);

		// Rotation 보간
		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DestRotation, DeltaTime, 10.0f);
		SetActorRotation(NewRotation);
	}
}

void AClientPlayer::SetPlayerInfo(const Protocol::ObjectInfo& PlayerInfo_)
{
	ObjectInfo->CopyFrom(PlayerInfo_);
}

void AClientPlayer::SetDestInfo(const Protocol::Position& DestInfo_)
{
	DestInfo->CopyFrom(DestInfo_);
}

const Protocol::Position& AClientPlayer::GetPlayerPosition() const
{
	return ObjectInfo->pos();
}

uint64 AClientPlayer::GetObjectId() const
{
	if (ObjectInfo == nullptr)
		return 0;
	return ObjectInfo->object_id();
}

bool AClientPlayer::IsMyPlayer()
{
	return Cast<AClientMyPlayer>(this) != nullptr;
}
