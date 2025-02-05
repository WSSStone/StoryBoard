#include "StoryBoardEdMode.h"
#include "StoryBoardEditorSubsystem.h"

#include "Styling/SlateIconFinder.h"
#include "LevelInstance/LevelInstanceActor.h"

#include "Widgets/Images/SImage.h"
#include "Slate/SlateBrushAsset.h"
#include "Engine/Texture2D.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "ObjectTools.h"

#define LOCTEXT_NAMESPACE "StoryBoardEditorMode"

DEFINE_LOG_CATEGORY_STATIC(LogStoryBoardEditorMode, Log, All);

const FEditorModeID UStoryBoardEdMode::EM_StoryBoardEdModeId = TEXT("EM_StoryBoardEdMode");

UStoryBoardEdMode::UStoryBoardEdMode()
    : UEdMode() {
    Info = FEditorModeInfo(UStoryBoardEdMode::EM_StoryBoardEdModeId,
        LOCTEXT("DirectorMode", "Story Board Edit Mode"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "DeveloperTools.MenuIcon"),
        true);
}

UStoryBoardEdMode::~UStoryBoardEdMode() {}

void UStoryBoardEdMode::Enter()
{
    UEdMode::Enter();

    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    edSubsys->isEdMode = true;
    edSubsys->EdActiveScenarioChangeEvent.BindStatic(Toolkit, &FStoryBoardEdToolkit::OnEdActiveScenarioChange);
    
    TryExitEdModeEvent.AddUObject(edSubsys, &UStoryBoardEditorSubsystem::OnExitEdMode);
}

void UStoryBoardEdMode::Exit()
{
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    edSubsys->isEdMode = false;
    edSubsys->EdActiveScenarioChangeEvent.Unbind();

    TryExitEdModeEvent.RemoveAll(edSubsys);

    UEdMode::Exit();
}

bool UStoryBoardEdMode::UsesToolkits() const
{
    return true;
}

void UStoryBoardEdMode::CreateToolkit() {
    Toolkit = MakeShared<FStoryBoardEdToolkit>();
}

void UStoryBoardEdMode::ActorSelectionChangeNotify() {
    UE_LOG(LogStoryBoardEditorMode, Display, TEXT("TODO: ActorSelectionChangeNotify"));
}

void UStoryBoardEdMode::BroadcastTryExitEdModeEvent() {
    TryExitEdModeEvent.Broadcast();
}

#undef LOCTEXT_NAMESPACE

#define LOCTEXT_NAMESPACE "StoryBoardEditorModeToolkit"

FStoryBoardEdToolkit::FStoryBoardEdToolkit() {

}

FStoryBoardEdToolkit::~FStoryBoardEdToolkit() {
    if (IsHosted() && ViewportOverlayWidget.IsValid()) {
        GetToolkitHost()->RemoveViewportOverlayWidget(ViewportOverlayWidget.ToSharedRef());
    }
}

void FStoryBoardEdToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode) {
    FModeToolkit::Init(InitToolkitHost, InOwningMode);

    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    edSubsys->EdActiveScenarioChangeEvent.BindRaw(this, &FStoryBoardEdToolkit::OnEdActiveScenarioChange);

    ArrangeWidget();

    GetToolkitHost()->AddViewportOverlayWidget(ViewportOverlayWidget.ToSharedRef());
}

void FStoryBoardEdToolkit::OnEdActiveScenarioChange() {
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
    detailView->SetObject(edSubsys->GetCurrentScenario());

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
                    .VAlign(VAlign_Center)
                    .HAlign(HAlign_Right)
                    .Padding(FMargin(0.0f, 8.0f, 0.0f, 0.0f))
                    [
                        SNew(SButton)
                        .ButtonStyle(FAppStyle::Get(), "PrimaryButton")
                        .TextStyle(FAppStyle::Get(), "DialogButtonText")
                        .Text(LOCTEXT("Previous Node", "<"))
                        .ToolTipText(LOCTEXT("PrevNodeTooltip", "Select Previous Scenario"))
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Center)
                        .OnClicked_Lambda([&edSubsys]() {
                            edSubsys->PreviousScenario();
                            return FReply::Handled(); })
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
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .VAlign(VAlign_Center)
                            [
                                detailView
                            ]
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .VAlign(VAlign_Center)
                            [
                                SNew(STextBlock)
                                .AccessibleText_Lambda([&edSubsys]() {
                                    return edSubsys->GetCurrentScenario() ? edSubsys->GetCurrentScenario()->Name : FText::FromString(""); })
                            ]
                        ]
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    .HAlign(HAlign_Left)
                    .Padding(FMargin(0.0f, 8.0f, 0.0f, 0.0f))
                    [
                        SNew(SButton)
                        .ButtonStyle(FAppStyle::Get(), "PrimaryButton")
                        .TextStyle(FAppStyle::Get(), "DialogButtonText")
                        .Text(LOCTEXT("Next Node", ">"))
                        .ToolTipText(LOCTEXT("NextNodeTooltip", "Select Next Scenario"))
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Center)
                        .OnClicked_Lambda([&edSubsys]() {
                            edSubsys->NextScenario();
                            return FReply::Handled();
                                })
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
                            edSubsys->OnExitEdMode();
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

#undef LOCTEXT_NAMESPACE