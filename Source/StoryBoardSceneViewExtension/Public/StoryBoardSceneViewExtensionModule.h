#pragma once

#include "Modules/ModuleManager.h"

class FStoryBoardSVExtensionModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};