#include "StoryBoardSceneViewExtension.h"
#include "StoryBoardSceneViewExtensionSubsystem.h"

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

FStoryBoardSceneViewExtension::FStoryBoardSceneViewExtension(
    const FAutoRegister& AutoRegister,
    UStoryBoardSceneViewExtensionSubsystem* InSubsystem)
    : FSceneViewExtensionBase(AutoRegister),
	WorldSubsystem(InSubsystem) {

}

void FStoryBoardSceneViewExtension::PrePostProcessPass_RenderThread(
    FRDGBuilder& GraphBuilder,
    const FSceneView& View,
    const FPostProcessingInputs& Inputs) {
	if (!IsValid(WorldSubsystem)) {
		return;
	}

    // draw sth else

    // draw hint node
    // draw thumbnail image from WorldSubsystem->HintWrapper->Node->Scenario
    

}

void FStoryBoardSceneViewExtension::Invalidate() {
    WorldSubsystem = nullptr;
}