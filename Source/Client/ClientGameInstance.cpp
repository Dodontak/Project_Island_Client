// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientGameInstance.h"

// OpenSSL의 UI 구조체 이름을 잠시 바꿔서 언리얼의 namespace UI와 충돌을 피함
#define UI UI_ST
#include <openssl/ssl.h>
#undef UI
// --- OpenSSL 충돌 방지 처리 끝 ---

#include "PacketSession.h"
#include "Sockets.h"
#include "NetworkWorker.h"
#include "ServerPacketHandler.h"
#include "AuthSession.h"
#include "GameSession.h"
#include "CharacterListResult.h"
#include "CharacterSelectWidget.h"
#include "Components/WrapBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Game/ClientMyPlayer.h"
#include "Game/Monster.h"

void UClientGameInstance::Init()
{
	const SSL_METHOD* Method = TLS_client_method();
	Ctx = SSL_CTX_new(Method);
	ServerPacketHandler::Init();
}

void UClientGameInstance::Shutdown()
{
	Super::Shutdown();

	if (GameServerSession)
	{
		// 워커 스레드 먼저 종료
		if (GameServerSession->RecvWorkerThread)
		{
			GameServerSession->RecvWorkerThread->Destroy();
		}
		if (GameServerSession->SendWorkerThread)
		{
			GameServerSession->SendWorkerThread->Destroy();
		}

		// 소켓 닫기
		if (GameServerSession->Socket)
		{
			GameServerSession->Socket->Close();
		}

		GameServerSession = nullptr;
	}
	if (AuthServerSession)
	{
		// 워커 스레드 먼저 종료
		if (AuthServerSession->RecvWorkerThread)
		{
			AuthServerSession->RecvWorkerThread->Destroy();
		}
		if (AuthServerSession->SendWorkerThread)
		{
			AuthServerSession->SendWorkerThread->Destroy();
		}

		// 소켓 닫기
		if (AuthServerSession->Socket)
		{
			AuthServerSession->Socket->Close();
		}

		AuthServerSession = nullptr;
	}
}

void UClientGameInstance::ConnectToGameServer()
{
	AuthServerSession = MakeShared<AuthSession>("192.168.0.39", 4242, Ctx);
	AuthServerSession->ConnectToGameServer();
}

void UClientGameInstance::DisconnectFromGameServer()
{
}

void UClientGameInstance::LeaveRoom()
{
	if (GameServerSession == nullptr)
		return;

	Protocol::GC_LEAVE_ROOM LeaveRoomPkt;
	GameServerSession->SendPacket(ServerPacketHandler::MakeSendBuffer(LeaveRoomPkt));
}

void UClientGameInstance::HandleRecvPackets()
{
	if (GameServerSession != nullptr)
		GameServerSession->HandleRecvPackets();
	if (AuthServerSession != nullptr)
		AuthServerSession->HandleRecvPackets();
}

