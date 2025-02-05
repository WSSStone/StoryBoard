#include "StoryBoardEdMode.h"
#include "StoryBoardEditorSubsystem.h"

#include "Styling/SlateIconFinder.h"
#include "LevelInstance/LevelInstanceActor.h"

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

    TryExitEdModeEvent.AddUObject(edSubsys, &UStoryBoardEditorSubsystem::OnExitEdMode);
}

void UStoryBoardEdMode::Exit()
{
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    edSubsys->isEdMode = false;

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

    SAssignNew(ViewportOverlayWidget, SHorizontalBox)
        + SHorizontalBox::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Bottom)
        .Padding(FMargin(0.0f, 0.0f, 0.f, 15.f))
        [
            SNew(SBorder)
            .BorderImage(FAppStyle::Get().GetBrush("EditorViewport.OverlayBrush"))
            .Padding(8.f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                .Padding(FMargin(0.f, 0.f, 8.f, 0.f))
                [
                    SNew(SImage)
                    .Image(FSlateIconFinder::FindIconBrushForClass(ALevelInstance::StaticClass()))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                .Padding(FMargin(0.f, 0.f, 8.f, 0.f))
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Text Block"))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(2.0, 0.f, 0.f, 0.f))
                [
                    SNew(SButton)
                    .ButtonStyle(FAppStyle::Get(), "PrimaryButton")
                    .TextStyle(FAppStyle::Get(), "DialogButtonText")
                    .Text(LOCTEXT("ExitEdit", "Exit"))
                    .ToolTipText(LOCTEXT("ExitTooltip", "Exit Level Instance Edit"))
                    .HAlign(HAlign_Center)
                    .OnClicked_Lambda([&edSubsys]() {
                        edSubsys->OnExitEdMode();
                        return FReply::Handled(); })
                ]
            ]
        ];

    GetToolkitHost()->AddViewportOverlayWidget(ViewportOverlayWidget.ToSharedRef());
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