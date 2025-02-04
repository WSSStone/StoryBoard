# pragma once

#include "CoreMinimal.h"
#include "StoryBoardSubsystem.h"
#include "LevelEditor.h"
#include "EditorSubsystem.h"

#include "StoryBoardEditorSubsystem.generated.h"

UCLASS()
class STORYBOARDEDITOR_API UStoryBoardEditorSubsystem : public UEditorSubsystem {
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    void SetupDefaultScenario();

    UFUNCTION(BlueprintCallable)
    void SetupScenario(UStoryScenario* StoryScenario);

    UFUNCTION(BlueprintCallable)
    UStoryScenario* GetDefaultScenario();

    UFUNCTION(BlueprintCallable)
    UStoryScenario* GetCurrentScenario();

    UFUNCTION(BlueprintCallable)
    void SetDefaultScenario(UStoryScenario* in);

    void ExecuteCommands(const TArray<FStatusCommand>& ConsoleCommands);

    void SetupWeather(const FWeatherStatus& WeatherStatus);

    // Setup data layer editor status: bEditorVisible, bLoaded
    void SetupDataLayerStatus(const TArray<FDataLayerStatus>& DataLayerStatuses);

#pragma region Editor GUI
    void RegisterEntry();

    void OnLevelEditorCreatedEvent(TSharedPtr<ILevelEditor> Editor);

    UE_DEPRECATED(5.5, "No longer use this.")
    TSharedRef<SDockTab> SummonScenarioEditor();
#pragma endregion

#pragma region Scenario Change
    // listen to ed mode activate scenario.
    void OnScenarioChange(UStoryScenario* inp);
#pragma endregion
    
private:
    bool isEdMode { false };

    FORCEINLINE UStoryBoardSubsystem* GetStoryBoardSubsystem();

    UStoryBoardSubsystem* StoryBoardPtr;

    void HandleOnMapOpened(const FString& Filename, bool bAsTemplate);

    void OnCurrentLevelChanged(ULevel* InNewLevel, ULevel* InOldLevel, UWorld* InWorld);

    friend class UStoryBoardEdMode;
};