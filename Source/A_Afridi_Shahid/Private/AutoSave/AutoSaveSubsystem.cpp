#include "AutoSave/AutoSaveSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void UAutoSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Load previous save data if available
    LoadGame();
}

void UAutoSaveSubsystem::Deinitialize()
{
    GetWorld()->GetTimerManager().ClearTimer(HideWidgetTimerHandle);
    Super::Deinitialize();
}

void UAutoSaveSubsystem::IncrementJumpCount()
{
    JumpCount++;

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Jump Count: %d"), JumpCount));

    // Save every time jump count is a multiple of 10
    if (JumpCount % 10 == 0)
    {
        if (SaveGame())
        {
            ShowSaveNotificationWidget();
        }
    }
}

void UAutoSaveSubsystem::IncrementPortalsUsedCount()
{
    PortalsUsedCount++;

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("PortalsUsed COunt: %d"), PortalsUsedCount));

    // Save every time jump count is a multiple of 10
    if (PortalsUsedCount % 5 == 0)
    {
        if (SaveGame())
        {
            ShowSaveNotificationWidget();
        }
    }
}

void UAutoSaveSubsystem::IncrementDeathCount()
{
    DeathCount++;

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("PortalsUsed Count: %d"), DeathCount));

    // Save every time jump count is a multiple of 10
    if (DeathCount % 3 == 0)
    {
        if (SaveGame())
        {
            ShowSaveNotificationWidget();
        }
    }
}

void UAutoSaveSubsystem::IncrementButtonClickCount()
{
    ButtonClickCount++;

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Button Click Count: %d"), ButtonClickCount));

    // Save every time click count is a multiple of 16
    if (ButtonClickCount % 10 == 0)
    {
        if (SaveGame())
        {
            ShowSaveNotificationWidget();
        }
    }
}

bool UAutoSaveSubsystem::SaveGame()
{
    UAutoSaveGame* SaveGameInstance = Cast<UAutoSaveGame>(UGameplayStatics::CreateSaveGameObject(UAutoSaveGame::StaticClass()));
    if (!SaveGameInstance) return false;

    SaveGameInstance->JumpCount = JumpCount;
    SaveGameInstance->ButtonClickCount = ButtonClickCount;
    SaveGameInstance->ObjectsDestroyedCount = ObjectsDestroyedCount;
    SaveGameInstance->SoulsCollectedCount = SoulsCollectedCount;
    SaveGameInstance->PortalsUsedCount = PortalsUsedCount;
    SaveGameInstance->DeathCount = DeathCount;

    return UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);
}

void UAutoSaveSubsystem::LoadGame()
{
    if (UAutoSaveGame* LoadedGame = Cast<UAutoSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0)))
    {
        JumpCount = LoadedGame->JumpCount;
        ButtonClickCount = LoadedGame->ButtonClickCount;
        ObjectsDestroyedCount = LoadedGame->ObjectsDestroyedCount;
        SoulsCollectedCount = LoadedGame->SoulsCollectedCount;
        PortalsUsedCount = LoadedGame->PortalsUsedCount;
        DeathCount = LoadedGame->DeathCount;
    }
    else
    {
        JumpCount = 0;
        ButtonClickCount = 0;
        ObjectsDestroyedCount = 0;
        SoulsCollectedCount = 0;
        PortalsUsedCount = 0;
        DeathCount = 0;
    }
}

// Show the save notification widget
void UAutoSaveSubsystem::ShowSaveNotificationWidget()
{
    if (SaveNotificationWidgetClass && !ActiveSaveNotificationWidget)
    {
        ActiveSaveNotificationWidget = CreateWidget<UUserWidget>(GetWorld(), SaveNotificationWidgetClass);
        if (ActiveSaveNotificationWidget)
        {
            ActiveSaveNotificationWidget->AddToViewport();
            GetWorld()->GetTimerManager().SetTimer(HideWidgetTimerHandle, this, &UAutoSaveSubsystem::HideSaveNotificationWidget, 2.0f, false);
        }
    }
}

// Hide the save notification widget
void UAutoSaveSubsystem::HideSaveNotificationWidget()
{
    if (ActiveSaveNotificationWidget)
    {
        ActiveSaveNotificationWidget->RemoveFromParent();
        ActiveSaveNotificationWidget = nullptr;
    }
}

void UAutoSaveSubsystem::IncrementObjectsDestroyedCount()
{
    ObjectsDestroyedCount++;

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Objects Count: %d"), ObjectsDestroyedCount));

    // Save every time object is destroyed
    if (SaveGame())
    {
        ShowSaveNotificationWidget();
    }
}

void UAutoSaveSubsystem::IncrementSoulsCollectedCount()
{
    SoulsCollectedCount++;

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("SoulsCollected Count: %d"), SoulsCollectedCount));

    // Save every time object is destroyed
    if (SaveGame())
    {
        ShowSaveNotificationWidget();
    }
}

void UAutoSaveSubsystem::CheckBestTime(int32 NewSeconds, int32 NewMinutes)
{
    // If there is no best time set yet, initialize it
    if (minutes == 0 && seconds == 0)
    {
        // New Best Time Set!
        minutes = NewMinutes;
        seconds = NewSeconds;
        SaveGame();
        return;
    }

    // Compare new time with the saved best time
    if (NewMinutes < minutes || (NewMinutes == minutes && NewSeconds < seconds))
    {
        // New Best Time Set!
        minutes = NewMinutes;
        seconds = NewSeconds;
        SaveGame();
    }
    else
    {
        // Not a Best Time
    }
}
