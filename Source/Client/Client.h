// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct PacketHeader
{
	uint16 id;
	uint16 size;
};

class SendBuffer : public TSharedFromThis<SendBuffer>
{
public:
	SendBuffer() = default;
	SendBuffer(BYTE* buffer, uint32 dataLen);
	SendBuffer(uint32 dataLen);//for AppendBuffer
	~SendBuffer();

public: //use at fill buffer
	bool AppendBuffer(BYTE* buffer, uint32 dataLen);
	BYTE* WritePos() { return &_buffer[_writePos]; }

public: //use at write
	BYTE* GetBuffer() { return &_buffer[0]; }
	BYTE* GetPosPtr(uint32 pos) { return &_buffer[pos]; }
	uint32 GetFreeSize() { return _allocSize - _writePos; }
	uint32 GetDataLen() { return _writePos; }
	bool OnWrite(uint32 dataLen);

private:
	uint32			_writePos = 0;
	uint32			_allocSize = 0;
	TArray<BYTE>	_buffer;
};

#define USING_SHARED_PTR(name) using name##Ref = TSharedPtr<name>;

class Session;
class PacketSession;

USING_SHARED_PTR(Session);
USING_SHARED_PTR(PacketSession);
USING_SHARED_PTR(SendBuffer);