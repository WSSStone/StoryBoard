#include "StoryBoardUIModule.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"


void FStoryBoardUIModule::StartupModule()
{
    // map plugin shader path
    FString shaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("StoryBoard"))->GetBaseDir(), TEXT("Shaders"));
    AddShaderSourceDirectoryMapping(TEXT("/StoryBoard"), shaderDir);
}

void FStoryBoardUIModule::ShutdownModule()
{

}

IMPLEMENT_MODULE(FStoryBoardUIModule, StoryBoardUI)