void UClientGameInstance::SendPacket(SendBufferRef SendBuffer)
{
	if (GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}

/*----------------------------------------------------------------------------*\
|                                                                              |
|                               AuthServerAPI                                  |
|                                                                              |
\*----------------------------------------------------------------------------*/

void UClientGameInstance::LoginToAuthServer(FString Id, FString Password)
{
	TSharedPtr<AuthSession> AuthServerSessionRef = StaticCastSharedPtr<AuthSession>(AuthServerSession);
	if (AuthServerSessionRef == nullptr)
		return;
	AuthServerSessionRef->LoginToAuthServer(Id, Password);
}

void UClientGameInstance::SignUpValidCheck(bool SkipEmail, FString Email, FString Id, FString Password)
{
	TSharedPtr<AuthSession> AuthServerSessionRef = StaticCastSharedPtr<AuthSession>(AuthServerSession);
	if (AuthServerSessionRef == nullptr)
		return;
	AuthServerSessionRef->SignUpValidCheck(SkipEmail, Email, Id, Password);
}

void UClientGameInstance::RequestVerifyCode()
{
	TSharedPtr<AuthSession> AuthServerSessionRef = StaticCastSharedPtr<AuthSession>(AuthServerSession);
	if (AuthServerSessionRef == nullptr)
		return;
	AuthServerSessionRef->RequestVerifyCode();
}

void UClientGameInstance::SignUpVerifyEmailCode(FString Code)
{
	TSharedPtr<AuthSession> AuthServerSessionRef = StaticCastSharedPtr<AuthSession>(AuthServerSession);
	if (AuthServerSessionRef == nullptr)
		return;
	AuthServerSessionRef->SignUpVerifyEmailCode(Code);
}

/*----------------------------------------------------------------------------*\
|                                                                              |
|                               GameServerAPI                                  |
|                                                                              |
\*----------------------------------------------------------------------------*/

void UClientGameInstance::RequestMyCharacterList()
{
	TSharedPtr<GameSession> GameServerRef = StaticCastSharedPtr<GameSession>(GameServerSession);
	if (GameServerRef == nullptr)
		return;
	GameServerRef->RequestMyCharacterList();
}

void UClientGameInstance::HandleMyCharacterListResponse(UUserWidget* WrapBox, UUserWidget* AddNewCharacter,
                                                        TSubclassOf<UUserWidget> CharacterWidgetClass,
                                                        FCharacterListResult Characters)
{
	UWrapBox* Box = Cast<UWrapBox>(WrapBox->GetWidgetFromName(TEXT("CharacterListWrapBox")));
	if (Box == nullptr)
		return;

	for (int32 Index = 0; Index < Characters.Objects.Num(); Index++)
	{
		UCharacterSelectWidget* CharacterWidget =
			CreateWidget<UCharacterSelectWidget>(GetWorld(), CharacterWidgetClass);
		if (CharacterWidget == nullptr) continue;
		UImage* PortraitImage = Cast<UImage>(CharacterWidget->GetWidgetFromName(TEXT("Portrait")));
		if (PortraitImage == nullptr) continue;

		switch (Characters.Objects[Index].template_id() & 0x000000ff)
		{
		case Protocol::PLAYER_TYPE_ARCHER:
			PortraitImage->SetBrushFromTexture(ArcherPortrait);
			break;
		case Protocol::PLAYER_TYPE_KNIGHT:
			PortraitImage->SetBrushFromTexture(KnightPortrait);
			break;
		case Protocol::PLAYER_TYPE_MAGE:
			PortraitImage->SetBrushFromTexture(MagePortrait);
			break;
		default:
			break;
		}
		UTextBlock* Nickname = Cast<UTextBlock>(CharacterWidget->GetWidgetFromName(TEXT("NickName")));
		if (Nickname)
			Nickname->SetText(FText::FromString(Characters.Objects[Index].name().c_str()));
		CharacterWidget->Index = Index;
		CharacterWidget->Nickname = Nickname->GetText().ToString();;
		Box->AddChildToWrapBox(CharacterWidget);
	}
	Box->AddChildToWrapBox(AddNewCharacter);
}

void UClientGameInstance::CheckNicknameAvailability(FString Nickname)
{
	Protocol::GC_CHECK_NICKNAME Pkt;

	Pkt.set_nickname(TCHAR_TO_UTF8(*Nickname));
	GameServerSession->SendPacket(ServerPacketHandler::MakeSendBuffer(Pkt));
}

void UClientGameInstance::CreateNewCharacter(FString Nickname, FString ClassName)
{
	Protocol::GC_CREATE_CHARACTER Pkt;

	Pkt.set_nickname(TCHAR_TO_UTF8(*Nickname));
	if (ClassName.Equals(TEXT("Knight")))
		Pkt.set_type(Protocol::PLAYER_TYPE_KNIGHT);
	else if (ClassName.Equals(TEXT("Mage")))
		Pkt.set_type(Protocol::PLAYER_TYPE_MAGE);
	else if (ClassName.Equals(TEXT("Archer")))
		Pkt.set_type(Protocol::PLAYER_TYPE_ARCHER);
	else
		Pkt.set_type(Protocol::PLAYER_TYPE_NONE);
	GameServerSession->SendPacket(ServerPacketHandler::MakeSendBuffer(Pkt));
}

void UClientGameInstance::SelectCharacter(UCharacterSelectWidget* Character)
{
	Protocol::GC_ENTER_ROOM Pkt;

	Pkt.set_character_index(Character->GetIndex());
	Pkt.set_room_id(0);
	GameServerSession->SendPacket(ServerPacketHandler::MakeSendBuffer(Pkt));
}

void UClientGameInstance::HandleSpawnMe()
{
	if (bHasPendingSpawn == false)
		return;
	bHasPendingSpawn = false;

	if (GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectId = PendingPlayerInfo.object_id();

	if (Objects.Find(ObjectId) != nullptr)
		return;

	FVector SpawnLocation(PendingPlayerInfo.pos().x(), PendingPlayerInfo.pos().y(), PendingPlayerInfo.pos().z());
	AClientMyPlayer* SpawnedCharacter = World->SpawnActor<AClientMyPlayer>(
		MyPlayerClass, SpawnLocation, FRotator::ZeroRotator);

	SpawnedCharacter->SetPlayerInfo(PendingPlayerInfo);

	if (UClientGameInstance* GI = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
	{
		GI->MyPlayer = SpawnedCharacter;
	}

	Objects.Add(PendingPlayerInfo.object_id(), SpawnedCharacter);

	APlayerController* PC = World->GetFirstPlayerController();
	if (PC)
	{
		PC->Possess(SpawnedCharacter);
	}
}

void UClientGameInstance::HandleSpawn(const Protocol::ObjectInfo& ObjectInfo)
{
	if (GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;
	FVector SpawnLocation(ObjectInfo.pos().x(), ObjectInfo.pos().y(), 200);
	ACharacter* SpawnedPawn;
	switch ((ObjectInfo.template_id() & 0x0000FF00) >> 8)
	{	
	case Protocol::CreatureType::CREATURE_TYPE_MONSTER:
		switch (ObjectInfo.template_id() & 0x000000FF)
		{
		case Protocol::MonsterType::MONSTER_TYPE_SKELETON:
			SpawnedPawn = World->SpawnActor<ACharacter>(MonsterSkeletonClass, SpawnLocation, FRotator::ZeroRotator);
			break;
		case Protocol::MonsterType::MONSTER_TYPE_WEREWOLF:
			SpawnedPawn = World->SpawnActor<ACharacter>(MonsterWerewolfClass, SpawnLocation, FRotator::ZeroRotator);
			break;
		case Protocol::MonsterType::MONSTER_TYPE_STONEGOLEM:
			SpawnedPawn = World->SpawnActor<ACharacter>(MonsterStoneGolemClass, SpawnLocation, FRotator::ZeroRotator);
			break;
		default:
			SpawnedPawn = World->SpawnActor<ACharacter>(MonsterSkeletonClass, SpawnLocation, FRotator::ZeroRotator);
			break;
		}
		break;
	case Protocol::CreatureType::CREATURE_TYPE_PLAYER:
		SpawnedPawn = World->SpawnActor<ACharacter>(OtherPlayerClass, SpawnLocation, FRotator::ZeroRotator);
		break;
	default:
		SpawnedPawn = World->SpawnActor<ACharacter>(MonsterSkeletonClass, SpawnLocation, FRotator::ZeroRotator);
		break;
	}

	Objects.Add(ObjectInfo.object_id(), SpawnedPawn);
}

void UClientGameInstance::HandleDespawn(uint64 ObjectId)
{
	if (GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	AActor** FindActor = Objects.Find(ObjectId);
	if (FindActor == nullptr)
		return;

	World->DestroyActor(*FindActor);
}

void UClientGameInstance::HandleDespawn(const Protocol::GS_DESPAWN& DespawnPkt)
{
	for (auto& ObjectId : DespawnPkt.object_ids())
	{
		HandleDespawn(ObjectId);
	}
}
