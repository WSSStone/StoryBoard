#include "StoryBoardSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceNull.h"
#include "EngineUtils.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogStoryBoard, Log, All);

void UStoryBoardSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
    UWorld* world = GetWorld();
    if (!world->IsGameWorld()) {
        return;
    }

    StoryNodeHelper = MakeUnique<FStoryNodeHelper>();

    FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UStoryBoardSubsystem::HandleInitializedActors); // called after this world's actors are initialized

    // fall back to default scenario
    GConfig->GetString(
        *DefaultScenarioSectionName,
        *FString::Printf(TEXT("%s"), *world->GetMapName()),
        DefaultScenarioPath,
        GGameIni
    );

    TryLoadDefaultScenario(world->GetMapName());
    if (DefaultScenario) {
        SetupScene(DefaultScenario, EExecuteFlag::GAME);
    }
}

void UStoryBoardSubsystem::Deinitialize() {
    FWorldDelegates::OnWorldInitializedActors.RemoveAll(this);

    if (StoryNodeHelper.IsValid()) {
        StoryNodeHelper.Reset(nullptr);
    }

    OnWeatherStatusChanged.Clear();
    DefaultScenario = nullptr;
    CurrentScenario = nullptr;
}

void UStoryBoardSubsystem::HandleInitializedActors(const FActorsInitializedParams& InitParams) {
    StoryNodeHelper->Initialize(InitParams.World);

    auto scenario = StoryNodeHelper->BFSNearestPrevScenario();

    if (scenario) {
        SetupScene(scenario, EExecuteFlag::GAME);
        return;
    }
}

void UStoryBoardSubsystem::SetupScene(UStoryScenario* Scenario, EExecuteFlag ExecuteFlag) {
    if (Scenario == nullptr) {
        UE_LOG(LogStoryBoard, Warning, TEXT("Input Scenario is nullptr."));
        return;
    }

    CurrentScenario = Scenario;

    SetupDataLayerStatus(Scenario->DataLayerStatuses, ExecuteFlag);
    ExecuteCommands(Scenario->ConsoleCommands, ExecuteFlag);
    SetupActorVisibilities(Scenario->ActorVisibilities, ExecuteFlag);
    SetupWeather(Scenario->WeatherStatus);
}

void UStoryBoardSubsystem::SetDefaultScenario(UStoryScenario* in) {
    DefaultScenario = in;
    DefaultScenarioPath = DefaultScenario->GetPathName();
    GConfig->SetString(
        *DefaultScenarioSectionName,
        *FString::Printf(TEXT("%s"), *GetWorld()->GetMapName()),
        *DefaultScenarioPath,
        GGameIni
    );
}

void UStoryBoardSubsystem::ExecuteCommands(const TArray<FStatusCommand>& ConsoleCommands, EExecuteFlag ExecuteFlag) {
    UWorld* World = GetWorld();
    if (World == nullptr) {
        UE_LOG(LogStoryBoard, Warning, TEXT("Invalid World context while executing command"));
        return;
    }

    for (const FStatusCommand& cmd : ConsoleCommands) {
        if ((ExecuteFlag & cmd.ExecuteFlag) == EExecuteFlag::NONE || cmd.Command.IsEmpty()) {
            continue;
        }

        GEngine->Exec(World, *cmd.Command);
    }
}

void UStoryBoardSubsystem::SetupActorVisibilities(const TArray<FActorVisibility>& ActorVisibilities, EExecuteFlag ExecuteFlag) {
    for (const FActorVisibility& actorVisibility : ActorVisibilities) {
        AActor* actor = actorVisibility.Actor.Get();

        if ((ExecuteFlag & actorVisibility.ExecuteFlag) == EExecuteFlag::NONE || actor == nullptr) {
            continue;
        }

        actor->SetActorHiddenInGame(actorVisibility.bHiddenInGame);
    }
}

void UStoryBoardSubsystem::SetupWeather(const FWeatherStatus& WeatherStatus) {
    
    if (OnWeatherStatusChanged.IsBound()) {
        UE_LOG(LogStoryBoard, Display, TEXT("Broadcast OnWeatherStatusChanged!"));
        OnWeatherStatusChanged.Broadcast(WeatherStatus.WeatherParams);
    } else {
        UE_LOG(LogStoryBoard, Warning, TEXT("OnWeatherStatusChanged is not bound!"));
    }
}

