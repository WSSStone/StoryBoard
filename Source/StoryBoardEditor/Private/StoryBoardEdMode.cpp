#include "StoryBoardEdMode.h"
#include "StoryBoardEditorSubsystem.h"

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
}

void UStoryBoardEdMode::Exit()
{
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    edSubsys->isEdMode = false;

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

#undef LOCTEXT_NAMESPACE

#define LOCTEXT_NAMESPACE "StoryBoardEditorModeToolkit"

FStoryBoardEdToolkit::FStoryBoardEdToolkit() {

}

FStoryBoardEdToolkit::~FStoryBoardEdToolkit() {
    if (IsHosted() && ToolkitWidget.IsValid()) {
        GetToolkitHost()->RemoveViewportOverlayWidget(ToolkitWidget.ToSharedRef());
    }
}

void FStoryBoardEdToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode) {
    FModeToolkit::Init(InitToolkitHost, InOwningMode);
    
    SAssignNew(ToolkitWidget, SVerticalBox)
    + SVerticalBox::Slot()
    .Padding(5)
    .AutoHeight()
    [
        SNew(STextBlock)
        .Text(FText::FromString(TEXT("My Custom Edit Mode UI")))
    ]
    + SVerticalBox::Slot()
    .Padding(5)
    .AutoHeight()
    [
        SNew(SButton)
        .Text(FText::FromString(TEXT("Click Me")))
        .OnClicked_Lambda([]() {
            UE_LOG(LogTemp, Warning, TEXT("Button Clicked!"));
            return FReply::Handled(); })
    ];

    
    // GetToolkitHost()->AddViewportOverlayWidget(ToolkitWidget.ToSharedRef());
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

TSharedPtr<SWidget> FStoryBoardEdToolkit::GetInlineContent() const {
    return ToolkitWidget.IsValid() ? ToolkitWidget : SNullWidget::NullWidget;
}

#undef LOCTEXT_NAMESPACE