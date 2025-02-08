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
    TObjectPtr<UStoryScenario> Scenario;

    UPROPERTY(EditAnywhere)
    bool bDebugStartPoint = false;

#if WITH_EDITOR
    DECLARE_DELEGATE_OneParam(FScenraioPropChange, AStoryNode*)
    FScenraioPropChange OnScenarioPropChanged;
    FScenraioPropChange OnNextPointsPropChanged;

    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // !WITH_EIDTOR
};