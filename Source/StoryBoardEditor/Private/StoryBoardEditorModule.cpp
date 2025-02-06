#include "StoryBoardEditorModule.h"
#include "StoryBoardCommands.h"
#include "StoryBoardEditorSubsystem.h"
#include "StoryScenarioAssetActions.h"
#include "StoryBoardEditorStyle.h"

#include "EditorModeRegistry.h"

#define LOCTEXT_NAMESPACE "StoryBoardEditorModule"

void FStoryBoardEditorModule::StartupModule()
{
    FStoryBoardEditorStyle::Initialize();

    // EditorMode is registered with LevelEditor's EdMode allocation

    FStoryBoardCommands::Register();

    FStoryScenarioAssetActions::RegisterAssetTypeActions();
}

void FStoryBoardEditorModule::ShutdownModule()
{
    FStoryScenarioAssetActions::UnregisterAssetTypeActions();

    FStoryBoardCommands::Unregister();

    FEditorModeRegistry::Get().UnregisterMode(FName(TEXT("StoryBoardEditMode")));

    FStoryBoardEditorStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStoryBoardEditorModule, StoryBoardEditor)