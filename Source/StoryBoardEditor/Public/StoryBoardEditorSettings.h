#pragma once

#include "CoreMinimal.h"
#include "Math/MathFwd.h"
#include "Math/Color.h"

#include "StoryBoardEditorSettings.generated.h"

USTRUCT()
struct STORYBOARDEDITOR_API FDrawAttribute {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    float Size {2.0f};
    
    UPROPERTY(EditAnywhere)
    FLinearColor Color {1.0f, 0.25f, 0.33f, 1.0f};
    
    UPROPERTY(EditAnywhere)
    float LifeTime {0.0f};
    
    UPROPERTY(EditAnywhere)
    float ThickNess {1.0f};
};

UCLASS(config = StoryBoardEditorSettings, defaultconfig, meta = (DisplayName = "StoryBoardEditorSettings"))
class STORYBOARDEDITOR_API UStoryBoardEditorSettings : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY(Config, EditAnywhere, meta = (DisplayName = "Previous Edges View"))
    FDrawAttribute StoryBoardPreEdgeView;

    UPROPERTY(Config, EditAnywhere, meta = (DisplayName = "Next Edges View"))
    FDrawAttribute StoryBoardNextEdgeView;
    
    UPROPERTY(Config, EditAnywhere, meta = (DisplayName = "Hovered Node View"))
    FDrawAttribute StoryBoardHintPointView;
};

