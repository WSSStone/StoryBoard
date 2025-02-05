# pragma once

#include "CoreMinimal.h"
#include "StoryBoardSubsystem.h"
#include "LevelEditor.h"
#include "EditorSubsystem.h"

#include "StoryBoardEditorSubsystem.generated.h"

DECLARE_DELEGATE(FEdActiveScenarioChange)

UCLASS()
class STORYBOARDEDITOR_API UStoryBoardEditorSubsystem : public UEditorSubsystem {
    GENERATED_BODY()
public:
    FEdActiveScenarioChange EdActiveScenarioChangeEvent;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

    FORCEINLINE UStoryBoardSubsystem* GetStoryBoardSubsystem();

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

#pragma region Editor Mode
    UE_DEPRECATED(5.4, "No longer standalone window. Use EditorMode instead.")
    void RegisterEntry();

    void OnLevelEditorCreatedEvent(TSharedPtr<ILevelEditor> Editor);

    UE_DEPRECATED(5.4, "No longer use this.")
    TSharedRef<SDockTab> SummonScenarioEditor();

    void OnEnterEdMode();

    void OnExitEdMode();

    void PreviousScenario();

    void NextScenario();

    // listen to ed mode selected/activated scenario.
    void OnScenarioChange(UStoryScenario* inp);
#pragma endregion
    
private:
    bool isEdMode { false };

    TSoftObjectPtr<UStoryScenario> CurrentScenario;

    UStoryBoardSubsystem* StoryBoardPtr;

    void HandleOnMapOpened(const FString& Filename, bool bAsTemplate);

    void OnCurrentLevelChanged(ULevel* InNewLevel, ULevel* InOldLevel, UWorld* InWorld);

    TSharedPtr<SWidget> ViewportOverlayWidget;

    friend class UStoryBoardEdMode;
};