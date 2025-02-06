#pragma once

#include "StoryScenario.h"

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"

#include "StoryNode.generated.h"

UCLASS()
class STORYBOARDRUNTIME_API AStoryNode : public ATriggerBox {
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TObjectPtr<AStoryNode>> NextPoints;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UStoryScenario> Scenario;

#if WITH_EDITOR
    TArray<TObjectPtr<AStoryNode>> PrevPoints;

    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // !WITH_EIDTOR
};