#pragma once

#include "AssetTypeActions_Base.h"

class FStroyScenarioAssetActions : public FAssetTypeActions_Base {
public:
    static void RegisterAssetTypeActions();
    static void UnregisterAssetTypeActions();

    virtual FText GetName() const override { return FText::FromString("Story Scenario Asset"); }
    virtual FColor GetTypeColor() const override { return FColor::Emerald; }
    virtual UClass* GetSupportedClass() const override;
    virtual uint32 GetCategories() override { return EAssetTypeCategories::Misc; }
};