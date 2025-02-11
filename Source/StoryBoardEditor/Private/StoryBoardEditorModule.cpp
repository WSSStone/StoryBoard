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

    FStoryScenarioAssetActions::RegisterAssetTypeActions();

    UStoryBoardEditorSettings* Settings = GetMutableDefault<UStoryBoardEditorSettings>();
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
    if (SettingsModule) {
        SettingsModule->RegisterSettings("Project",
            TEXT("Plugins"),
            TEXT("Story Board"),
            LOCTEXT("StoryBoardEditorDisplayName", "Story Board"),
            LOCTEXT("StoryBoardEditorDescription", "Configure Story Board Settings"),
            Settings);
    }
}

void FStoryBoardEditorModule::ShutdownModule() {
    if (!FSlateApplication::IsInitialized()) {
        return;
    }

    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
    if (SettingsModule) {
        SettingsModule->UnregisterSettings("Project", TEXT("Plugins"), TEXT("Story Board"));
    }

    FStoryScenarioAssetActions::UnregisterAssetTypeActions();

    FEditorModeRegistry::Get().UnregisterMode(FName(TEXT("StoryBoardEditMode")));

    FStoryBoardEditorStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStoryBoardEditorModule, StoryBoardEditor)