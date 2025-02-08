#include "StoryNode.h"

#if WITH_EDITOR
void AStoryNode::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
    const FName PropertyName(PropertyChangedEvent.Property->GetFName());

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AStoryNode, NextPoints)) {
        OnNextPointsPropChanged.ExecuteIfBound(this);
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AStoryNode, Scenario)) {
        OnScenarioPropChanged.ExecuteIfBound(this);
    }

    Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif