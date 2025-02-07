#include "StoryScenarioAssetActions.h"
#include "StoryScenario.h"
#include "ThumbnailRendering/SceneThumbnailInfo.h"

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

UThumbnailInfo* FStoryScenarioAssetActions::GetThumbnailInfo(UObject* Asset) const {
    UStoryScenario* scenario = Cast<UStoryScenario>(Asset);
    UThumbnailInfo* thumbnailInfo = NewObject<USceneThumbnailInfo>(scenario, NAME_None, RF_Transactional);

    return thumbnailInfo;
}