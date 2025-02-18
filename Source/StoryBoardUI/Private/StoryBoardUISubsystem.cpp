#include "StoryBoardUISubsystem.h"

#define LOCTEXT_NAMESPACE "StoryBoardSceneViewExtensionSubsystem"

void UStoryBoardUISubsystem::Initialize(FSubsystemCollectionBase& Collection) {
    Super::Initialize(Collection);

	StoryBoardSceneViewExtension = FSceneViewExtensions::NewExtension<FStoryBoardSceneViewExtension>(this);
}

void UStoryBoardUISubsystem::Deinitialize() {
	DisableSVE();

    Super::Deinitialize();
}

void UStoryBoardUISubsystem::DisableSVE() {
	{
		StoryBoardSceneViewExtension->IsActiveThisFrameFunctions.Empty();

		FSceneViewExtensionIsActiveFunctor IsActiveFunctor;

		IsActiveFunctor.IsActiveFunction = [](const ISceneViewExtension* SceneViewExtension, const FSceneViewExtensionContext& Context) {
			return TOptional<bool>(false);
			};

		StoryBoardSceneViewExtension->IsActiveThisFrameFunctions.Add(IsActiveFunctor);
	}

	// release scene view extension in render thread
	ENQUEUE_RENDER_COMMAND(ReleaseSVE)([this](FRHICommandListImmediate& RHICmdList) {
		// Prevent this SVE from being gathered, in case it is kept alive by a strong reference somewhere else.
		{
			StoryBoardSceneViewExtension->IsActiveThisFrameFunctions.Empty();

			FSceneViewExtensionIsActiveFunctor IsActiveFunctor;

			IsActiveFunctor.IsActiveFunction = [](const ISceneViewExtension* SceneViewExtension, const FSceneViewExtensionContext& Context) {
				return TOptional<bool>(false);
				};

			StoryBoardSceneViewExtension->IsActiveThisFrameFunctions.Add(IsActiveFunctor);
		}

		StoryBoardSceneViewExtension->Invalidate();
		StoryBoardSceneViewExtension.Reset();
		StoryBoardSceneViewExtension = nullptr;
		});

	FlushRenderingCommands();
}

void UStoryBoardUISubsystem::BindIndicatorDelegate(FStoryNodeWrapperDelegate& Delegate) {
    Delegate.AddUObject(this, &UStoryBoardUISubsystem::HandleStoryNodeWrapperHint);
}

void UStoryBoardUISubsystem::UnbindIndicatorDelegate(FStoryNodeWrapperDelegate& Delegate) {
    Delegate.RemoveAll(this);
}

void UStoryBoardUISubsystem::HandleStoryNodeWrapperHint(FStoryNodeWrapper* Wrapper) {
    HintWrapper = Wrapper;
}

#undef LOCTEXT_NAMESPACE