#include "StoryBoardEdMode.h"
#include "StoryBoardEditorSubsystem.h"
#include "StoryBoardEdToolkit.h"
#include "StoryBoardEditorStyle.h"

#include "Selection.h"

#define LOCTEXT_NAMESPACE "StoryBoardEditorMode"

DEFINE_LOG_CATEGORY_STATIC(LogStoryBoardEditorMode, Log, All);

const FEditorModeID UStoryBoardEdMode::EM_StoryBoardEdModeId = TEXT("EM_StoryBoardEdMode");

UStoryBoardEdMode::UStoryBoardEdMode()
    : UEdMode() {
    Info = FEditorModeInfo(UStoryBoardEdMode::EM_StoryBoardEdModeId,
        LOCTEXT("DirectorMode", "Story Board"),
        FSlateIcon(FStoryBoardEditorStyle::GetStyleSetName(), "StoryBoardEditor.StoryBoardEdMode20"),
        true);
}

UStoryBoardEdMode::~UStoryBoardEdMode() {}

void UStoryBoardEdMode::Enter() {
    UEdMode::Enter();

    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    edSubsys->EdNodeSelectedEvent.AddRaw(static_cast<FStoryBoardEdToolkit*>(Toolkit.Get()), &FStoryBoardEdToolkit::OnNodeSelectedRedraw);
    edSubsys->isEdMode = true;
    GEditor->Exec(edSubsys->GetWorld(), TEXT("SCAct.DrawMovePoint 1"));
}

void UStoryBoardEdMode::Exit() {
    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
    GEditor->Exec(edSubsys->GetWorld(), TEXT("SCAct.DrawMovePoint 0"));
    edSubsys->isEdMode = false;
    edSubsys->EdNodeSelectedEvent.RemoveAll(Toolkit.Get());
    edSubsys->OnExitEdMode();

    UEdMode::Exit();
}

bool UStoryBoardEdMode::UsesToolkits() const {
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