void UStoryBoardSubsystem::SetupDataLayerStatus(const TArray<FDataLayerStatus>& DataLayerStatuses, EExecuteFlag ExecuteFlag) {
    UDataLayerManager* dataLayerManager = GetWorld()->GetDataLayerManager();
    if (dataLayerManager == nullptr) {
        UE_LOG(LogStoryBoard, Warning, TEXT("Current world does not have a DataLayerManager!"));
        return;
    }

    for (const FDataLayerStatus& dataLayerStatus : DataLayerStatuses) {
        const UDataLayerInstance* inst = dataLayerManager->GetDataLayerInstanceFromAsset(dataLayerStatus.DataLayerAsset.Get());

        if ((ExecuteFlag & dataLayerStatus.ExecuteFlag) == EExecuteFlag::NONE || inst == nullptr) {
            continue;
        }

        dataLayerManager->SetDataLayerInstanceRuntimeState(inst, dataLayerStatus.RuntimeState, false);
    }
}

bool UStoryBoardSubsystem::TryLoadDefaultScenario(const FString& WorldName) {
    DefaultScenario = LoadObject<UStoryScenario>(nullptr, *DefaultScenarioPath);
    CurrentScenario = DefaultScenario;

    if (DefaultScenario == nullptr) {
        UE_LOG(LogStoryBoard, Warning, TEXT("Failed to load DefaultSceneStatus config with config.ini: %s %s. Check if %s.uasset exists."),
            TEXT("/Script/CodeFlee.UStoryBoardSubsystem"),
            *FString::Printf(TEXT("%s"), *WorldName),
            *DefaultScenarioPath);
        return false;
    } else {
        UE_LOG(LogStoryBoard, Display, TEXT("DefaultSceneStatus loaded but not applied."));
        return true;
    }
}

FStoryNodeHelper::FStoryNodeHelper() {

}

FStoryNodeHelper::~FStoryNodeHelper() {
    StoryNodeWrappers.Empty();
    StoryNodes.Empty();
}

void FStoryNodeHelper::Initialize(UWorld* World) {
    AllocateStoryNodes(World);
    BuildGraph();
}

void FStoryNodeHelper::AllocateStoryNodes(UWorld* World) {
    TArray<AActor*> actors;
    UGameplayStatics::GetAllActorsOfClass(World, AStoryNode::StaticClass(), actors);
    for (auto actor : actors) {
        AStoryNode* node = Cast<AStoryNode>(actor);
        if (!node) {
            continue;
        }

        StoryNodes.Add(node);

        if (node->bDebugStartPoint) {
            StartPoint = node;
        }
    }
}

void FStoryNodeHelper::BuildGraph() {
    StoryNodeWrappers.Reserve(StoryNodes.Num());
    for (auto node : StoryNodes) {
        if (node == nullptr) {
            continue;
        }

        auto wrapper = StoryNodeWrappers.Find(node.Get());
        if (wrapper == nullptr) {
            wrapper = &StoryNodeWrappers.Add(node.Get(), FStoryNodeWrapper(node.Get()));
        }

        for (auto childNode : node->NextPoints) {
            if (childNode == nullptr) {
                continue;
            }

            auto childWrapper = StoryNodeWrappers.Find(childNode.Get());
            if (childWrapper == nullptr) {
                childWrapper = &StoryNodeWrappers.Add(childNode.Get(), FStoryNodeWrapper(childNode.Get()));
            }

            if (!wrapper->NextNodes.Contains(childWrapper)) {
                wrapper->NextNodes.Add(childWrapper);
            }

            if (!childWrapper->PrevNodes.Contains(wrapper)) {
                childWrapper->PrevNodes.Add(wrapper);
            }
        }
    }
}

UStoryScenario* FStoryNodeHelper::BFSNearestPrevScenario() {
    if (StartPoint) {
        return BFSNearestPrevScenario(StartPoint.Get());
    }
    return nullptr;
}

UStoryScenario* FStoryNodeHelper::BFSNearestPrevScenario(AStoryNode* Node) {
    auto wrapper = StoryNodeWrappers.Find(Node);
    if (wrapper == nullptr) {
        return nullptr;
    }

    TQueue<FStoryNodeWrapper*> queue;
    queue.Enqueue(wrapper);
    TArray<FStoryNodeWrapper*> history;

    while (!queue.IsEmpty()) {
        FStoryNodeWrapper* curr;
        queue.Peek(curr);
        queue.Pop();
        history.Add(curr);
        if (curr->Node == nullptr) {
            continue;
        }
        if (curr->Node->Scenario.Get()) {
            return curr->Node->Scenario.Get();
        }
        for (auto prev : curr->PrevNodes) {
            if (history.Contains(prev)) {
                continue;
            }
            queue.Enqueue(prev);
        }
    }
    return nullptr;
}