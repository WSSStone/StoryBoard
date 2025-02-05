#pragma once

#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

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

    void BroadcastTryExitEdMode();

    DECLARE_EVENT(UStoryBoardEdMode, FTryExitEdModeEvent);
    
    void BroadcastTryExitEdModeEvent();

protected:
    virtual void CreateToolkit() override;

private:
    FTryExitEdModeEvent TryExitEdModeEvent;
};

class FStoryBoardEdToolkit : public FModeToolkit {
public:
    FStoryBoardEdToolkit();
    virtual ~FStoryBoardEdToolkit();

    virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode) override;
    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual class FEdMode* GetEditorMode() const override;

protected:
    bool bUsesToolkitBuilder { false };

    virtual void RequestModeUITabs() override;

private:
    TSharedPtr<SWidget> ViewportOverlayWidget;
};