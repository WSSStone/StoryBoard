#include "StoryBoardSceneViewExtension.h"
#include "StoryBoardUISubsystem.h"
#include "StoryBoardSubsystem.h"

#include "Containers/DynamicRHIResourceArray.h"
#include "DynamicResolutionState.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FXRenderingUtils.h"
#include "PostProcess/PostProcessInputs.h"
#include "RHI.h"
#include "SceneRenderTargetParameters.h"
#include "SceneView.h"
#include "ScreenPass.h"

namespace {
    struct FIndicatorContext {
        FVector         CameraWorldLocation;
        FVector         TargetWorldLocation;
        FIntVector2     SceneTextureSize;
        FVector2D       IndicatorSize;
        FVector2D       TargetOffsetRange;
        FVector2D       IndicatorOffsetRange;
        UTexture2D*     Texture;
    } IndicatorContext;

    struct FDrawTexture2DInfo {
        FMatrix2x2      Rotation;
        FVector2D       Offset;
        FVector2D       Scale;
    } Indicaotr2DInfo, Arrow2DInfo;

    void ConfigureDrawContext(const FSceneView& View, const FPostProcessingInputs& Inputs, FStoryNodeWrapper* Target) {
        if (!Target) {
            return;
        }
        IndicatorContext.CameraWorldLocation = View.ViewMatrices.GetViewOrigin();
        IndicatorContext.TargetWorldLocation = Target->Node->GetActorLocation();
        const FIntRect primaryViewRect = UE::FXRenderingUtils::GetRawViewRectUnsafe(View);
        IndicatorContext.SceneTextureSize = FIntVector2(primaryViewRect.Width(), primaryViewRect.Height());
        IndicatorContext.IndicatorSize = FVector2D(0.1f, 0.1f);
        IndicatorContext.TargetOffsetRange = FVector2D(0.55f, 0.55f);
        IndicatorContext.IndicatorOffsetRange = FVector2D(0.5f, 0.5f);
    }

    void DrawIndicator(FRDGBuilder& GraphBuilder,const FSceneView& View, const FPostProcessingInputs& Inputs) {
        
    }
}

FStoryBoardSceneViewExtension::FStoryBoardSceneViewExtension(
    const FAutoRegister& AutoRegister,
    UStoryBoardUISubsystem* InSubsystem)
    : FSceneViewExtensionBase(AutoRegister),
	WorldSubsystem(InSubsystem) {}

void FStoryBoardSceneViewExtension::PrePostProcessPass_RenderThread(
    FRDGBuilder& GraphBuilder,
    const FSceneView& View,
    const FPostProcessingInputs& Inputs) {
	if (!IsValid(WorldSubsystem)) {
		return;
	}

    ConfigureDrawContext(View, Inputs, WorldSubsystem->HintWrapper);

    // draw hint node
    // draw thumbnail image from WorldSubsystem->HintWrapper->Node->Scenario
}

void FStoryBoardSceneViewExtension::Invalidate() {
    WorldSubsystem = nullptr;
}