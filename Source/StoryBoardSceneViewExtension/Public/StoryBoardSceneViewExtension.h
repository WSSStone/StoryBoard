#pragma once

#include "StoryBoardSceneViewExtensionSubsystem.h"

#include "CoreMinimal.h"
#include "SceneViewExtension.h"

class UStoryBoardSceneViewExtensionSubsystem;

class FStoryBoardSceneViewExtension : public FSceneViewExtensionBase
{
public:
    FStoryBoardSceneViewExtension(const FAutoRegister& AutoRegister, UStoryBoardSceneViewExtensionSubsystem* InSubsystem);
	
#pragma region FSceneViewExtensionBase Interface
    virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {};
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {};
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {};
    virtual void PreRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& InViewFamily) override {};
    virtual void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView) override {};
	virtual void PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs) override;
#pragma endregion
    void Invalidate();
private:
    UStoryBoardSceneViewExtensionSubsystem* WorldSubsystem;
};