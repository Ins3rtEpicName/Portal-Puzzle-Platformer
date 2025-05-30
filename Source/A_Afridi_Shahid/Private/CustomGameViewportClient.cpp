#include "CustomGameViewportClient.h"
#include "Engine/Canvas.h"

void UCustomGameViewportClient::PostRender(UCanvas* Canvas)
{
    Super::PostRender(Canvas);

    // Fade if requested, you could use the same DrawScreenFade method from any canvas such as the HUD
    if (bFading)
    {
        DrawScreenFade(Canvas);
    }
}

void UCustomGameViewportClient::Fade(const bool ToBlack)
{
    if (World)
    {
        bFading = true;
        bToBlack = ToBlack;
        FadeStartTime = World->GetTimeSeconds();
    }
}

void UCustomGameViewportClient::DrawScreenFade(UCanvas* Canvas)
{
    if (bFading)
    {
        if (World)
        {
            const float Time = World->GetTimeSeconds();
            const float Alpha = FMath::Clamp((Time - FadeStartTime) / FadeDuration, 0.f, 1.f);

            // Make sure that we stay black in a fade to black
            if (Alpha == 1.f && !bToBlack)
            {
                bFading = false;
            }
            else
            {
                FColor OldColor = Canvas->DrawColor;
                FLinearColor FadeColor = FLinearColor::Black;
                FadeColor.A = bToBlack ? Alpha : 1 - Alpha;
                Canvas->DrawColor = FadeColor.ToFColor(true);

                Canvas->DrawTile(Canvas->DefaultTexture, 0, 0, // UTexture* Tex, float X, float Y
                    Canvas->ClipX, Canvas->ClipY, 0, 0, // float XL, float YL, float U, float V
                    Canvas->DefaultTexture->GetSizeX(), Canvas->DefaultTexture->GetSizeY()); // float UL, float VL
                Canvas->DrawColor = OldColor;
            }
        }
    }
}