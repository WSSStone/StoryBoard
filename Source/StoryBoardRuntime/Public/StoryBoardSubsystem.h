#pragma once

#include "StoryScenario.h"
#include "StoryNode.h"

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Subsystems/WorldSubsystem.h"

#include "StoryBoardSubsystem.generated.h"

class FStoryNodeHelper;

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

    void HandleInitializedActors(const FActorsInitializedParams&);

    TUniquePtr<FStoryNodeHelper> StoryNodeHelper;

private:
    FString DefaultScenarioPath;
};

/*
    Transient class help building bi-directional graph
    Lifecycle: with StoryBoardSubsystem
*/
struct STORYBOARDRUNTIME_API FStoryNodeWrapper {
    FStoryNodeWrapper(AStoryNode* Node) : Node(Node) {}
    ~FStoryNodeWrapper() {}
    TSoftObjectPtr<AStoryNode> Node;
    TArray<FStoryNodeWrapper*> PrevNodes;
    TArray<FStoryNodeWrapper*> NextNodes;
};

/*
    Helper class finding StoryNode's initial StoryScenario in game world.
    Lifecycle: in game world
*/
class STORYBOARDRUNTIME_API FStoryNodeHelper {
public:
    FStoryNodeHelper();
    virtual ~FStoryNodeHelper();

    void Initialize(UWorld* World);

    // Check all story nodes in world, and make delegate bindings.
    virtual void AllocateStoryNodes(UWorld* World);

    virtual void BuildGraph();

    virtual UStoryScenario* BFSNearestPrevScenario();

    virtual UStoryScenario* BFSNearestPrevScenario(AStoryNode* Node);

    TArray<TSoftObjectPtr<AStoryNode>> StoryNodes;

    TMap<AStoryNode*, FStoryNodeWrapper> StoryNodeWrappers;

private:
    TSoftObjectPtr<AStoryNode> StartPoint;
};