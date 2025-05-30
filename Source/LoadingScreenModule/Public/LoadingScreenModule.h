#pragma once

#include "Modules/ModuleInterface.h"

/** Module interface for this game's Loading screens */
class ILoadingScreenModule : public IModuleInterface
{
public:
    /** Kicks off the loading screen for in-game loading (not startup) */
    virtual void StartInGameLoadingScreen() = 0;
};
