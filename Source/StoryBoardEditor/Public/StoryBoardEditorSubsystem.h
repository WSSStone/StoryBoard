# pragma once

#include "StoryBoardSubsystem.h"
#include "StoryNode.h"

#include "CoreMinimal.h"
#include "LevelEditor.h"
#include "EditorSubsystem.h"

#include "StoryBoardEditorSubsystem.generated.h"

class FStoryNodeHelper;

DECLARE_MULTICAST_DELEGATE_OneParam(FNodeSelectedEvent, AStoryNode*)

UCLASS()
class STORYBOARDEDITOR_API UStoryBoardEditorSubsystem : public UEditorSubsystem {
    GENERATED_BODY()
public:
    static FNodeSelectedEvent EdNodeSelectedEvent;

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

    // EdMode
    UE_DEPRECATED(5.4, "No longer standalone window. Use EditorMode instead.")
    void RegisterEntry();

    void OnLevelEditorCreatedEvent(TSharedPtr<ILevelEditor> Editor);

    UE_DEPRECATED(5.4, "No longer use this.")
    TSharedRef<SDockTab> SummonScenarioEditor();

    // callback when enter StoryBoardEdMode
    void OnEnterEdMode();
    // callback when exit StoryBoardEdMode
    void OnExitEdMode();

    // enter StoryBoardEdMode
    void EnterEdMode();
    // exit StoryBoardEdMode
    void ExitEdMode();

    FReply PreviousNode();
    FReply NextNode();
    FReply UISelectNode(AStoryNode* Node);

    void SetCurrentNode(AStoryNode* Node);
    void SetCurrentScenario(UStoryScenario* Scenario);

    // listen to ed mode selected/activated scenario.
    void OnScenarioChange(UStoryScenario* inp);

    void CreateStoryNodeHelper();

    TUniquePtr<FStoryNodeHelper> StoryNodeHelper;
    
private:
    bool isEdMode { false };

    TSoftObjectPtr<UStoryScenario> CurrentScenario;

    UStoryBoardSubsystem* StoryBoardPtr;

    void HandleOnMapOpened(const FString& Filename, bool bAsTemplate);

    void OnCurrentLevelChanged(ULevel* InNewLevel, ULevel* InOldLevel, UWorld* InWorld);

    friend class UStoryBoardEdMode;
};

class FStoryNodeHelper {
public:
    FStoryNodeHelper(UWorld* World);
    ~FStoryNodeHelper();

    void OnStoryNodeAddedOrRemoved();

    void AllocateStoryNodes(UWorld* World);

    TSoftObjectPtr<AStoryNode> SelectedNode;

    TArray<TSoftObjectPtr<AStoryNode>> StoryNodes;
};