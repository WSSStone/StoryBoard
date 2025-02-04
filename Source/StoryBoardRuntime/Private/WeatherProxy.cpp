#include "WeatherProxy.h"
#include "StoryBoardSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeatherHanlder, Log, All);

void AWeatherProxy::BeginPlay()
{
    Super::BeginPlay();

    if (UStoryBoardSubsystem* storyBoardSubsystem = GetWorld()->GetSubsystem<UStoryBoardSubsystem>())
    {
        storyBoardSubsystem->OnWeatherStatusChanged.AddDynamic(this, &AWeatherProxy::OnWeatherChange);
    }
}

void AWeatherProxy::OnWeatherChange_Implementation(const FWeatherParams& WeatherParams)
{
    UE_LOG(LogWeatherHanlder, Display, TEXT("HandleWeatherChange: (%d, %d, %d, %d), %f"),
        WeatherParams.TimeOfDay.Hours,
        WeatherParams.TimeOfDay.Minutes, 
        WeatherParams.TimeOfDay.Seconds, 
        WeatherParams.TimeOfDay.Frames, 
        WeatherParams.CloudCoverage);
}