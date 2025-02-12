#include "StoryBoardSceneViewExtensionModule.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"


void FStoryBoardSVExtensionModule::StartupModule()
{
    // map plugin shader path
    FString shaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("StoryBoard"))->GetBaseDir(), TEXT("Shaders"));
    AddShaderSourceDirectoryMapping(TEXT("/StoryBoard"), shaderDir);
}

void FStoryBoardSVExtensionModule::ShutdownModule()
{

}

IMPLEMENT_MODULE(FStoryBoardSVExtensionModule, StoryBoardSceneViewExtension)