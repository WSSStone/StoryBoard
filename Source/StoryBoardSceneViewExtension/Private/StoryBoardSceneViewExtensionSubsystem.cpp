#include "StoryBoardSceneViewExtensionSubsystem.h"
#include "StoryBoardSceneViewExtension.h"

#define LOCTEXT_NAMESPACE "StoryBoardSceneViewExtensionSubsystem"

void UStoryBoardSceneViewExtensionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UStoryBoardSceneViewExtensionSubsystem::Deinitialize() {
    Super::Deinitialize();
}

void UStoryBoardSceneViewExtensionSubsystem::OnActivateSceneViewExtension() {
    if (SceneViewExtension) {
        SceneViewExtension->bRender = true;
    }
}

void UStoryBoardSceneViewExtensionSubsystem::OnDeactivateSceneViewExtension() {
    if (SceneViewExtension) {
        SceneViewExtension->bRender = false;
    }
}

void UStoryBoardSceneViewExtensionSubsystem::OnSelectedNodeChange() {
    if (SceneViewExtension) {
        // SceneViewExtension->OnSelectedNodeChange();
    }
}

void UStoryBoardSceneViewExtensionSubsystem::OnActiveScenarioChange() {
    if (SceneViewExtension) {
        // SceneViewExtension->OnActiveScenarioChange();
    }
}

#undef LOCTEXT_NAMESPACE