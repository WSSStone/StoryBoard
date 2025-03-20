using UnrealBuildTool;

public class StoryBoardEditor : ModuleRules
{
    public StoryBoardEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        // bUsePrecompiled = true;
        PrecompileForTargets = PrecompileTargetsType.Any;

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
				"StoryBoardUI"
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