#include "StoryBoardEdMode.h"
#include "StoryBoardEditorSubsystem.h"
#include "StoryBoardEdToolkit.h"

#include "Selection.h"

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
    edSubsys->EdNodeSelectedEvent.AddRaw(static_cast<FStoryBoardEdToolkit*>(Toolkit.Get()), &FStoryBoardEdToolkit::OnNodeSelected);
    edSubsys->isEdMode = true;
}

void UStoryBoardEdMode::Exit()
{
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    edSubsys->isEdMode = false;
    edSubsys->EdNodeSelectedEvent.RemoveAll(Toolkit.Get());
    edSubsys->OnExitEdMode();

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
    TArray<UObject*> selectedActors;
    GEditor->GetSelectedActors()->GetSelectedObjects(AStoryNode::StaticClass(), selectedActors);
    if (!selectedActors.IsEmpty()) {
        auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
        edSubsys->SetCurrentNode(Cast<AStoryNode>(selectedActors[0]));
    }

    UEdMode::ActorSelectionChangeNotify();
}

#undef LOCTEXT_NAMESPACE
