using UnrealBuildTool;

public class StoryBoardEditor : ModuleRules
{
    public StoryBoardEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "UnrealEd", 
				"AssetTools",
				"Kismet",
				"Core",
				"RenderCore",
				"RHI",
				"AssetRegistry",
                "EditorFramework",
                "StoryBoardRuntime",
                "StoryBoardSceneViewExtension"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
                "PropertyEditor",
                "UnrealEd",
                "AssetRegistry",
                "EditorStyle",
                "InputCore",
                "ContentBrowser",
                "ContentBrowserData",
                "ToolMenus",
                "Projects",
                "EditorSubsystem",
                "LevelEditor",
				"DataLayerEditor",
                "EnhancedInput",
			}
        );
	}
}