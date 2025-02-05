#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FStoryBoardEditorStyle
{
public:
    static FString InContent(const FString& RelativePath, const ANSICHAR* Extension);

    static FString InResource(const FString& RelativePath, const ANSICHAR* Extension);

    static void Initialize();
    
    static void Shutdown();
    
    static void ReloadTextures(); 

    static const ISlateStyle& Get();

    static FName GetStyleSetName();

private:
    static TSharedPtr<FSlateStyleSet> StyleSet;

    static TSharedRef<class FSlateStyleSet> Create();
};