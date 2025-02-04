using UnrealBuildTool;

public class StoryBoardRuntime : ModuleRules
{
    public StoryBoardRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "InputCore",
                "EnhancedInput",
			}
        );

        if (Target.bBuildEditor)
        {
            PublicDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd",
            });
        }
	}
}