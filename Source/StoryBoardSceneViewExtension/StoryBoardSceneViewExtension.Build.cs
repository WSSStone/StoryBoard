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
				"UnrealEd",
				"InputCore",
                "EnhancedInput",
				"Projects"
			}
        );
	}
}