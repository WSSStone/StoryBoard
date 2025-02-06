#pragma once

#include "StoryNode.h"

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

    void OnNodeSelected(AStoryNode* node);

protected:
    bool bUsesToolkitBuilder { false };

    virtual void RequestModeUITabs() override;

private:
    void ArrangeWidget();

    TSharedPtr<SWidget> CreatePrevBtn();
    TSharedPtr<SWidget> CreateNextBtn();
    TSharedPtr<SWidget> CreateCurrnetNodeView();
    TSharedPtr<SWidget> CreatePrevNodesView();
    TSharedPtr<SWidget> CreateNextNodesView();
    TSharedPtr<SWidget> CreateNodeListView(const TArray<TObjectPtr<AStoryNode>>& List);

    TSharedPtr<SWidget> ViewportOverlayWidget;
};