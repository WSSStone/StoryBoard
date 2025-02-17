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
                "Slate",
                "SlateCore",
                "StoryBoardRuntime"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
				"InputCore",
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