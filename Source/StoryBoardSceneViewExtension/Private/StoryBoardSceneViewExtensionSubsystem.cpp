#include "StoryBoardSceneViewExtensionSubsystem.h"
#include "StoryBoardSceneViewExtension.h"

#define LOCTEXT_NAMESPACE "StoryBoardSceneViewExtensionSubsystem"

void UStoryBoardSceneViewExtensionSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
    Super::Initialize(Collection);

	StoryBoardSceneViewExtension = FSceneViewExtensions::NewExtension<FStoryBoardSceneViewExtension>(this);
}

void UStoryBoardSceneViewExtensionSubsystem::Deinitialize() {
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

    Super::Deinitialize();
}

void UStoryBoardSceneViewExtensionSubsystem::OnActivate(FStoryNodeWrapperDelegate& Delegate) {
    Delegate.AddUObject(this, &UStoryBoardSceneViewExtensionSubsystem::HandleStoryNodeWrapperHint);
}

void UStoryBoardSceneViewExtensionSubsystem::OnDeactivate(FStoryNodeWrapperDelegate& Delegate) {
    Delegate.RemoveAll(this);
}

void UStoryBoardSceneViewExtensionSubsystem::HandleStoryNodeWrapperHint(FStoryNodeWrapper* Wrapper) {
    HintWrapper = Wrapper;
}

#undef LOCTEXT_NAMESPACE