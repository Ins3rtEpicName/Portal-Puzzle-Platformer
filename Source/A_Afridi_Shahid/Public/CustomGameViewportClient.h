#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "CustomGameViewportClient.generated.h"

/** A simple UGameViewportClient used to handle a global fade in/out */
UCLASS()
class A_AFRIDI_SHAHID_API UCustomGameViewportClient : public UGameViewportClient
{
    GENERATED_BODY()

public:
    /** Called after rendering the player views and HUDs to render menus, the console, etc.
    * This is the last rendering call in the render loop
    * @param Canvas The canvas to use for rendering.
    */
    virtual void PostRender(UCanvas* Canvas) override;

    /** Used for Fade to and from black */
    virtual void Fade(const bool ToBlack);

private:
    /** Does the actual screen fading */
    void DrawScreenFade(UCanvas* Canvas);

private:
    // Values used by our screen fading
    bool bFading : 1;
    bool bToBlack : 1; // Fading to black will be applied even if alpha is 1
    float FadeAlpha;
    float FadeStartTime;
    float FadeDuration = 2.0f;
};
