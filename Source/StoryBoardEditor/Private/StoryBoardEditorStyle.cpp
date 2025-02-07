#include "StoryBoardEditorStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleMacros.h"

#define RESOURCE_BRUSH(RelativePath, ...) FSlateImageBrush(FStoryBoardEditorStyle::InResource(RelativePath, ".png"), __VA_ARGS__)
#define RootToContentDir StyleSet->RootToContentDir

TSharedPtr<FSlateStyleSet> FStoryBoardEditorStyle::StyleSet = nullptr;
const FVector2D Icon16x16 {16.f, 16.f};
const FVector2D Icon20x20 {20.f, 20.f};
const FVector2D Icon128x128 {128.0f, 128.0f};

FString FStoryBoardEditorStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension) {
    static FString contentDir = IPluginManager::Get().FindPlugin("StoryBoard")->GetContentDir();
    return (contentDir / RelativePath) + Extension;
}

FString FStoryBoardEditorStyle::InResource(const FString& RelativePath, const ANSICHAR* Extension) {
    static FString resourcesDir = IPluginManager::Get().FindPlugin("StoryBoard")->GetBaseDir() / TEXT("Resources");
    return (resourcesDir / RelativePath) + Extension;
}

void FStoryBoardEditorStyle::Initialize() {
    if (!StyleSet.IsValid()) {
        StyleSet = Create();

        FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
    }
}

void FStoryBoardEditorStyle::Shutdown() {
    FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);

    ensure(StyleSet.IsUnique());

    StyleSet.Reset();
}

FName FStoryBoardEditorStyle::GetStyleSetName() {
    static FName StyleSetName(TEXT("StoryBoardEditorStyle"));

    return StyleSetName;
}

TSharedRef<FSlateStyleSet> FStoryBoardEditorStyle::Create() {
    TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("StoryBoardEditorStyle"));

    {
        Style->Set("StoryBoardEditor.StoryBoardEdMode16", new RESOURCE_BRUSH("Icon16", Icon16x16));
        Style->Set("StoryBoardEditor.StoryBoardEdMode20", new RESOURCE_BRUSH("Icon20", Icon20x20));
        Style->Set("StoryBoardEditor.StoryBoardEdMode128", new RESOURCE_BRUSH("Icon128", Icon128x128));

        Style->Set("StoryBoardEditor.AddStoryScenario16", new RESOURCE_BRUSH("AddIcon16", Icon16x16));
        Style->Set("StoryBoardEditor.AddStoryScenario20", new RESOURCE_BRUSH("AddIcon20", Icon20x20));
        Style->Set("StoryBoardEditor.AddStoryScenario128", new RESOURCE_BRUSH("AddIcon128", Icon128x128));
    }

    return Style;
}

void FStoryBoardEditorStyle::ReloadTextures() {
    if (FSlateApplication::IsInitialized()) {
        FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
    }
}

const ISlateStyle& FStoryBoardEditorStyle::Get() {
    return *StyleSet;
}

#undef RESOURCE_BRUSH