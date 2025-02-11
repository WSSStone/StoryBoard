#pragma once

#include "Definitions.h"
#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "StoryBoardSceneViewExtensionSubsystem.generated.h"

class FStoryBoardSceneViewExtension;

UCLASS()
class STORYBOARDSCENEVIEWEXTENSION_API UStoryBoardSceneViewExtensionSubsystem : public UEditorSubsystem
{
    GENERATED_BODY()
public:
    void Initialize(FSubsystemCollectionBase& Collection) override;
    void Deinitialize() override;

    void OnActivateSceneViewExtension();
    void OnDeactivateSceneViewExtension();

    void HandleStoryNodeWrapper(FStoryNodeWrapper* Wrapper);

    FStoryBoardSceneViewExtension* SceneViewExtension;
};