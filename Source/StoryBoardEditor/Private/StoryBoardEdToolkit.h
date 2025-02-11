#pragma once

#include "StoryBoardSubsystem.h"

#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

class FStoryBoardEdToolkit : public FModeToolkit {
public:
    FStoryBoardEdToolkit();
    virtual ~FStoryBoardEdToolkit();

    virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode) override;
    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual class FEdMode* GetEditorMode() const override;

    void OnNodeSelectedRedraw(FStoryNodeWrapper* Wrapper);

protected:
    bool bUsesToolkitBuilder { false };

    virtual void RequestModeUITabs() override;

private:
    void ArrangeWidget();

    enum class ImageSize : int32 {
        S16 = 16,
        S20 = 20,
        S50 = 50,
        S128 = 128,
        S256 = 256
    };

    TSharedPtr<SWidget> CreatePrevBtn();
    
    TSharedPtr<SWidget> CreateNextBtn();
    
    TSharedPtr<SWidget> CreateCurrnetNodeView();
    
    TSharedPtr<SWidget> CreatePrevNodesView();
    
    TSharedPtr<SWidget> CreateNextNodesView();
    
    TSharedPtr<SWidget> CreateNodeListView(const TArray<TObjectPtr<AStoryNode>>& List);
    
    TSharedPtr<SWidget> CreateNodeView(AStoryNode* Node, ImageSize Size);

    TSharedPtr<SWidget> CreateAssetThumbnailWidget(UObject* Asset, int32 Size);

    TSharedPtr<SWidget> ViewportOverlayWidget;
};