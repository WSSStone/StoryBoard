#pragma once

#include "StoryBoardEdMode.h"
#include "EditorModeManager.h"
#include "LevelEditor.h"

#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"
 
#define LOCTEXT_NAMESPACE "StoryBoardCommands"
 
class FStoryBoardCommands : public TCommands<FStoryBoardCommands>
{
public:
    FStoryBoardCommands() :
        TCommands<FStoryBoardCommands>
        (
            "StoryBoardEditorCommands", // Unique name of the commands set
            LOCTEXT("StoryBoardEditorCommandsNames", "Story Board Editor Commands"), // Human readable name (will be displayed in the editor preferences window)
            NAME_None, // Name of the parent commands set this one is extending (if any)
            FAppStyle::GetAppStyleSetName()) // Name of the style set from which command icons should be loaded (if any)
    {}
 
    // Commands should be declared as members
    TSharedPtr<FUICommandInfo> StoryModeEntry;
 
    // RegisterCommands should be overridden to define the commands
    void RegisterCommands() override
    {
        UI_COMMAND
        (
            StoryModeEntry,
            "Activate Story Board Mode", // Label
            "Enter Story Board Mode", // Tooltip
            EUserInterfaceActionType::Button, // UI representation (when used to dynamically build toolbars and menus)
            FInputChord(EKeys::Zero, EModifierKey::Shift) // keyboard shortcut (can be empty)
        );

        const FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
        LevelEditorModule.GetGlobalLevelEditorActions()->MapAction(
            StoryModeEntry,
            FExecuteAction::CreateLambda([]() {
                GLevelEditorModeTools().ActivateMode(UStoryBoardEdMode::EM_StoryBoardEdModeId);
                }),
            FCanExecuteAction()
        );
    }
};
 
#undef LOCTEXT_NAMESPACE