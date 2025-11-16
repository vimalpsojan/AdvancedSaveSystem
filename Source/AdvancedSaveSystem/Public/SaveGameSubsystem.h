// Copyright (c) 2025 Vimal P Sojan
// Licensed under the MIT License. See LICENSE file in the project root.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveGameSubsystem.generated.h"

class UAdvancedLocalPlayerSaveGame;
class ULocalPlayerSaveGame;

UENUM(BlueprintType)
enum class EGameSavedStates : uint8
{
	Init = 0,
	OnSaveGameLoadingStart,
	OnSaveGameLoaded,
	OnSaveGameStart,
	OnSaveGameSaved
};

// Delegate called when the replay player state changes
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameSavedState, EGameSavedStates);

/**
 * Game subsystem that handles queries and changes to user identity and login status.
 * One subsystem is created for each game instance and can be accessed from blueprints or C++ code.
 * If a game-specific subclass exists, this base subsystem will not be created.
 */
UCLASS(BlueprintType, Config=Game)
class ADVANCEDSAVESYSTEM_API USaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	USaveGameSubsystem();

	static FString DefaultSaveName;

	void UserInitialized();

	FDelegateHandle CallOrRegister_OnSaveStateUpdate(FOnGameSavedState::FDelegate&& Delegate);
	
	void Unregister_OnSaveStateUpdate(FDelegateHandle DelegateHandle);

	UFUNCTION(BlueprintPure)
	EGameSavedStates GetState() const { return CurrentSaveSate; }

protected:

	UPROPERTY()
	TObjectPtr<UAdvancedLocalPlayerSaveGame> CurrentSaveGame;

	UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category=Classes)
	TSubclassOf<UAdvancedLocalPlayerSaveGame> SaveGameClass;
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	UFUNCTION(BlueprintCallable)
	void SaveProgress();

	UFUNCTION(BlueprintCallable,BlueprintPure = true)
	UAdvancedLocalPlayerSaveGame* GetCurrentSaveGame() const { return CurrentSaveGame; }

private:

	EGameSavedStates CurrentSaveSate;

	FOnGameSavedState OnGameSavedStates;
	
	void InitSaveGame();

	virtual void OnSaveGameLoaded(ULocalPlayerSaveGame* SaveGame);
	
	virtual void OnGameSaved(bool isSuccess);

	void PublishSaveSate(EGameSavedStates Sate);
	
};
