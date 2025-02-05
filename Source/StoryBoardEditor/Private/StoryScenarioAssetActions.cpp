#include "StoryScenarioAssetActions.h"
#include "StoryScenario.h"

void FStoryScenarioAssetActions::RegisterAssetTypeActions()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    AssetTools.RegisterAssetTypeActions(MakeShareable(new FStoryScenarioAssetActions));
}

void FStoryScenarioAssetActions::UnregisterAssetTypeActions()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    AssetTools.UnregisterAssetTypeActions(MakeShareable(new FStoryScenarioAssetActions));
}

UClass* FStoryScenarioAssetActions::GetSupportedClass() const
{
    return UStoryScenario::StaticClass();
}