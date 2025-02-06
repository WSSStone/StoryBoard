#include "StoryScenario.h"

DEFINE_LOG_CATEGORY_STATIC(LogStoryScenario, Log, All);

#if WITH_EDITOR
void UStoryScenario::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
    Super::PostEditChangeProperty(PropertyChangedEvent);

    OnStoryScenarioChanged.ExecuteIfBound(this);
}
#endif