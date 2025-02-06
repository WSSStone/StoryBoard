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
        .Padding(FMargin(0.0f, 0.0f, 0.0f, 15.0f))
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
                    .Padding(FMargin(0.0f, 8.0f, 0.0f, 0.0f))
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
                        .Padding(2.f)
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
#if 0
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .VAlign(VAlign_Center)
                            [
                                widget
                                // detailView
                            ]
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .VAlign(VAlign_Center)
                            [
                                SNew(STextBlock)
                                .AccessibleText_Lambda([&edSubsys]() {
                                    return edSubsys->GetCurrentScenario() ? edSubsys->GetCurrentScenario()->Name : FText::FromString(""); })
                            ]
#endif
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
                    .Padding(FMargin(0.0f, 8.0f, 0.0f, 0.0f))
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
                    .Padding(FMargin(0.0f, 0.0f, 8.0f, 0.0f))
                    [
                        SNew(SImage)
                        .Image(FSlateIconFinder::FindIcon("StoryBoardEditor.StoryBoardEdMode").GetIcon())
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    .Padding(FMargin(0.0f, 0.0f, 8.0f, 0.0f))
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString("Story Board"))
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(FMargin(2.0f, 0.0f, 0.0f, 0.0f))
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
TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateCurrnetNodeView() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    AStoryNode* node = edSubsys->StoryNodeHelper->SelectedNode.Get();
    UStoryScenario* scenario = edSubsys->GetCurrentScenario();
    
    auto widget = SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Bottom)
        .Padding(FMargin(0.0f, 2.0f, 0.0f, 0.0f))
        [
            SNew(STextBlock)
            .AccessibleText_Lambda([&node]() {
                return node ? FText::FromString(node->GetActorLabel()) : FText::FromString("");
            })
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Bottom)
        .Padding(FMargin(0.0f, 0.0f, 0.0f, 2.0f))
        [
            SNew(SButton)
            .ButtonStyle(FAppStyle::Get(), "PrimaryButton")
            .TextStyle(FAppStyle::Get(), "DialogButtonText")
            .Text(FText::FromString("--"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked_Lambda([edSubsys, node]() {
                edSubsys->UISelectNode(node);
                return FReply::Handled();
            })
            [
                SNew(SImage)
                .Image(FSlateIconFinder::FindIconForClass(UStoryScenario::StaticClass()).GetIcon())
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