#include "StoryBoardEditorModule.h"
#include "StoryBoardCommands.h"
#include "StoryBoardEditorSubsystem.h"
#include "StoryScenarioAssetActions.h"
#include "StoryBoardEditorStyle.h"

#define LOCTEXT_NAMESPACE "StoryBoardEditorModule"

void FStoryBoardEditorModule::StartupModule()
{
    FStoryBoardEditorStyle::Initialize();

    FStoryBoardCommands::Register();

    FStoryScenarioAssetActions::RegisterAssetTypeActions();
}

void FStoryBoardEditorModule::ShutdownModule()
{
    FStoryScenarioAssetActions::UnregisterAssetTypeActions();

    FStoryBoardCommands::Unregister();

    FStoryBoardEditorStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStoryBoardEditorModule, StoryBoardEditor)