// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct PacketHeader
{
	uint16 id;
	uint16 size;
};

#define USING_SHARED_PTR(name) using name##Ref = TSharedPtr<class name>;

USING_SHARED_PTR(PacketSession);
USING_SHARED_PTR(SendBuffer);
USING_SHARED_PTR(RecvBuffer);
USING_SHARED_PTR(SslObject);
