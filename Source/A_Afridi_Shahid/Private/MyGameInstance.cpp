#include "MyGameInstance.h"

#include "CustomGameViewportClient.h"
#include "Kismet/GameplayStatics.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"
#include "MyBlueprintFunctionLibrary.h"
#include "LoadingScreenModule.h"
#include "CustomGameViewportClient.h"
#include "Components/AudioComponent.h"
#include "LevelLoadingSettings.h"
#include "AutoSave/AutoSaveSubsystem.h"

void UMyGameInstance::Init()
{
	Super::Init();

	FTickerDelegate TickDelegate = FTickerDelegate::CreateUObject(this, &UMyGameInstance::Tick);
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);

	IPlatformInputDeviceMapper::Get().GetOnInputDeviceConnectionChange().AddUObject(this, &UMyGameInstance::OnControllerChanged);

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UMyGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UMyGameInstance::EndLoadingScreen);

	UAutoSaveSubsystem* AutoSaveSubsystem = GetSubsystem<UAutoSaveSubsystem>();
	if (AutoSaveSubsystem)
	{
		AutoSaveSubsystem->SaveNotificationWidgetClass = SaveNotificationWidgetClass;
	}
}

void UMyGameInstance::Shutdown()
{
	Super::Shutdown();
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

bool UMyGameInstance::Tick(float DeltaTime)
{

	if (GEngine)
	{
		FString message = FString::Printf(TEXT("Active Controller ID: %d"), ActiveControllerId);
		GEngine->AddOnScreenDebugMessage(1, 0.2f, FColor::Blue, *message);
	}

	return true;
}


void UMyGameInstance::SetActiveControllerID(int32 ControllerID)
{

	ActiveControllerId = ControllerID;
}

int32 UMyGameInstance::GetActiveControllerID()
{
	return ActiveControllerId;
}

UUIDataAsset* UMyGameInstance::GetUIDataAsset() const
{
	// Get current Platform
	FString PlatformName = UGameplayStatics::GetPlatformName();

	if (PlatformName == "PS5")
	{
		return PS5UIDataAsset;
	}
	else if (PlatformName == "SWITCH")
	{
		return SwitchUIDataAsset;
	}
	else
	{
		return XboxUIDataAsset;
	}
}

void UMyGameInstance::OnControllerChanged(EInputDeviceConnectionState connectionState, FPlatformUserId userID, FInputDeviceId inputDeviceId)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller changed!"));

	FString UserId_Text;
	if (userID == PLATFORMUSERID_NONE)
	{
		UserId_Text = TEXT("None");
	}
	else
	{
		UserId_Text = FString::Printf(TEXT("%d"), userID.GetInternalId());
	}

	if (connectionState == EInputDeviceConnectionState::Disconnected)
	{
		FString message = FString::Printf(TEXT("Controller connection changed - UserIdx:%d, UserID:%s, Disconnected"), inputDeviceId.GetId(), *UserId_Text);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, message);
	}
	else if (connectionState == EInputDeviceConnectionState::Connected)
	{
		FString message = FString::Printf(TEXT("Controller connection changed - UserIdx:%d, UserID:%s, Connected"), inputDeviceId.GetId(), *UserId_Text);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, message);
	}

	if (GetActiveControllerID() == inputDeviceId.GetId())
	{
		if (APlayerController* ActivePC = UMyBlueprintFunctionLibrary::GetActivePlayerController(this))
		{
			if (connectionState == EInputDeviceConnectionState::Connected) // Our active controller connected
			{
				if (bDidControllerDisconnectPauseGame)
				{
					UGameplayStatics::SetGamePaused(this, false);
					bDidControllerDisconnectPauseGame = false;
				}
				ControllerDisconnectedWidget->RemoveFromParent();
			}
			else // Our active controller disconnected
			{
				if (ControllerDisconnectedWidgetClass)
				{
					if(!UGameplayStatics::IsGamePaused(this))
					{
						bDidControllerDisconnectPauseGame = true;
						UGameplayStatics::SetGamePaused(this,true);
					}
					ControllerDisconnectedWidget = CreateWidget<UControllerDisconnectedWidget>(ActivePC, ControllerDisconnectedWidgetClass);
					ControllerDisconnectedWidget->AddToViewport();
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ControllerDisconnectedWidgetClass NOT set!"));
				}
			}	
		}
	}
}

