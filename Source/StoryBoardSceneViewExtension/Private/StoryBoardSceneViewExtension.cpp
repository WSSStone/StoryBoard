#include "StoryBoardSceneViewExtension.h"

FStoryBoardSceneViewExtension::FStoryBoardSceneViewExtension(const FAutoRegister& AutoRegister, UStoryBoardSceneViewExtensionSubsystem* InSubsystem)
    : FSceneViewExtensionBase(AutoRegister)
{
    InSubsystem->SceneViewExtension = this;
}