#pragma once

#include "Modules/ModuleManager.h"

class FStoryBoardRuntimeModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
