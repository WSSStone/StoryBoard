#include "StoryBoardSubsystem.h"

#include "Misc/OutputDeviceNull.h"
#include "EngineUtils.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogStoryBoard, Log, All);

void UStoryBoardSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
    UWorld* world = GetWorld();

    GConfig->GetString(
        TEXT("/Script/CodeFlee.StoryBoardSubsystem"),
        *FString::Printf(TEXT("%s"), *world->GetMapName()),
        DefaultScenarioPath,
        GGameIni
    );

    TryLoadDefaultScenario(world->GetMapName());
}

void UStoryBoardSubsystem::Deinitialize() {
    OnWeatherStatusChanged.Clear();
    DefaultScenario = nullptr;
    CurrentScenario = nullptr;
    UE_LOG(LogStoryBoard, Display, TEXT("Deinitialize."));
}

void UStoryBoardSubsystem::SetupScene(UStoryScenario* SceneStatus) {
    if (SceneStatus == nullptr) {
        UE_LOG(LogStoryBoard, Warning, TEXT("Input SceneSetting is nullptr."));
        return;
    }

    CurrentScenario = SceneStatus;

    ExecuteCommands(SceneStatus->ConsoleCommands);
    SetupActorVisibilities(SceneStatus->ActorVisibilities);
    SetupWeather(SceneStatus->WeatherStatus);
    SetupDataLayerStatus(SceneStatus->DataLayerStatuses);
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