void UMyGameInstance::BeginLoadingScreen(const FString& InMapName)
{
	// fade in to black
	const UWorld* World = GetWorld();
	if (World)
	{
		UCustomGameViewportClient* GameViewportClient = Cast<UCustomGameViewportClient>(World->GetGameViewport());
		if (GameViewportClient)
		{
			GameViewportClient->Fade(true);
		}
	}

	//if (InMapName.Contains("Screen"))
	//{
	//	// We are loading a frontend screen, so no loading screen is required
	//}
	//else
	//{
	//	// Example of how to display the loading screen with the LoadingScreenModule
	//	ILoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<ILoadingScreenModule>("LoadingScreenModule");

	//	if (LoadingScreenModule != nullptr)
	//	{
	//		LoadingScreenModule->StartInGameLoadingScreen();
	//	}

	//}


	//// Display loading screen with our LoadingScreenModule
	ILoadingScreenModule* LoadingScreenModule = 
		FModuleManager::LoadModulePtr<ILoadingScreenModule>("LoadingScreenModule");

	if (LoadingScreenModule != nullptr)
	{
		FString LevelName;
		if (InMapName.Split(TEXT("/"), nullptr, &LevelName, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
		{
			ULevelLoadingSettings* LoadingSettings = GetMutableDefault<ULevelLoadingSettings>();
			bool bShowLoadingScreen = false;

			for (FSoftObjectPath Path : LoadingSettings->LevelLoadingScreens)
			{
				if (Path.ToString().Contains(LevelName))
				{
					bShowLoadingScreen = true;
					break;
				}
				else
				{
					bShowLoadingScreen = false;
				}
			}

			if (bShowLoadingScreen)
			{
				// Show loading screen
				LoadingScreenModule->StartInGameLoadingScreen();
			}
				
		}

		

	//	FString LevelName;

	//	if (InMapName.Split(TEXT("/"), nullptr, &LevelName, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
	//	{
	//		// Convert FString to FName for map lookup
	//		FName LevelFName(*LevelName);

	//		if (LoadingSettings->LevelLoadingScreens.Contains(LevelFName))
	//		{
	//			bShowLoadingScreen = LoadingSettings->LevelLoadingScreens[LevelFName];
	//		}

	//		if (bShowLoadingScreen)
	//		{
	//			// Show loading screen
	//			LoadingScreenModule->StartInGameLoadingScreen();
	//		}
	//	}
	}

}

void UMyGameInstance::EndLoadingScreen(UWorld * InLoadedWorld)
{
	// fade out to black
	const UWorld* World = GetWorld();
	if (World)
	{
		UCustomGameViewportClient* GameViewportClient = Cast<UCustomGameViewportClient>(World->GetGameViewport());
		if (GameViewportClient)
		{
			GameViewportClient->Fade(false);
		}
	}
}

void UMyGameInstance::PlayGameplayMusic()
{
	// Stop any playing menu or start screen music
	StopMenuMusic();
	StopStartScreenMusic();

	// Play gameplay music
	if (GameplayMusicAudioComponent)
	{
		// If the music is not already playing, fade it in
		if (!GameplayMusicAudioComponent->IsPlaying())
		{
			// Fade in gameplay music
			GameplayMusicAudioComponent->FadeIn(MusicFadeDuration, 1.0f);
		}
	}
	else
	{
		// If there is gameplay music available
		if (GameplayMusic)
		{
			// Create and play gameplay music
			GameplayMusicAudioComponent = UGameplayStatics::SpawnSound2D(this, GameplayMusic);

			if (GameplayMusicAudioComponent)
			{
				// Fade in gameplay music
				GameplayMusicAudioComponent->FadeIn(MusicFadeDuration, 1.0f);
			}
		}
		else
		{
			// Log a warning if the GameplayMusic is NULL
			UE_LOG(LogTemp, Warning, TEXT("GameplayMusic is NULL!"));
		}
	}
}

void UMyGameInstance::StopGameplayMusic(bool ForceStop)
{
	// Stop gameplay music
	if (GameplayMusicAudioComponent && GameplayMusicAudioComponent->IsPlaying())
	{
		if (ForceStop)
		{
			GameplayMusicAudioComponent->Stop();
		}
		else
		{
			// Fade out gameplay music
			GameplayMusicAudioComponent->FadeOut(MusicFadeDuration, 0.0f);
		}
	}
}

void UMyGameInstance::PlayMenuMusic()
{
	// Stop any gameplay or start screen music
	StopGameplayMusic(false);
	StopStartScreenMusic();

	// Play menu music
	if (MenuMusicAudioComponent)
	{
		if (!MenuMusicAudioComponent->IsPlaying())
		{
			// Fade in menu music
			MenuMusicAudioComponent->FadeIn(MusicFadeDuration, 1.0f);
		}
	}
	else
	{
		if (MenuMusic)
		{
			// Create and play menu music
			MenuMusicAudioComponent = UGameplayStatics::SpawnSound2D(
				this, MenuMusic, 1.0f, 1.0f, 0.0f, nullptr, true, true);

			if (MenuMusicAudioComponent)
			{
				// Fade in menu music
				MenuMusicAudioComponent->FadeIn(MusicFadeDuration, 1.0f);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MenuMusic is NULL!"));
		}
	}
}

// Function to stop menu music with optional force stop
void UMyGameInstance::StopMenuMusic(bool ForceStop)
{
	// Check if the menu audio component is playing
	if (MenuMusicAudioComponent && MenuMusicAudioComponent->IsPlaying())
	{
		if (ForceStop)
		{
			MenuMusicAudioComponent->Stop();
		}
		else
		{
			// Fade out menu music
			MenuMusicAudioComponent->FadeOut(MusicFadeDuration, 0.0f);
		}
	}
}

void UMyGameInstance::PlayStartScreenMusic()
{
	StopGameplayMusic(false);
	StopMenuMusic(false);

	// Play start screen music
	if (StartScreenMusicAudioComponent)
	{
		if (!StartScreenMusicAudioComponent->IsPlaying())
		{
			// Fade in start screen music
			StartScreenMusicAudioComponent->FadeIn(MusicFadeDuration, 1.0f);
		}
	}
	else if (StartScreenMusic)
	{
		// Create and play start screen music
		StartScreenMusicAudioComponent = UGameplayStatics::SpawnSound2D(this, StartScreenMusic);
		if (StartScreenMusicAudioComponent)
		{
			// Fade in start screen music
			StartScreenMusicAudioComponent->FadeIn(MusicFadeDuration, 1.0f);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("StartScreenMusic is NULL!"));
	}
}

void UMyGameInstance::StopStartScreenMusic(bool ForceStop)
{
	// Stop start screen music
	if (StartScreenMusicAudioComponent && StartScreenMusicAudioComponent->IsPlaying())
	{
		if (ForceStop)
		{
			StartScreenMusicAudioComponent->Stop();
		}
		else
		{
			// Fade out start screen music
			StartScreenMusicAudioComponent->FadeOut(MusicFadeDuration, 0.0f);
		}
	}
}



