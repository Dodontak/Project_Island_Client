// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "Struct.pb.h"

// Sets default values
AMonster::AMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ObjectInfo = new Protocol::ObjectInfo();
	DestInfo = new Protocol::Position();
}

AMonster::~AMonster()
{
	delete ObjectInfo;
	delete DestInfo;
}

// Called when the game starts or when spawned
void AMonster::BeginPlay()
{
	Super::BeginPlay();
	{
		FVector Location = GetActorLocation();
		DestInfo->set_x(Location.X);
		DestInfo->set_y(Location.Y);
		DestInfo->set_z(Location.Z);
		DestInfo->set_yaw(GetControlRotation().Yaw);
	}
}

// Called every frame
void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//몬스터 움직임 구현
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

// Called to bind functionality to input
void AMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

uint64 AMonster::GetObjectId() const
{
	return ObjectInfo->object_id();
}

void AMonster::SetMonsterInfo(const Protocol::ObjectInfo& MonsterInfo_)
{
	ObjectInfo->CopyFrom(MonsterInfo_);
}

void AMonster::SetDestInfo(const Protocol::Position& DestInfo_)
{
	DestInfo->CopyFrom(DestInfo_);
}

