# pragma once

#include "StoryBoardSubsystem.h"
#include "StoryNode.h"

#include "CoreMinimal.h"
#include "LevelEditor.h"
#include "EditorSubsystem.h"

#include "StoryBoardEditorSubsystem.generated.h"

class FStoryNodeHelper;
class FStoryAssetHelper;

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
    void OnScenarioChange(UStoryScenario* inp);

    // helper objects
    FORCEINLINE void CreateStoryAssetHelper();
    FORCEINLINE void RemoveStoryAssetHelper();
    FORCEINLINE void CreateStoryNodeHelper();
    FORCEINLINE void RemoveStoryNodeHelper();

    TUniquePtr<FStoryNodeHelper> StoryNodeHelper;
    TUniquePtr<FStoryAssetHelper> StoryAssetHelper;
    
private:
    bool isEdMode { false };

    TSoftObjectPtr<UStoryScenario> CurrentScenario;

    UStoryBoardSubsystem* StoryBoardPtr;

    void HandleOnMapOpened(const FString& Filename, bool bAsTemplate);

    void OnCurrentLevelChanged(ULevel* InNewLevel, ULevel* InOldLevel, UWorld* InWorld);

    friend class UStoryBoardEdMode;
};

/*
    Helper class dealing with StoryNode in editor world.
    Lifecycle: StoryBoardEdMode [enter, exit]
*/
class FStoryNodeHelper {
public:
    FStoryNodeHelper(UWorld* World);
    ~FStoryNodeHelper();

    void OnStoryNodeAddedOrRemoved();

    void AllocateStoryNodes(UWorld* World);

    TSoftObjectPtr<AStoryNode> SelectedNode;

    TArray<TSoftObjectPtr<AStoryNode>> StoryNodes;
};

/*
    Helper class dealing with StoryScenario asset.
    Lifecycle: UStoryBoardEditorSubsystem [init, deinit]
*/
class FStoryAssetHelper {
public:
    FStoryAssetHelper() = default;
    ~FStoryAssetHelper() = default;
    /*
        Create a scenario asset in TargetPath and assign it to Node.
        Scenario template priority: current scenario > nearest PrevNode > map default scenario > empty scenario.
    */
    FString CreateScenario(UStoryScenario* TemplateScenario);
private:
    void RaiseSaveAssetWindow(FString& AssetPath);
};