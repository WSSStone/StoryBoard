#include "StoryBoardEdToolkit.h"
#include "StoryBoardEditorSubsystem.h"
#include "StoryBoardEdMode.h"

#include "Widgets/Images/SImage.h"
#include "Slate/SlateBrushAsset.h"
#include "Engine/Texture2D.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "ObjectTools.h"
#include "Styling/SlateIconFinder.h"


#define LOCTEXT_NAMESPACE "StoryBoardEditorModeToolkit"

FStoryBoardEdToolkit::FStoryBoardEdToolkit() {

}

FStoryBoardEdToolkit::~FStoryBoardEdToolkit() {
    if (IsHosted() && ViewportOverlayWidget.IsValid()) {
        GetToolkitHost()->RemoveViewportOverlayWidget(ViewportOverlayWidget.ToSharedRef());
    }

    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    edSubsys->EdNodeSelectedEvent.RemoveAll(this);
}

void FStoryBoardEdToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode) {
    FModeToolkit::Init(InitToolkitHost, InOwningMode);

    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    edSubsys->EdNodeSelectedEvent.AddRaw(this, &FStoryBoardEdToolkit::OnNodeSelected);
    
    edSubsys->OnEnterEdMode();
    ArrangeWidget();

    GetToolkitHost()->AddViewportOverlayWidget(ViewportOverlayWidget.ToSharedRef());
}

void FStoryBoardEdToolkit::OnNodeSelected(AStoryNode* node) {
    if (IsHosted() && ViewportOverlayWidget.IsValid()) {
        GetToolkitHost()->RemoveViewportOverlayWidget(ViewportOverlayWidget.ToSharedRef());
    }

    ArrangeWidget();

    GetToolkitHost()->AddViewportOverlayWidget(ViewportOverlayWidget.ToSharedRef());
}

void FStoryBoardEdToolkit::ArrangeWidget() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();

    FPropertyEditorModule& editModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs detailsViewArgs;
    detailsViewArgs.bShowObjectLabel = true;
    detailsViewArgs.bAllowSearch = false;
    detailsViewArgs.bAllowFavoriteSystem = false;
    detailsViewArgs.NameAreaSettings = FDetailsViewArgs::ENameAreaSettings::HideNameArea;
    detailsViewArgs.ViewIdentifier = FName("BlueprintDefaults");
    auto detailView = editModule.CreateDetailView(detailsViewArgs);
    auto scenario = edSubsys->GetCurrentScenario();
    detailView->SetObject(scenario);

    FMenuBuilder MenuBuilder(true, nullptr);
    MenuBuilder.AddMenuEntry(
        scenario->Name,
        scenario->Tooltip,
        FSlateIconFinder::FindIconForClass(UStoryScenario::StaticClass()),
        FUIAction(
            FExecuteAction::CreateLambda([scenario]() {
                GEditor->EditObject(scenario);
                })
        ),
        NAME_None,
        EUserInterfaceActionType::Button);
    TSharedRef<SWidget> widget = MenuBuilder.MakeWidget();

    auto currentView = CreateCurrnetNodeView();
    auto nextListView = CreateNextNodesView();
    auto prevListView = CreatePrevNodesView();
    auto nextBtn = CreateNextBtn();
    auto prevBtn = CreatePrevBtn();

    SAssignNew(ViewportOverlayWidget, SHorizontalBox)
        + SHorizontalBox::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Bottom)
        .Padding(FMargin(0.0, 0.0, 0.0, 15.0))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Bottom)
            .AutoHeight()
            [
                SNew(SBorder)
                .BorderImage(FAppStyle::Get().GetBrush("EditorViewport.OverlayBrush"))
                .Padding(8.f)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Bottom)
                    .HAlign(HAlign_Right)
                    .Padding(FMargin(0.0, 8.0, 0.0, 0.0))
                    [
                        prevBtn.ToSharedRef()
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SBorder)
                        .BorderImage(FAppStyle::Get().GetBrush("EditorViewport.OverlayBrush"))
                        .Padding(FMargin(2.0, 0.0, 2.0, 0.0))
                        [
                            SNew(SHorizontalBox)
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Center)
                            [
                                prevListView.ToSharedRef()
                            ]
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Center)
                            [
                                currentView.ToSharedRef()
                            ]
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Center)
                            [
                                nextListView.ToSharedRef()
                            ]
                        ]
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Bottom)
                    .HAlign(HAlign_Left)
                    .Padding(FMargin(0.0, 8.0, 0.0, 0.0))
                    [
                        nextBtn.ToSharedRef()
                    ]
                ]
            ]
            + SVerticalBox::Slot()
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Bottom)
            .AutoHeight()
            [
                SNew(SBorder)
                .BorderImage(FAppStyle::Get().GetBrush("EditorViewport.OverlayBrush"))
                .Padding(8.f)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    .Padding(FMargin(0.0, 0.0, 8.0, 0.0))
                    [
                        SNew(SImage)
                        .Image(FSlateIconFinder::FindIcon("StoryBoardEditor.StoryBoardEdMode16").GetIcon())
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    .Padding(FMargin(0.0, 0.0, 8.0, 0.0))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString("Story Board"))
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(FMargin(2.0, 0.0, 0.0, 0.0))
                    [
                        SNew(SButton)
                        .ButtonStyle(FAppStyle::Get(), "PrimaryButton")
                        .TextStyle(FAppStyle::Get(), "DialogButtonText")
                        .Text(LOCTEXT("ExitEdit", "Exit"))
                        .ToolTipText(LOCTEXT("ExitTooltip", "Exit Story Board View"))
                        .HAlign(HAlign_Center)
                        .OnClicked_Lambda([&edSubsys]() {
                            edSubsys->ExitEdMode();
                            return FReply::Handled(); })
                    ]
                ]
            ]
        ];
}

