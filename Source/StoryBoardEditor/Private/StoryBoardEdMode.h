#pragma once

#include "CoreMinimal.h"
#include "Tools/UEdMode.h"

#include "StoryBoardEdMode.generated.h"

class UStoryBoardEditorSubsystem;

UCLASS()
class STORYBOARDEDITOR_API UStoryBoardEdMode : public UEdMode {
    GENERATED_BODY()
public:
    static const FEditorModeID EM_StoryBoardEdModeId;

    UStoryBoardEdMode();
    virtual ~UStoryBoardEdMode();

    // Override methods to handle input, rendering, etc.
    virtual void Enter() override;
    virtual void Exit() override;
    virtual bool UsesToolkits() const override;
    virtual void ActorSelectionChangeNotify() override;
    // virtual TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetModeCommands() const override;

protected:
    virtual void CreateToolkit() override;
};