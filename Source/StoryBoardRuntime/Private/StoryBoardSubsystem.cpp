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

    FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UStoryBoardSubsystem::HandleInitializedActors);

    // fall back to default scenario
    GConfig->GetString(
        TEXT("/Script/CodeFlee.StoryBoardSubsystem"),
        *FString::Printf(TEXT("%s"), *world->GetMapName()),
        DefaultScenarioPath,
        GGameIni
    );

    TryLoadDefaultScenario(world->GetMapName());
    if (DefaultScenario) {
        SetupScene(DefaultScenario);
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
        SetupScene(scenario);
        return;
    }
}

void UStoryBoardSubsystem::SetupScene(UStoryScenario* Scenario) {
    if (Scenario == nullptr) {
        UE_LOG(LogStoryBoard, Warning, TEXT("Input Scenario is nullptr."));
        return;
    }

    CurrentScenario = Scenario;

    ExecuteCommands(Scenario->ConsoleCommands);
    SetupActorVisibilities(Scenario->ActorVisibilities);
    SetupWeather(Scenario->WeatherStatus);
    SetupDataLayerStatus(Scenario->DataLayerStatuses);
}

void UStoryBoardSubsystem::SetDefaultScenario(UStoryScenario* in) {
    DefaultScenario = in;
    DefaultScenarioPath = DefaultScenario->GetPathName();
    GConfig->SetString(
        TEXT("/Script/CodeFlee.StoryBoardSubsystem"),
        *FString::Printf(TEXT("%s"), *GetWorld()->GetMapName()),
        *DefaultScenarioPath,
        GGameIni
    );
}

void UStoryBoardSubsystem::ExecuteCommands(const TArray<FStatusCommand>& ConsoleCommands) {
    UWorld* World = GetWorld();
    if (World == nullptr) {
        UE_LOG(LogStoryBoard, Warning, TEXT("Invalid World context while executing command"));
        return;
    }

    for (const FStatusCommand& cmd : ConsoleCommands) {
        if (cmd.Command.IsEmpty()) {
            continue;
        }

        GEngine->Exec(World, *cmd.Command);
    }
}

void UStoryBoardSubsystem::SetupActorVisibilities(const TArray<FActorVisibility>& ActorVisibilities) {
    for (const FActorVisibility& ActorVisibility : ActorVisibilities) {
        if (AActor* actor = ActorVisibility.Actor.Get()) {
            actor->SetActorHiddenInGame(ActorVisibility.bHiddenInGame);
            UE_LOG(LogStoryBoard, Log, TEXT("[%s] from %d to %d"), *actor->GetName(), ActorVisibility.bHiddenInGame, actor->IsHidden());
        }
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

void UStoryBoardSubsystem::SetupDataLayerStatus(const TArray<FDataLayerStatus>& DataLayerStatuses) {
    UDataLayerManager* dataLayerManager = GetWorld()->GetDataLayerManager();
    if (dataLayerManager == nullptr) {
        UE_LOG(LogStoryBoard, Warning, TEXT("Current world does not have a DataLayerManager!"));
        return;
    }

    for (const FDataLayerStatus& dataLayerStatus : DataLayerStatuses) {
        const UDataLayerInstance* inst = dataLayerManager->GetDataLayerInstanceFromAsset(dataLayerStatus.DataLayerAsset.Get());
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
    if (StartPoint.IsValid()) {
        return BFSNearestPrevScenario(StartPoint.Get());
    }
    return nullptr;
}

UStoryScenario* FStoryNodeHelper::BFSNearestPrevScenario(AStoryNode* Node) {
    TArray<FStoryNodeWrapper*> queue {&StoryNodeWrappers[Node]};
    TArray<FStoryNodeWrapper*> history;

    while (!queue.IsEmpty()) {
        FStoryNodeWrapper* curr = queue[0];
        queue.RemoveAt(0);
        history.Add(curr);
        if (curr->Node->Scenario.Get()) {
            return curr->Node->Scenario.Get();
        }
        for (auto prev : curr->PrevNodes) {
            if (history.Contains(prev)) {
                continue;
            }
            queue.Add(prev);
        }
    }
    return nullptr;
}