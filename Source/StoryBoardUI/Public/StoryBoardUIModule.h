#pragma once

#include "Modules/ModuleManager.h"

class FStoryBoardUIModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};