// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"

#include "Protocol.pb.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
	Protocol::GC_CHAT pkt;
	pkt.set_msg("Hello world!");
	
	UE_LOG(LogTemp, Display, TEXT("%s"), *FString(UTF8_TO_TCHAR(pkt.msg().c_str())));
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