FName FStoryBoardEdToolkit::GetToolkitFName() const {
    return FName("StoryBoardEdToolkit");
}

FText FStoryBoardEdToolkit::GetBaseToolkitName() const {
    return LOCTEXT("ToolkitDisplayName", "Story Board Editor Mode");
}

class FEdMode* FStoryBoardEdToolkit::GetEditorMode() const {
    return GLevelEditorModeTools().GetActiveMode(UStoryBoardEdMode::EM_StoryBoardEdModeId);
}

void FStoryBoardEdToolkit::RequestModeUITabs() {
    // do not draw default side bar
}

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreatePrevBtn() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    auto widget = SNew(SButton)
        .ButtonStyle(FAppStyle::Get(), "PrimaryButton")
        .TextStyle(FAppStyle::Get(), "DialogButtonText")
        .Text(LOCTEXT("Previous Node", "<"))
        .ToolTipText(LOCTEXT("PrevNodeTooltip", "Select Previous Node"))
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        .OnClicked_UObject(edSubsys, &UStoryBoardEditorSubsystem::PreviousNode);
     return widget;
}
TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateNextBtn() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    auto widget = SNew(SButton)
        .ButtonStyle(FAppStyle::Get(), "PrimaryButton")
        .TextStyle(FAppStyle::Get(), "DialogButtonText")
        .Text(LOCTEXT("Next Node", ">"))
        .ToolTipText(LOCTEXT("NextNodeTooltip", "Select Next Node"))
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        .OnClicked_UObject(edSubsys, &UStoryBoardEditorSubsystem::NextNode);
    return widget;
}

auto BFSPrevForScenario = [](AStoryNode* node) -> UStoryScenario* {
    TArray<AStoryNode*> stack {node};

    // bfs searching prevs for closest scenario
    while (!stack.IsEmpty()) {
        AStoryNode* curr = stack.Pop();
        if (curr->Scenario.Get()) {
            return curr->Scenario.Get();
        }
        for (auto prev : curr->PrevPoints) {
            stack.Add(prev.Get());
        }
    }

    // there is no scenario in all prev nodes
    return nullptr;
};

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateCurrnetNodeView() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    AStoryNode* node = edSubsys->StoryNodeHelper->SelectedNode.Get();
    UStoryScenario* nodeScenario = node ? node->Scenario.Get() : nullptr;

    FText btnName = FText::FromString("Empty");
    FString btnToolTip = FString("Add Scenario");
    const FSlateBrush* btnBrush = FSlateIconFinder::FindIcon("StoryBoardEditor.AddStoryScenario128").GetIcon();

    UStoryScenario* templateScenario = edSubsys->GetCurrentScenario();
    if (!templateScenario) {
        templateScenario = BFSPrevForScenario(node);
    }

    if (nodeScenario) {
        btnName = nodeScenario->Name;
        btnToolTip = FString("Edit Scenario");
        btnBrush = FSlateIconFinder::FindIcon("StoryBoardEditor.StoryBoardEdMode128").GetIcon();
    }
    
    auto widget = SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Bottom)
        .Padding(FMargin(0.0, 2.0, 0.0, 0.0))
        [
            SNew(STextBlock)
            .Text(FText::FromString(node->GetActorLabel()))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Bottom)
        .Padding(FMargin(0.0, 0.0, 0.0, 2.0))
        [
            SNew(SButton)
            .ButtonStyle(FAppStyle::Get(), "PrimaryButton")
            .TextStyle(FAppStyle::Get(), "DialogButtonText")
            .Text_Lambda([btnName]() {
                return btnName;
            })
            .ToolTipText_Lambda([btnToolTip]() {
                return FText::FromString(btnToolTip);
            })
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked_Lambda([edSubsys, node, nodeScenario, templateScenario]() {
                if (!nodeScenario) {
                    FString path = edSubsys->StoryAssetHelper->CreateScenario(templateScenario);
                    node->Scenario = LoadObject<UStoryScenario>(nullptr, *path);
                }

                if (node->Scenario.IsValid()) {
                    GEditor->EditObject(node->Scenario.Get());
                }
                
                edSubsys->UISelectNode(node);

                return FReply::Handled();
            })
            [
                SNew(SImage)
                .Image_Lambda([btnBrush]() {
                    return btnBrush;
                 })
            ]
        ];

    return widget;
}
TSharedPtr<SWidget> FStoryBoardEdToolkit::CreatePrevNodesView() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    AStoryNode* node = edSubsys->StoryNodeHelper->SelectedNode.Get();

    if (node) return CreateNodeListView(node->PrevPoints);

    return SNew(SBorder);
}

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateNextNodesView() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    AStoryNode* node = edSubsys->StoryNodeHelper->SelectedNode.Get();

    if (node) return CreateNodeListView(node->NextPoints);

    return SNew(SBorder);
}

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateNodeListView(const TArray<TObjectPtr<AStoryNode>>& List) {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    FMenuBuilder MenuBuilder(true, nullptr);
    for (auto item : List) {
        FSoftObjectPath Path = item->Scenario->GetPathName();
        MenuBuilder.AddMenuEntry(
            FText::FromString(item->GetActorLabel()),
            FText::FromString("Selet Node"),
            FSlateIconFinder::FindIconForClass(UStoryScenario::StaticClass()),
            FUIAction(
                FExecuteAction::CreateLambda([item, edSubsys]() {
                    edSubsys->UISelectNode(item);
                })
            ),
            NAME_None,
            EUserInterfaceActionType::Button);
    }
    return MenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE