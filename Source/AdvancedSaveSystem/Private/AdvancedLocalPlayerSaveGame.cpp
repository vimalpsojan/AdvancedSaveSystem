// Copyright (c) 2025 Vimal P Sojan
// Licensed under the MIT License. See LICENSE file in the project root.


#include "AdvancedLocalPlayerSaveGame.h"

#include "Kismet/GameplayStatics.h"

bool UAdvancedLocalPlayerSaveGame::AsyncSaveGame(FOnGameSavedNative Delegate)
{
	// Subclasses can override this to add additional checks like if one is in progress
	if (!ensure(GetLocalPlayer()))
	{
		UE_LOG(LogPlayerManagement, Error, TEXT("AsyncSaveGameToSlotForLocalPlayer called with null local player!"));
		return false;
	}

	int32 RequestUserIndex = GetPlatformUserIndex();
	FString RequestSlotName = GetSaveSlotName();
	if (!ensure(RequestSlotName.Len() > 0))
	{
		UE_LOG(LogPlayerManagement, Error, TEXT("AsyncSaveGameToSlotForLocalPlayer called with empty slot name!"));
		return false;
	}

	HandlePreSave();

	// Start an async save and pass through the current save request count

	UAdvancedLocalPlayerSaveGame* SaveGame = this;
	const FAsyncSaveGameToSlotDelegate Lambda = FAsyncSaveGameToSlotDelegate::CreateWeakLambda(
		GetLocalPlayer(), [SaveGame,Delegate]
	(const FString& SlotName, const int32 UserIndex, bool bIsSuccess)
		{
			SaveGame->ProcessSaveComplete(SlotName, UserIndex, bIsSuccess, SaveGame->CurrentSaveRequest);
			Delegate.ExecuteIfBound(bIsSuccess);
		});

	UGameplayStatics::AsyncSaveGameToSlot(SaveGame, RequestSlotName, RequestUserIndex, Lambda);

	return true;
}
