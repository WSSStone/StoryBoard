#include "StoryScenarioAssetActions.h"
#include "StoryScenario.h"

void FStroyScenarioAssetActions::RegisterAssetTypeActions()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    AssetTools.RegisterAssetTypeActions(MakeShareable(new FStroyScenarioAssetActions));
}

void FStroyScenarioAssetActions::UnregisterAssetTypeActions()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    AssetTools.UnregisterAssetTypeActions(MakeShareable(new FStroyScenarioAssetActions));
}

UClass* FStroyScenarioAssetActions::GetSupportedClass() const
{
    return UStoryScenario::StaticClass();
}