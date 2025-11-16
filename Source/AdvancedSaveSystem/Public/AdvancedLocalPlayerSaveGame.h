// Copyright (c) 2025 Vimal P Sojan
// Licensed under the MIT License. See LICENSE file in the project root.

#pragma once

#include "GameFramework/SaveGame.h"
#include "AdvancedLocalPlayerSaveGame.generated.h"

DECLARE_DELEGATE_OneParam(FOnGameSavedNative, bool);

/**
 * 
 */
UCLASS()
class ADVANCEDSAVESYSTEM_API UAdvancedLocalPlayerSaveGame : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	bool AsyncSaveGame(FOnGameSavedNative Delegate);

private:
	FOnGameSavedNative SaveGameDelegate;
};
