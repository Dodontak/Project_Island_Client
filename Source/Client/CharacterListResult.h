#pragma once

#include "CoreMinimal.h"
#include "Struct.pb.h"
#include "CharacterListResult.generated.h"

USTRUCT(BlueprintType)
struct FCharacterListResult
{
	GENERATED_BODY()

	TArray<Protocol::ObjectInfo> Objects;
};
