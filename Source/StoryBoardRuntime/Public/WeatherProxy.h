#pragma once

#include "CoreMinimal.h"
#include "StoryScenario.h"

#include "WeatherProxy.generated.h"

UCLASS()
class STORYBOARDRUNTIME_API AWeatherProxy : public AActor {
    GENERATED_BODY()
public:
    // Bind delegate in BeginPlay
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintNativeEvent)
    void OnWeatherChange(const FWeatherParams& WeatherParams);

protected:
    virtual void OnWeatherChange_Implementation(const FWeatherParams& WeatherParams);
};