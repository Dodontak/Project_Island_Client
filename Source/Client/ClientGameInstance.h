// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Client.h"
#include "CharacterListResult.h"
#define UI UI_ST
#include <openssl/ossl_typ.h>
#undef UI
#include "Protocol.pb.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Character.h"
#include "ClientGameInstance.generated.h"

class AClientMyPlayer;
class UCharacterSelectWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSignUpValidCheck, bool, Success, bool, IsEmailSkip, FString, Email,
                                              FString, Reason);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSignUpVerifyCode, bool, Success, FString, Reason);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginToGameServer, bool, Success, FString, Reason);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRequestMyCharacterList, bool, Success, FCharacterListResult,
                                               Characters, FString, Reason);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCheckNicknameAvailability, bool, Success, FString, Reason);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreateNewCharacter, bool, Success, FString, Reason);

class FSocket;
class PacketSession;

UCLASS()
class CLIENT_API UClientGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();
	UFUNCTION(BlueprintCallable)
	void LeaveRoom();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(SendBufferRef SendBuffer);

	UFUNCTION(BlueprintCallable)
	void HandleSpawnMe();
	void HandleSpawn(const Protocol::ObjectInfo& ObjectInfo);

	void HandleDespawn(uint64 ObjectId);
	void HandleDespawn(const Protocol::GS_DESPAWN& DespawnPkt);


	// AuthServerAPI
	UFUNCTION(BlueprintCallable)
	void LoginToAuthServer(FString Id, FString Password);

	UFUNCTION(BlueprintCallable)
	void SignUpValidCheck(bool SkipEmail, FString Email, FString Id, FString Password);

	UFUNCTION(BlueprintCallable)
	void RequestVerifyCode();

	UFUNCTION(BlueprintCallable)
	void SignUpVerifyEmailCode(FString Code);

	// GameServerAPI
	UFUNCTION(BlueprintCallable)
	void RequestMyCharacterList();

	UFUNCTION(BlueprintCallable)
	void HandleMyCharacterListResponse(UUserWidget* WrapBox, UUserWidget* AddNewCharacter,
	                                   TSubclassOf<UUserWidget> CharacterWidgetClass,
	                                   FCharacterListResult Characters);

	UFUNCTION(BlueprintCallable)
	void CheckNicknameAvailability(FString Nickname);

	UFUNCTION(BlueprintCallable)
	void CreateNewCharacter(FString Nickname, FString ClassName);

	UFUNCTION(BlueprintCallable)
	void SelectCharacter(UCharacterSelectWidget* Character);

	UPROPERTY(BlueprintAssignable)
	FOnSignUpValidCheck OnSignUpValidCheck;

	UPROPERTY(BlueprintAssignable)
	FOnSignUpVerifyCode OnSignUpVerifyCode;

	UPROPERTY(BlueprintAssignable)
	FOnLoginToGameServer OnLoginToGameServer;

	UPROPERTY(BlueprintAssignable)
	FOnRequestMyCharacterList OnRequestMyCharacterList;

	UPROPERTY(BlueprintAssignable)
	FOnCheckNicknameAvailability OnCheckNicknameAvailability;

	UPROPERTY(BlueprintAssignable)
	FOnCreateNewCharacter OnCreateNewCharacter;

public:
	SSL_CTX* Ctx = nullptr;
	TSharedPtr<PacketSession> GameServerSession = nullptr;
	TSharedPtr<PacketSession> AuthServerSession = nullptr;

public:
	UFUNCTION(BlueprintPure)
	bool IsConnectToGameServer() { return GameServerSession != nullptr; }

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Image")
	UTexture2D* KnightPortrait;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Image")
	UTexture2D* ArcherPortrait;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Image")
	UTexture2D* MagePortrait;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacter> MyPlayerClass;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacter> OtherPlayerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacter> MonsterSkeletonClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacter> MonsterWerewolfClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacter> MonsterStoneGolemClass;
	
	TMap<uint64, AActor*> Objects;
	AClientMyPlayer* MyPlayer;
	Protocol::ObjectInfo PendingPlayerInfo; // 대기 중인 스폰 정보
	bool bHasPendingSpawn = false;
};
