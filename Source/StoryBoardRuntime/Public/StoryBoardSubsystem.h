#pragma once

#include "StoryScenario.h"

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "StoryBoardSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeatherStatusDelegate, const FWeatherParams&, WeatherParams);

UCLASS()
class STORYBOARDRUNTIME_API UStoryBoardSubsystem : public UWorldSubsystem {
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    void SetupScene(UStoryScenario* SceneSetting);

    UFUNCTION(BlueprintCallable)
    void SetDefaultScenario(UStoryScenario* in);

    UPROPERTY(BlueprintReadOnly)
    UStoryScenario* DefaultScenario;

    UPROPERTY(BlueprintReadOnly)
    UStoryScenario* CurrentScenario;

    UPROPERTY(BlueprintAssignable)
    FWeatherStatusDelegate OnWeatherStatusChanged;

    void ExecuteCommands(const TArray<FStatusCommand>& ConsoleCommands);

    void SetupActorVisibilities(const TArray<FActorVisibility>& ActorVisibilities);

    void SetupWeather(const FWeatherStatus& WeatherStatus);

    void SetupDataLayerStatus(const TArray<FDataLayerStatus>& DataLayerStatuseUEditorSubsystems);

    bool TryLoadDefaultScenario(const FString& WorldName);

private:
    FString DefaultScenarioPath;
};