# pragma once

#include "StoryBoardEditorSettings.h"
#include "StoryNode.h"
#include "StoryBoardSubsystem.h"
#include "DelegatesDefinitions.h"

#include "CoreMinimal.h"
#include "LevelEditor.h"
#include "EditorSubsystem.h"
#include "Math/Color.h"
#include "Math/MathFwd.h"

#include "StoryBoardEditorSubsystem.generated.h"

class FStoryNodeEditorHelper;
class FStoryAssetHelper;
class FStoryBoardViewportDrawer;

using FNodeSelectedEvent = FStoryNodeWrapperDelegate;
using FSetHintNodeEvent = FStoryNodeWrapperDelegate;

UCLASS()
class STORYBOARDEDITOR_API UStoryBoardEditorSubsystem : public UEditorSubsystem, public FEditorUndoClient {
    GENERATED_BODY()
public:
    static FNodeSelectedEvent EdSetCurrentNodeEvent;
    static FSetHintNodeEvent EdSetHindNodeEvent;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

    virtual void PostUndo(bool bSuccess) override {};

    virtual void PostRedo(bool bSuccess) override {}

    virtual UWorld* GetWorld() const override;

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
    bool IsEdMode() { return isEdMode; }

    UFUNCTION(BlueprintCallable)
    void SetDefaultScenario(UStoryScenario* in);

    void ExecuteCommands(const TArray<FStatusCommand>& ConsoleCommands);
    
    // Setup data layer editor status: bEditorVisible, bLoaded
    void SetupDataLayerStatus(const TArray<FDataLayerStatus>& DataLayerStatuses);

    void SetupWeather(const FWeatherStatus& WeatherStatus);

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

    FReply FirstNode();
    FReply PreviousNode();
    FReply CurrentNode();
    FReply NextNode();
    FReply LastNode();
    FReply UISelectNode(AStoryNode* Node, bool bApplyScenario = true);

    void SetCurrentNode(AStoryNode* Node, bool bApplyScenario = true);
    void SetCurrentScenario(UStoryScenario* Scenario);

    void HandleNodeScenarioChange(AStoryNode* Node);
    void HandleNodeNextPointsChange(AStoryNode* Node);
    void OnScenarioPropChange(UStoryScenario* inp);

    // helper objects
    FORCEINLINE void CreateStoryAssetHelper();
    FORCEINLINE void RemoveStoryAssetHelper();
    FORCEINLINE void CreateStoryNodeHelper();
    FORCEINLINE void RemoveStoryNodeHelper();
    FORCEINLINE void CreateStoryBoardViewportDrawer();
    FORCEINLINE void RemoveStoryBoardViewportDrawer();

    void SetHintNode(AStoryNode* Node);
    void RemoveHintNode();

    TUniquePtr<FStoryNodeEditorHelper> StoryNodeHelper;
    TUniquePtr<FStoryAssetHelper> StoryAssetHelper;
    
private:
    bool isEdMode {false};

    TUniquePtr<FStoryBoardViewportDrawer> StoryBoardViewportDrawer;

    TObjectPtr<UStoryScenario> CurrentScenario;

    TObjectPtr<UStoryBoardSubsystem> StoryBoardPtr;

    void HandleEditorBeginPIE(bool);

    void HandleOnMapOpened(const FString& Filename, bool bAsTemplate);

    void OnCurrentLevelChanged(ULevel* InNewLevel, ULevel* InOldLevel, UWorld* InWorld);
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

class FStoryNodeEditorHelper : public FStoryNodeHelper {
public:
    FStoryNodeEditorHelper(UWorld* World);
    virtual ~FStoryNodeEditorHelper();

    // Check all story nodes in world, and make delegate bindings.
    virtual void AllocateStoryNodes(UWorld* World) override;

    void ReallocateStoryNodes(UWorld* World);

    void OnStoryNodeAddedOrRemoved();

    virtual UStoryScenario* BFSNearestPrevScenario() override;

    FStoryNodeWrapper* BFSFurthestWrapper(FStoryNodeWrapper* Wrapper, bool bPrev = true);

    void GetPrevStoryNodes(TArray<TObjectPtr<AStoryNode>>& Ret);

    void GetNextStoryNodes(TArray<TObjectPtr<AStoryNode>>& Ret);

    void GetImmeidateNodes(TArray<TObjectPtr<AStoryNode>>& Ret, AStoryNode* Node, bool bParent = true);

    TObjectPtr<AStoryNode> SelectedNode;
};

/*
    Helper class draw viewport elements in StoryBoardEdMode
*/
class FStoryBoardViewportDrawer : public FTickableEditorObject {
public:
    FStoryBoardViewportDrawer(UStoryBoardEditorSubsystem* Owner);

    FStoryBoardViewportDrawer(UStoryBoardEditorSubsystem* Owner,
        const FDrawAttribute& PrevAttrib,
        const FDrawAttribute& NextAttrib,
        const FDrawAttribute& HintAttrib);

    ~FStoryBoardViewportDrawer();

    virtual void Tick(float DeltaTime) override;

    bool IsTickable() const override;

    virtual TStatId GetStatId() const override;

    TObjectPtr<AStoryNode> HintNode;

private:
    void DrawEdges();

    void DrawHint();

    TObjectPtr<UStoryBoardEditorSubsystem> Owner;

    UWorld* World;

    FDrawAttribute PrevAttrib;
    FDrawAttribute NextAttrib;
    FDrawAttribute HintAttrib;
};