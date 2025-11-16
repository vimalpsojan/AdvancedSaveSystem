// Copyright (c) 2025 Vimal P Sojan
// Licensed under the MIT License. See LICENSE file in the project root.


#include "SaveGameSubsystem.h"

#include "AdvancedLocalPlayerSaveGame.h"

FString USaveGameSubsystem::DefaultSaveName(TEXT("DefaultSaveGame"));

USaveGameSubsystem::USaveGameSubsystem()
{
	SaveGameClass = UAdvancedLocalPlayerSaveGame::StaticClass();
	CurrentSaveSate = EGameSavedStates::Init;
}

void USaveGameSubsystem::UserInitialized()
{
	InitSaveGame();
}

FDelegateHandle USaveGameSubsystem::CallOrRegister_OnSaveStateUpdate(FOnGameSavedState::FDelegate&& Delegate)
{
	Delegate.Execute(CurrentSaveSate);
	return OnGameSavedStates.Add(MoveTemp(Delegate));
}

void USaveGameSubsystem::Unregister_OnSaveStateUpdate(FDelegateHandle DelegateHandle)
{
	OnGameSavedStates.Remove(DelegateHandle);
}

bool USaveGameSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	TArray<UClass*> ChildClasses;
	GetDerivedClasses(GetClass(), ChildClasses, false);

	// Only create an instance if there is not a game-specific subclass
	return ChildClasses.Num() == 0;
}

void USaveGameSubsystem::SaveProgress()
{
	if (CurrentSaveGame)
	{
		
		PublishSaveSate(EGameSavedStates::OnSaveGameStart);
		const FOnGameSavedNative Delegate = FOnGameSavedNative::CreateUObject(this,
		&USaveGameSubsystem::OnGameSaved);
		CurrentSaveGame->AsyncSaveGame(Delegate);
	}
}

void USaveGameSubsystem::InitSaveGame()
{
	const FOnLocalPlayerSaveGameLoadedNative Delegate = FOnLocalPlayerSaveGameLoadedNative::CreateUObject(this,
		&USaveGameSubsystem::OnSaveGameLoaded);

	if (const auto Player = GetGameInstance()->GetFirstGamePlayer())
	{
		ULocalPlayerSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(SaveGameClass,
		                                                              Player,
		                                                              DefaultSaveName, Delegate);
		PublishSaveSate(EGameSavedStates::OnSaveGameLoadingStart);
	}
}

void USaveGameSubsystem::OnSaveGameLoaded(ULocalPlayerSaveGame* SaveGame)
{
	if (SaveGame)
	{
		if (auto TempSave = Cast<UAdvancedLocalPlayerSaveGame>(SaveGame))
		{
			CurrentSaveGame = TempSave;
			PublishSaveSate(EGameSavedStates::OnSaveGameLoaded);
		}
	}
}

void USaveGameSubsystem::OnGameSaved(bool isSuccess)
{
	PublishSaveSate(EGameSavedStates::OnSaveGameSaved);
}

void USaveGameSubsystem::PublishSaveSate(EGameSavedStates Sate)
{
	CurrentSaveSate = Sate;
	OnGameSavedStates.Broadcast(CurrentSaveSate);
}
