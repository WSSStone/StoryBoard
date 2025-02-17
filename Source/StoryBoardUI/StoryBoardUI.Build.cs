using UnrealBuildTool;

public class StoryBoardUI : ModuleRules
{
    public StoryBoardUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "RHI",
                "RenderCore",
                "Renderer",
                "StoryBoardRuntime"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
				"InputCore",
                "Slate",
                "SlateCore",
                "EnhancedInput",
				"Projects"
			}
        );

        // if is build with editor
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "UnrealEd",
                    "EditorStyle",
                    "PropertyEditor",
                    "LevelEditor",
                    "EditorWidgets",
                    "AssetTools",
                    "Kismet",
                    "KismetWidgets",
                    "GraphEditor",
                    "BlueprintGraph",
                    "ToolMenus",
                }
            );
        }
	}
}