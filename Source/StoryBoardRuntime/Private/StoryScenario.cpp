#include "StoryScenario.h"

DEFINE_LOG_CATEGORY_STATIC(LogStoryScenario, Log, All);

FStoryScenarioEvent UStoryScenario::OnStoryScenarioChanged;

#if WITH_EDITOR
void UStoryScenario::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
    Super::PostEditChangeProperty(PropertyChangedEvent);

    OnStoryScenarioChanged.ExecuteIfBound(this);
}
#endif