#pragma once
#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"
 
#define LOCTEXT_NAMESPACE "StoryBoardCommands"
 
class FStoryBoardCommands : public TCommands<FStoryBoardCommands>
{
public:
    FStoryBoardCommands() :
        TCommands<FStoryBoardCommands>
        (
            "StoryBoardCommands", // Unique name of the commands set
            LOCTEXT("StoryBoardCommandsNames", "Story Board Commands"), // Human readable name (will be displayed in the editor preferences window)
            NAME_None, // Name of the parent commands set this one is extending (if any)
            FAppStyle::GetAppStyleSetName()) // Name of the style set from which command icons should be loaded (if any)
    {}
 
    // Commands should be declared as members
    TSharedPtr<FUICommandInfo> Btn_Trial;
    TSharedPtr<FUICommandInfo> Entry_Trial;
 
    // RegisterCommands should be overridden to define the commands
    void RegisterCommands() override
    {
        UI_COMMAND
        (
            Btn_Trial,
            "My custom button", // Label
            "Tooltip for my custom button", // Tooltip
            EUserInterfaceActionType::Button, // UI representation (when used to dynamically build toolbars and menus)
            FInputChord(EKeys::C, EModifierKey::Shift | EModifierKey::Alt) // keyboard shortcut (can be empty)
        );
 
        UI_COMMAND(Entry_Trial, "My custom entry", "Tooltip for my custom entry",
            EUserInterfaceActionType::Button, FInputChord());
    }
};
 
#undef LOCTEXT_NAMESPACE