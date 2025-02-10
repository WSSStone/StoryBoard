#pragma once

#include "CoreMinimal.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "Misc/Timecode.h"

#include "StoryScenario.generated.h"

UENUM(BlueprintType)
enum class EExecuteFlag : uint8 {
    NONE = 0u UMETA(Hidden),
    GAME_TICK = 1u,
    GAME_BEGIN = (1u << 2) - 1,
    EDITOR = (1u << 3) - 1,
    MAX = (1u << 4) - 1 UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct FDataLayerStatus {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    EExecuteFlag ExecuteFlag;

    // Get current level's DataLayerInstance corresponding to this DataLayerAsset.
    UPROPERTY(EditAnywhere)
    TSoftObjectPtr<UDataLayerAsset> DataLayerAsset;

    // Runtime State
    UPROPERTY(EditAnywhere)
    EDataLayerRuntimeState RuntimeState;

#if WITH_EDITORONLY_DATA
    // Editor State
    UPROPERTY(EditAnywhere)
    bool bLoaded;

    // Editor visibility
    UPROPERTY(EditAnywhere)
    bool bEditorVisible;
#endif

    FDataLayerStatus() :
        ExecuteFlag(EExecuteFlag::GAME_BEGIN),
        DataLayerAsset(nullptr),
        RuntimeState(EDataLayerRuntimeState::Unloaded)
#if WITH_EDITORONLY_DATA
        , bLoaded(false),
        bEditorVisible(false)
#endif
    {
    }

    friend FArchive& operator<<(FArchive& Ar, FDataLayerStatus& Inp) {
        Ar << Inp.ExecuteFlag << Inp.DataLayerAsset << Inp.RuntimeState;
#if WITH_EDITORONLY_DATA
        Ar << Inp.bLoaded << Inp.bEditorVisible;
#endif
        return Ar;
    }
};

USTRUCT(BlueprintType)
struct FActorVisibility {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    EExecuteFlag ExecuteFlag;

    UPROPERTY(EditAnywhere)
    TSoftObjectPtr<AActor> Actor;

    UPROPERTY(EditAnywhere)
    bool bHiddenInGame;

    FActorVisibility() :
        ExecuteFlag(EExecuteFlag::GAME_TICK),
        Actor(nullptr),
        bHiddenInGame(false) {
    }

    friend FArchive& operator<<(FArchive& Ar, FActorVisibility& Inp) {
        Ar << Inp.ExecuteFlag << Inp.Actor << Inp.bHiddenInGame;
        return Ar;
    }
};

USTRUCT(BlueprintType)
struct FStatusCommand {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    EExecuteFlag ExecuteFlag;

    UPROPERTY(EditAnywhere)
    FString Command;

    FStatusCommand() :
        ExecuteFlag(EExecuteFlag::GAME_BEGIN),
        Command(TEXT("")) {
    }

    friend FArchive& operator<<(FArchive& Ar, FStatusCommand& Inp) {
        Ar << Inp.ExecuteFlag << Inp.Command;
        return Ar;
    }
};

USTRUCT(BlueprintType)
struct FWeatherParams {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTimecode TimeOfDay { 11, 30, 00, 00, false };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CloudCoverage { 1.4f };

    FWeatherParams() = default;
};

USTRUCT(BlueprintType)
struct FWeatherStatus {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<AActor> UDSActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<AActor> USWActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FWeatherParams WeatherParams;

    FWeatherStatus() :
        UDSActor(nullptr),
        USWActor(nullptr),
        WeatherParams() {
    }
};

DECLARE_DELEGATE_OneParam(FStoryScenarioEvent, UStoryScenario*)

UCLASS(BlueprintType, Blueprintable)
class STORYBOARDRUNTIME_API UStoryScenario : public UDataAsset {
    GENERATED_BODY()
public:
    UStoryScenario() {};

    // The name of the SceneSetting.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name;

    // Description of the SceneSetting.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Tooltip;

    // Which map this SceneSetting could apply to. 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSoftObjectPath Map;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDataLayerStatus> DataLayerStatuses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FActorVisibility> ActorVisibilities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FStatusCommand> ConsoleCommands;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FWeatherStatus WeatherStatus;

#if WITH_EDITOR
    // for edit mode
    FStoryScenarioEvent OnStoryScenarioChanged;

    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};