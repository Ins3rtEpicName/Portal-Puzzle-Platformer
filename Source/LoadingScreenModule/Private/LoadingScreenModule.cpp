#include "LoadingScreenModule.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "MoviePlayer.h"
#include "LevelLoadingSettings.h"

#if WITH_EDITOR
	#include "ISettingsModule.h"
#endif

struct FLoadingScreenBrush : public FSlateDynamicImageBrush, public FGCObject
{
    FLoadingScreenBrush(const FName InTextureName, const FVector2D& InImageSize)
        : FSlateDynamicImageBrush(InTextureName, InImageSize)
    {
        SetResourceObject(LoadObject<UObject>(NULL, *InTextureName.ToString()));
    }

    virtual void AddReferencedObjects(FReferenceCollector& Collector) override
    {
        FSlateBrush::AddReferencedObjects(Collector);
    }

    virtual FString GetReferencerName(void) const override
    {
        return "FLoadingScreenBrush";
    }
};

class SIngameLoadingScreen : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SIngameLoadingScreen) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs)
    {
        static const FName LoadingScreenName(TEXT("/Game/_Game/UI/Asserts/LoadingScreenBG.LoadingScreenBG"));
        LoadingScreenBrush = MakeShareable(new FLoadingScreenBrush(LoadingScreenName, FVector2D(1920, 1080)));

        ChildSlot
            [
                SNew(SOverlay)
                    + SOverlay::Slot()
                    .HAlign(HAlign_Fill)
                    .VAlign(VAlign_Fill)
                    [
                        SNew(SImage)
                            .Image(LoadingScreenBrush.Get())
                    ]
                    + SOverlay::Slot()
                    .HAlign(HAlign_Fill)
                    .VAlign(VAlign_Bottom)
                    [
                        SNew(SSafeZone)
                            .VAlign(VAlign_Bottom)
                            .HAlign(HAlign_Right)
                            .Padding(10.0f)
                            .IsTitleSafe(true)
                            [
                                SNew(SThrobber)
                                    .Visibility(this, &SIngameLoadingScreen::GetLoadIndicatorVisibility)
                            ]
                    ]
            ];
    }

private:
    EVisibility GetLoadIndicatorVisibility() const
    {
        return EVisibility::Visible;
    }

    /**loading screen image brush */
    TSharedPtr<FSlateDynamicImageBrush> LoadingScreenBrush;
};

class FLoadingScreenModule : public ILoadingScreenModule
{
public:
    virtual void StartupModule() override
    {
        // Load for cooker reference
        // LoadingScreenBG needs to be inside /Content/FirstPerson/UI/Assets
        LoadObject<UObject>(NULL, TEXT("/Game/_Game/UI/Asserts/LoadingScreenBG.LoadingScreenBG"));

        UE_LOG(LogTemp, Warning, TEXT("FLoadingScreenModule::StartupModule"));
#if WITH_EDITOR
        if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
        {
            SettingsModule->RegisterSettings("Project", "Game", "LevelLoadingSettings",
                NSLOCTEXT("LoadingScreenModule", "LevelLoadingSettingsName", "Level Loading Settings"),
                NSLOCTEXT("LoadingScreenModule", "LevelLoadingSettingsDescription", "Configure loading screens for each level."),
                GetMutableDefault<ULevelLoadingSettings>());
        }
#endif

    }

    virtual bool IsGameModule() const override
    {
        return true;
    }

    virtual void StartInGameLoadingScreen() override
    {
        UE_LOG(LogTemp, Warning, TEXT("FLoadingScreenModule::StartInGameLoadingScreen"));

        FLoadingScreenAttributes LoadingScreen;
        LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
        LoadingScreen.MinimumLoadingScreenDisplayTime = 5.0f;
        LoadingScreen.WidgetLoadingScreen = SNew(SIngameLoadingScreen);

        GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
    }
};

IMPLEMENT_GAME_MODULE(FLoadingScreenModule, LoadingScreenModule);
