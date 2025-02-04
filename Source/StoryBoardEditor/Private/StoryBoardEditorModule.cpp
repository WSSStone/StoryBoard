#include "StoryBoardEditorModule.h"
#include "StoryBoardCommands.h"
#include "StoryBoardEditorSubsystem.h"
#include "StoryScenarioAssetActions.h"

#define LOCTEXT_NAMESPACE "StoryBoardEditorModule"

void FStoryBoardEditorModule::StartupModule()
{
    FStoryBoardCommands::Register();

    FStroyScenarioAssetActions::RegisterAssetTypeActions();
}

void FStoryBoardEditorModule::ShutdownModule()
{
    FStroyScenarioAssetActions::UnregisterAssetTypeActions();

    FStoryBoardCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStoryBoardEditorModule, StoryBoardEditor)