using UnrealBuildTool;

public class StoryBoardSceneViewExtension : ModuleRules
{
    public StoryBoardSceneViewExtension(ReadOnlyTargetRules Target) : base(Target)
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
                    "Slate",
                    "SlateCore",
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