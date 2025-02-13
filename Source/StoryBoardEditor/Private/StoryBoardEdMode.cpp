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

    UStoryBoardEditorSubsystem::EdSetCurrentNodeEvent.AddRaw(static_cast<FStoryBoardEdToolkit*>(Toolkit.Get()), &FStoryBoardEdToolkit::OnNodeSelectedRedraw);
}

void UStoryBoardEdMode::Exit() {
    UStoryBoardEditorSubsystem::EdSetCurrentNodeEvent.RemoveAll(Toolkit.Get());

    auto edSubsys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
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
        auto actor = selectedActors[0];
        if (selectedActors[0] != edSubsys->StoryNodeHelper->SelectedNode.Get()) {
            edSubsys->SetCurrentNode(Cast<AStoryNode>(actor));
        }
    }

    UEdMode::ActorSelectionChangeNotify();
}

#undef LOCTEXT_NAMESPACE
