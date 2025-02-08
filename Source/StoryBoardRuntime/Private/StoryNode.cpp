#include "StoryNode.h"

#if WITH_EDITOR
void AStoryNode::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
    const FName PropertyName(PropertyChangedEvent.Property->GetFName());

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AStoryNode, NextPoints)) {
        for (auto& NextPoint : NextPoints) {
            if (NextPoint && !NextPoint->PrevPoints.Contains(this)) {
                NextPoint->PrevPoints.Add(this);
            }
        }
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AStoryNode, Scenario)) {
        ScenarioPropChangeEvent.ExecuteIfBound(this);
    }

    Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif