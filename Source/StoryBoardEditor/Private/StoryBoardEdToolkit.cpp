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
#include "ThumbnailRendering/ThumbnailManager.h"


#define LOCTEXT_NAMESPACE "StoryBoardEditorModeToolkit"

FStoryBoardEdToolkit::FStoryBoardEdToolkit() {

}

FStoryBoardEdToolkit::~FStoryBoardEdToolkit() {
    if (IsHosted() && ViewportOverlayWidget.IsValid()) {
        GetToolkitHost()->RemoveViewportOverlayWidget(ViewportOverlayWidget.ToSharedRef());
        ViewportOverlayWidget.Reset();
    }
}

void FStoryBoardEdToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode) {
    FModeToolkit::Init(InitToolkitHost, InOwningMode);

    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    edSubsys->OnEnterEdMode();

    ArrangeWidget();

    GetToolkitHost()->AddViewportOverlayWidget(ViewportOverlayWidget.ToSharedRef());
}

void FStoryBoardEdToolkit::OnNodeSelectedRedraw(FStoryNodeWrapper* Wrapper) {
    if (IsHosted() && ViewportOverlayWidget.IsValid()) {
        GetToolkitHost()->RemoveViewportOverlayWidget(ViewportOverlayWidget.ToSharedRef());
        ViewportOverlayWidget.Reset();
    }

    ArrangeWidget();

    GetToolkitHost()->AddViewportOverlayWidget(ViewportOverlayWidget.ToSharedRef());
}

void FStoryBoardEdToolkit::ArrangeWidget() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();

    auto currentView  = CreateCurrnetNodeView();
    auto nextListView = CreateNextNodesView();
    auto prevListView = CreatePrevNodesView();
    auto firstBtn     = CreateFirstBtn();
    auto prevBtn      = CreatePrevBtn();
    auto currBtn      = CreateCurrBtn();
    auto nextBtn      = CreateNextBtn();
    auto lastBtn      = CreateLastBtn();

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
            .Padding(0.0)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
#pragma region Scenario Icon View
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Bottom)
                .MaxHeight(159.0)
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
                        .Padding(FMargin(2.0, 0.0, 2.0, 0.0))
                        [
                            prevListView.ToSharedRef()
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Bottom)
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
#pragma endregion
                + SVerticalBox::Slot()
#pragma region Node Selection Action
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Top)
                .AutoHeight()
                [
                    SNew(SBorder)
                    .BorderImage(FAppStyle::Get().GetBrush("EditorViewport.OverlayBrush"))
                    .Padding(2.f)
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .HAlign(HAlign_Center)
                        .Padding(2.0)
                        [
                            firstBtn.ToSharedRef()
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .HAlign(HAlign_Center)
                        .Padding(2.0)
                        [
                            prevBtn.ToSharedRef()
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .HAlign(HAlign_Center)
                        .Padding(2.0)
                        [
                            currBtn.ToSharedRef()
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .HAlign(HAlign_Center)
                        .Padding(2.0)
                        [
                            nextBtn.ToSharedRef()
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .HAlign(HAlign_Center)
                        .Padding(2.0)
                        [
                            lastBtn.ToSharedRef()
                        ]
                    ]
                ]
#pragma endregion
            ]
        ]
        + SVerticalBox::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Bottom)
        .AutoHeight()
#pragma region Title Bar & Exit
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
#pragma endregion
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

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateFirstBtn() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    auto widget = SNew(SButton)
        .ButtonStyle(FAppStyle::Get(), "PrimaryButton")
        .TextStyle(FAppStyle::Get(), "DialogButtonText")
        .Text(LOCTEXT("First Node", "|<"))
        .ToolTipText(LOCTEXT("FirstNodeTooltip", "Select First Node"))
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        .DesiredSizeScale(FVector2D(1.0f, 1.0f))
        .OnClicked_UObject(edSubsys, &UStoryBoardEditorSubsystem::FirstNode);
    return widget;
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
        .DesiredSizeScale(FVector2D(1.0f, 1.0f))
        .OnClicked_UObject(edSubsys, &UStoryBoardEditorSubsystem::PreviousNode);
     return widget;
}

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateCurrBtn() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    auto widget = SNew(SButton)
        .ButtonStyle(FAppStyle::Get(), "PrimaryButton")
        .TextStyle(FAppStyle::Get(), "DialogButtonText")
        .Text(LOCTEXT("Current Node", "o"))
        .ToolTipText(LOCTEXT("CurrNodeTooltip", "Select and Apply Current Node"))
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        .DesiredSizeScale(FVector2D(1.0f, 1.0f))
        .OnClicked_UObject(edSubsys, &UStoryBoardEditorSubsystem::CurrentNode);
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
        .DesiredSizeScale(FVector2D(1.0f, 1.0f))
        .OnClicked_UObject(edSubsys, &UStoryBoardEditorSubsystem::NextNode);

    return widget;
}

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateLastBtn() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    auto widget = SNew(SButton)
        .ButtonStyle(FAppStyle::Get(), "PrimaryButton")
        .TextStyle(FAppStyle::Get(), "DialogButtonText")
        .Text(LOCTEXT("Last Node", ">|"))
        .ToolTipText(LOCTEXT("LastNodeTooltip", "Select Last Node"))
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        .DesiredSizeScale(FVector2D(1.0f, 1.0f))
        .OnClicked_UObject(edSubsys, &UStoryBoardEditorSubsystem::LastNode);
    return widget;
}

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateCurrnetNodeView() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    AStoryNode* node = edSubsys->StoryNodeHelper->SelectedNode.Get();
    if (!node) {
        return SNew(SBorder);
    }

    FText btnName = FText::FromString("Empty");
    FString btnToolTip = FString("Add Scenario");

    FString iconName = FString("StoryBoardEditor.AddStoryScenario128");
    const FSlateBrush* btnBrush = FSlateIconFinder::FindIcon(FName(iconName)).GetIcon();
    TSharedRef<SWidget> iconWidget = SNew(SImage)
        .Image_Lambda([btnBrush]() { return btnBrush; });

    UStoryScenario* nodeScenario = node->Scenario.Get();
    if (nodeScenario != nullptr) {
        btnName = nodeScenario->Name;
        btnToolTip = FString("Edit Scenario");
        iconWidget = CreateAssetThumbnailWidget(nodeScenario, int32(ImageSize::S128)).ToSharedRef();
    }

    UStoryScenario* templateScenario = edSubsys->StoryNodeHelper->BFSNearestPrevScenario();

    auto widget = SNew(SBorder)
        .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f))
        .OnMouseButtonDown_Lambda([edSubsys, node](const FGeometry&, const FPointerEvent&) {
            edSubsys->UISelectNode(node);
            return FReply::Handled();
        })
        [
            SNew(SVerticalBox)
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
                .TextStyle(FAppStyle::Get(), "DialogButtonText")
                .Text_Lambda([btnName]() { return btnName; })
                .ToolTipText_Lambda([btnToolTip]() { return FText::FromString(btnToolTip); })
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Bottom)
                .OnClicked_Lambda([edSubsys, node, nodeScenario, templateScenario]() {
                    if (!nodeScenario) {
                        FString path = edSubsys->StoryAssetHelper->CreateScenario(templateScenario);
                        node->Scenario = LoadObject<UStoryScenario>(nullptr, *path);
                    }
                    if (node->Scenario) {
                        GEditor->EditObject(node->Scenario.Get());
                    }
                    edSubsys->UISelectNode(node);
                    return FReply::Handled();
                })
                [
                    iconWidget
                ]
            ]
        ];

    return widget;
}

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateNodeView(AStoryNode* Node, ImageSize Size) {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();

    FString iconName = FString::Printf(TEXT("StoryBoardEditor.AddStoryScenario%d"), uint32(Size));
    const FSlateBrush* btnBrush = FSlateIconFinder::FindIcon(FName(iconName)).GetIcon();
    TSharedRef<SWidget> iconWidget = SNew(SImage)
        .Image_Lambda([btnBrush]() { return btnBrush; });

    UStoryScenario* nodeScenario = Node->Scenario.Get();
    if (nodeScenario != nullptr) {
        iconWidget = CreateAssetThumbnailWidget(nodeScenario, int(Size)).ToSharedRef();
    }

    UStoryScenario* templateScenario = edSubsys->StoryNodeHelper->BFSNearestPrevScenario();

    auto widget = SNew(SBox)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Bottom)
            .Padding(FMargin(0.0, 2.0, 0.0, 2.0))
            [
                SNew(STextBlock)
                .Text(FText::FromString(Node->GetActorLabel()))
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Bottom)
            .Padding(0.0)
            [
                iconWidget
            ]
        ];

    FNoReplyPointerEventHandler enterHandler;
    enterHandler.BindLambda([edSubsys, Node](const FGeometry&, const FPointerEvent&) {
        edSubsys->SetHintNode(Node);
    });

    widget.Get().SetOnMouseEnter(enterHandler);

    FSimpleNoReplyPointerEventHandler leaveHandler;
    leaveHandler.BindLambda([edSubsys](const FPointerEvent&) {
        edSubsys->RemoveHintNode();
    });
    widget.Get().SetOnMouseLeave(leaveHandler);

    return widget;
}

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateAssetThumbnailWidget(UObject* Asset, int32 Size) {
    if (!Asset) {
        return SNew(SBorder);
    }

    TSharedPtr<FAssetThumbnail> thumbnail = MakeShareable(new FAssetThumbnail(Asset, Size, Size, UThumbnailManager::Get().GetSharedThumbnailPool()));
    if (!thumbnail.IsValid()) {
        return SNew(SBorder);
    }

    FAssetThumbnailConfig thumbnailConfig;
    thumbnailConfig.bAllowFadeIn = false;
    thumbnailConfig.bAllowHintText = false;
    thumbnailConfig.bAllowRealTimeOnHovered = false;
    thumbnailConfig.bForceGenericThumbnail = false;

    return thumbnail->MakeThumbnailWidget(thumbnailConfig);
}

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreatePrevNodesView() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    if (AStoryNode* node = edSubsys->StoryNodeHelper->SelectedNode.Get()) {
        TArray<TObjectPtr<AStoryNode>> arr;
        edSubsys->StoryNodeHelper->GetPrevStoryNodes(arr);
        return CreateNodeListView(arr);
    }

    return SNew(SBorder);
}

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateNextNodesView() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    if (AStoryNode* node = edSubsys->StoryNodeHelper->SelectedNode.Get()) {
        TArray<TObjectPtr<AStoryNode>> arr;
        edSubsys->StoryNodeHelper->GetNextStoryNodes(arr);
        return CreateNodeListView(arr);
    }

    return SNew(SBorder);
}

TSharedPtr<SWidget> FStoryBoardEdToolkit::CreateNodeListView(const TArray<TObjectPtr<AStoryNode>>& List) {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    FMenuBuilder menuBuilder(true, nullptr);
    menuBuilder.SetStyle(&FAppStyle::Get(), "Menu");

    for (auto item : List) {
        if (item == nullptr) {
            continue;
        }

        FMenuEntryParams entryParams;

        entryParams.LabelOverride = FText::FromString(item->GetActorLabel());
        entryParams.ToolTipOverride = FText::FromString("Select Node");
        entryParams.IconOverride = FSlateIconFinder::FindIconForClass(UStoryScenario::StaticClass());
        entryParams.DirectActions = FUIAction(
            FExecuteAction::CreateLambda([item, edSubsys]() {
                edSubsys->UISelectNode(item);
            })
        );
        entryParams.ExtensionHook = NAME_None;
        entryParams.UserInterfaceActionType = EUserInterfaceActionType::Button;
        entryParams.EntryWidget = CreateNodeView(item.Get(), ImageSize::S20);

        menuBuilder.AddMenuEntry(entryParams);
    }

    return menuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE