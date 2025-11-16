### AdvancedSaveSystem — Read Me

#### Overview
AdvancedSaveSystem is a runtime plugin that centralizes game saving through a `UGameInstanceSubsystem` called `USaveGameSubsystem` and a concrete save object `UAdvancedLocalPlayerSaveGame` (derived from `ULocalPlayerSaveGame`). It exposes a simple Blueprint/C++ API to get the current save object, trigger saves, and react to lifecycle events via a state enum and delegates.

Key elements:
- `USaveGameSubsystem` — owns the current save, performs save operations, exposes state and delegates.
- `UAdvancedLocalPlayerSaveGame` — concrete save class with an optional async helper.
- `EGameSavedStates` — load/save lifecycle: `Init`, `OnSaveGameLoadingStart`, `OnSaveGameLoaded`, `OnSaveGameStart`, `OnSaveGameSaved`.

---

### Requirements
- Unreal Engine: 5.5.0 (Tested in 5.5.x, 5.6.x, and 5.7.0)
- Module type: Runtime

---

### Installation
Copy to Plugins folder
- `Plugins/AdvancedSaveSystem`

No extra setup needed. Build the project as usual and ensure the plugin is enabled.

---

### Features
- Centralized save orchestration per `UGameInstance`.
- Blueprint-accessible API to save and retrieve the current save object.
- High-level state notifications for UI/UX integration.
- Native async helper on the save object (`AsyncSaveGame`).
- Configurable default slot name.

---

### Public API and Headers

1) USaveGameSubsystem
- Header: `Source/AdvancedSaveSystem/Public/SaveGameSubsystem.h`
- Highlights:
  - `static FString DefaultSaveName;`
  - `UFUNCTION(BlueprintCallable) void SaveProgress();`
  - `UFUNCTION(BlueprintCallable, BlueprintPure) UAdvancedLocalPlayerSaveGame* GetCurrentSaveGame() const;`
  - `UFUNCTION(BlueprintPure) EGameSavedStates GetState() const;`
  - Delegates:
    - `DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameSavedState, EGameSavedStates);`
    - `FDelegateHandle CallOrRegister_OnSaveStateUpdate(FOnGameSavedState::FDelegate&&);`
    - `void Unregister_OnSaveStateUpdate(FDelegateHandle);`

2) UAdvancedLocalPlayerSaveGame
- Header: `Source/AdvancedSaveSystem/Public/AdvancedLocalPlayerSaveGame.h`
- Highlights:
  - `DECLARE_DELEGATE_OneParam(FOnGameSavedNative, bool /*bSuccess*/);`
  - `bool AsyncSaveGame(FOnGameSavedNative Delegate);`

3) EGameSavedStates
- Enum defined in `SaveGameSubsystem.h` with values: `Init`, `OnSaveGameLoadingStart`, `OnSaveGameLoaded`, `OnSaveGameStart`, `OnSaveGameSaved`.

---

### Quick Start (Blueprint)
1. Create your own Blueprint class derived from `AdvancedLocalPlayerSaveGame` (or a C++ subclass) and add your save fields.
2. In gameplay UI or controller logic:
   - Get Game Instance Subsystem → `SaveGameSubsystem`.
   - Use `GetCurrentSaveGame` to read/write values.
   - Call `SaveProgress` to persist.
   - Optionally read `GetState` to drive loading/saving indicators.

Tip: Because the subsystem is `BlueprintType`, it’s accessible via the standard “Get Game Instance Subsystem” node.

---

### Quick Start (C++)
Access and save via the subsystem:
```c++
#include "SaveGameSubsystem.h"
#include "AdvancedLocalPlayerSaveGame.h"

void UMyComponent::SaveNow()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (USaveGameSubsystem* SGS = GI->GetSubsystem<USaveGameSubsystem>())
            {
                if (UAdvancedLocalPlayerSaveGame* Save = SGS->GetCurrentSaveGame())
                {
                    // Modify your save values here
                    // Save->YourData = ...;
                }

                SGS->SaveProgress();
            }
        }
    }
}
```

Subscribe to state updates:
```c++
FDelegateHandle Handle;

void UMyComponent::BeginPlay()
{
    Super::BeginPlay();
    if (USaveGameSubsystem* SGS = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
    {
        Handle = SGS->CallOrRegister_OnSaveStateUpdate(
            FOnGameSavedState::FDelegate::CreateUObject(this, &UMyComponent::OnSaveStateChanged));
    }
}

void UMyComponent::EndPlay(const EEndPlayReason::Type Reason)
{
    if (USaveGameSubsystem* SGS = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
    {
        SGS->Unregister_OnSaveStateUpdate(Handle);
    }
    Super::EndPlay(Reason);
}

void UMyComponent::OnSaveStateChanged(EGameSavedStates NewState)
{
    // Update UI or react to save lifecycle
}
```

Use the async helper (optional):
```c++
if (USaveGameSubsystem* SGS = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
{
    if (UAdvancedLocalPlayerSaveGame* Save = SGS->GetCurrentSaveGame())
    {
        Save->AsyncSaveGame(FOnGameSavedNative::CreateLambda([](bool bSuccess)
        {
            UE_LOG(LogTemp, Log, TEXT("Async save complete: %s"), bSuccess ? TEXT("OK") : TEXT("FAIL"));
        }));
    }
}
```

---

### Configuration
- Default slot name: `USaveGameSubsystem::DefaultSaveName` (static `FString`). Set it at startup if you need a different default.
- Save class: `USaveGameSubsystem::SaveGameClass` (editable/BlueprintReadOnly). Assign your subclass of `UAdvancedLocalPlayerSaveGame` to control stored data.

---

### Typical Lifecycle
1) Subsystem is created with the `UGameInstance` (subject to `ShouldCreateSubsystem`).
2) It initializes/loads a save (`OnSaveGameLoadingStart` → `OnSaveGameLoaded`).
3) Game updates `CurrentSaveGame` during play.
4) Calling `SaveProgress` publishes `OnSaveGameStart` and then `OnSaveGameSaved` on completion.

Note: The concrete serialization is based on standard Unreal save APIs inside the implementation for this project.

---

### Troubleshooting
- Subsystem not found: Ensure the plugin is enabled and you query it from a valid `UGameInstance` (`GetWorld()` must not be null).
- Data not persisting: Verify your fields aren’t `Transient`, ensure the correct `SaveGameClass` is set, and confirm the default slot name is consistent.
- No state notifications: Make sure you keep the returned `FDelegateHandle` and unregister on shutdown; poll `GetState` from Blueprints if needed.

---

---

### Related
See the `SaveGameInspector` plugin in this repo for in-editor inspection and editing of `USaveGame` files — a useful companion to this runtime system.
