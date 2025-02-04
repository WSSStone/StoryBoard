#include "StoryBoardFunctionLibrary.h"

void UStoryBoardFunctionLibrary::Lerp(UStoryScenario* A, UStoryScenario* B, float Alpha, UStoryScenario* out) {
    FTimecode ta = A->WeatherStatus.WeatherParams.TimeOfDay;
    FTimecode tb = B->WeatherStatus.WeatherParams.TimeOfDay;
    FTimecode tr;
    tr.Hours = FMath::Lerp(ta.Hours, tb.Hours, Alpha);
    tr.Minutes = FMath::Lerp(ta.Minutes, tb.Minutes, Alpha);
    tr.Seconds = FMath::Lerp(ta.Seconds, tb.Seconds, Alpha);;
    out->WeatherStatus.WeatherParams.TimeOfDay = tr;

    float ca = A->WeatherStatus.WeatherParams.CloudCoverage;
    float cb = A->WeatherStatus.WeatherParams.CloudCoverage;
    out->WeatherStatus.WeatherParams.CloudCoverage = FMath::Lerp(ca, cb, Alpha);
}