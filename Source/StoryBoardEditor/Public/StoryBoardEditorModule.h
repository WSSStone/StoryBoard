#pragma once

#include "Modules/ModuleManager.h"

class FStoryBoardEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
