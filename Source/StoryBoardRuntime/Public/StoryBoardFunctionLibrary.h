#pragma once

#include "StoryScenario.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "StoryBoardFunctionLibrary.generated.h"

UCLASS()
class STORYBOARDRUNTIME_API UStoryBoardFunctionLibrary : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "StoryBoard")
    static void Lerp(UStoryScenario* A, UStoryScenario* B, float Alpha, UStoryScenario* out);
};