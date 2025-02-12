#pragma once

#include "StoryBoardSceneViewExtension.h"

#include "DelegatesDefinitions.h"
#include "CoreMinimal.h"

#include "StoryBoardSceneViewExtensionSubsystem.generated.h"

#if WITH_EDITOR
class FStoryBoardSceneViewExtensionEditorUndoClient : public FEditorUndoClient {
};
#else
class FStoryBoardSceneViewExtensionEditorUndoClient {
};
#endif

UCLASS()
class STORYBOARDSCENEVIEWEXTENSION_API UStoryBoardSceneViewExtensionSubsystem : public UWorldSubsystem, public FStoryBoardSceneViewExtensionEditorUndoClient {
    GENERATED_BODY()
public:
    void Initialize(FSubsystemCollectionBase& Collection) override;

    void Deinitialize() override;

    void DisableSVE();

#if WITH_EDITOR
    virtual void PostUndo(bool bSuccess) override {};

    virtual void PostRedo(bool bSuccess) override {}
#endif

    void BindIndicatorDelegate(FStoryNodeWrapperDelegate& Delegate);
    
    void UnbindIndicatorDelegate(FStoryNodeWrapperDelegate& Delegate);

    void HandleStoryNodeWrapperHint(FStoryNodeWrapper* Wrapper);

private:
    bool bRender {false};

    FStoryNodeWrapper* HintWrapper;

    TSharedPtr<class FStoryBoardSceneViewExtension, ESPMode::ThreadSafe> StoryBoardSceneViewExtension;

    friend class FStoryBoardSceneViewExtension;
};