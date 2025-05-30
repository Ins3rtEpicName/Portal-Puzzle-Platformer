#include "GameMode/BaseGM.h"
#include "PlayerController/BasePC.h"
#include "HUD/BaseHUD.h"

ABaseGM::ABaseGM()
	: Super()
{
	// Use our custom c++ Base PlayerController class
	PlayerControllerClass = ABasePC::StaticClass();

	// Use our custom c++ base hud class
	HUDClass = ABaseHUD::StaticClass();
}
