#include "StoryBoardEditorModule.h"
#include "StoryBoardCommands.h"
#include "StoryBoardEditorSubsystem.h"
#include "StoryScenarioAssetActions.h"
#include "StoryBoardEditorStyle.h"

#include "ISettingsModule.h"

#include "EditorModeRegistry.h"

#define LOCTEXT_NAMESPACE "StoryBoardEditorModule"

void FStoryBoardEditorModule::StartupModule() {
    FStoryBoardEditorStyle::Initialize();

    // EditorMode is registered with LevelEditor's EdMode allocation

    FStoryBoardCommands::Register();

    FStoryScenarioAssetActions::RegisterAssetTypeActions();

    UStoryBoardEditorSettings* Settings = GetMutableDefault<UStoryBoardEditorSettings>();
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
    if (SettingsModule) {
        SettingsModule->RegisterSettings("Project", TEXT("Project"), TEXT("StoryBoardEditor"),
            LOCTEXT("StoryBoardEditor", "StoryBoardEditor"),
            LOCTEXT("StoryBoardEditor", "StoryBoardEditor"),
            Settings);
    }
}

void FStoryBoardEditorModule::ShutdownModule() {
    if (!FSlateApplication::IsInitialized()) {
        return;
    }

    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
    if (SettingsModule) {
        SettingsModule->UnregisterSettings("Project", TEXT("Project"), TEXT("StoryBoardEditor"));
    }

    FStoryScenarioAssetActions::UnregisterAssetTypeActions();

    FStoryBoardCommands::Unregister();

    FEditorModeRegistry::Get().UnregisterMode(FName(TEXT("StoryBoardEditMode")));

    FStoryBoardEditorStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStoryBoardEditorModule, StoryBoardEditor)