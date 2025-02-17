#pragma warning(disable:4834)

#include "StoryBoardEditorSubsystem.h"
#include "StoryBoardEdMode.h"
#include "StoryBoardCommands.h"

#include "StoryBoardUISubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EditorModeManager.h"
#include "EditorModes.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "EditorDirectories.h"
#include "FileHelpers.h"
#include "UObject/SavePackage.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "DataLayer/DataLayerEditorSubsystem.h"
#include "Subsystems/ActorEditorContextSubsystem.h"
#include "LevelEditor.h"
#include "LevelEditorViewport.h"
#include "Selection.h"

#define LOCTEXT_NAMESPACE "StoryBoardEditorSubsystem"

DEFINE_LOG_CATEGORY_STATIC(LogStoryBoardEditor, Log, All);

FNodeSelectedEvent UStoryBoardEditorSubsystem::EdSetCurrentNodeEvent;
FSetHintNodeEvent UStoryBoardEditorSubsystem::EdSetHindNodeEvent;

void UStoryBoardEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
    Super::Initialize(Collection);

    FStoryBoardCommands::Register();

    GEditor->RegisterForUndo(this);
    FEditorDelegates::BeginPIE.AddUObject(this, &UStoryBoardEditorSubsystem::HandleEditorBeginPIE);
    FEditorDelegates::OnMapOpened.AddUObject(this, &UStoryBoardEditorSubsystem::HandleOnMapOpened);
    FWorldDelegates::OnCurrentLevelChanged.AddUObject(this, &UStoryBoardEditorSubsystem::OnCurrentLevelChanged);
    
    // register ui extensions here
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.OnLevelEditorCreated().AddUObject(this, &UStoryBoardEditorSubsystem::OnLevelEditorCreatedEvent);

    CreateStoryAssetHelper();
}

void UStoryBoardEditorSubsystem::Deinitialize() {
    // ExitEdMode();

    RemoveStoryAssetHelper();

    FWorldDelegates::OnCurrentLevelChanged.RemoveAll(this);
    FEditorDelegates::ChangeEditorMode.RemoveAll(this);
    FEditorDelegates::OnMapOpened.RemoveAll(this);
    FEditorDelegates::BeginPIE.RemoveAll(this);
    GEditor->UnregisterForUndo(this);

    FStoryBoardCommands::Unregister();

    Super::Deinitialize();
}

UWorld* UStoryBoardEditorSubsystem::GetWorld() const {
    return GEditor->GetEditorWorldContext().World();
}

UStoryBoardSubsystem* UStoryBoardEditorSubsystem::GetStoryBoardSubsystem() {
    if (StoryBoardPtr == nullptr) {
        UWorld* world = GEditor->GetEditorWorldContext().World();
        check(world);
        StoryBoardPtr = world->GetSubsystem<UStoryBoardSubsystem>();
        check(StoryBoardPtr);
    }
    return StoryBoardPtr;
}

void UStoryBoardEditorSubsystem::CreateStoryAssetHelper() {
    RemoveStoryAssetHelper();
    StoryAssetHelper = MakeUnique<FStoryAssetHelper>();
}

void UStoryBoardEditorSubsystem::RemoveStoryAssetHelper() {
    if (StoryAssetHelper.IsValid()) {
        StoryAssetHelper.Reset(nullptr);
    }
}

void UStoryBoardEditorSubsystem::CreateStoryNodeHelper() {
    RemoveStoryNodeHelper();
    StoryNodeHelper = MakeUnique<FStoryNodeEditorHelper>(GEditor->GetEditorWorldContext().World());
    StoryNodeHelper->BuildGraph();
}

void UStoryBoardEditorSubsystem::RemoveStoryNodeHelper() {
    if (StoryNodeHelper.IsValid()) {
        StoryNodeHelper.Reset(nullptr);
    }
}

void UStoryBoardEditorSubsystem::CreateStoryBoardViewportDrawer() {
    RemoveStoryBoardViewportDrawer();

    auto drawSettings = GetMutableDefault<UStoryBoardEditorSettings>();
    StoryBoardViewportDrawer = MakeUnique<FStoryBoardViewportDrawer>(
        this,
        drawSettings->StoryBoardPreEdgeView,
        drawSettings->StoryBoardNextEdgeView,
        drawSettings->StoryBoardHintPointView);
}

void UStoryBoardEditorSubsystem::RemoveStoryBoardViewportDrawer() {
    if (StoryBoardViewportDrawer.IsValid()) {
        StoryBoardViewportDrawer.Reset(nullptr);
    }
}

void UStoryBoardEditorSubsystem::SetHintNode(AStoryNode* Node) {
    StoryBoardViewportDrawer->HintNode = Node;
    
    FStoryNodeWrapper* wrapper = StoryNodeHelper->StoryNodeWrappers.Find(Node);
    EdSetHindNodeEvent.Broadcast(wrapper);
}

void UStoryBoardEditorSubsystem::RemoveHintNode() {
    StoryBoardViewportDrawer->HintNode = nullptr;

    EdSetHindNodeEvent.Broadcast(nullptr);
}


void UStoryBoardEditorSubsystem::HandleEditorBeginPIE(bool) {
    ExitEdMode();
}

void UStoryBoardEditorSubsystem::HandleOnMapOpened(const FString& Filename, bool bAsTemplate) {
    FString shortName = FPackageName::GetShortName(Filename);
    
    GetStoryBoardSubsystem();
}

void UStoryBoardEditorSubsystem::OnCurrentLevelChanged(ULevel* InNewLevel, ULevel* InOldLevel, UWorld* InWorld) {
    GetStoryBoardSubsystem();
}

void UStoryBoardEditorSubsystem::SetupDefaultScenario() {
    StoryBoardPtr->SetupScene(StoryBoardPtr->DefaultScenario, EExecuteFlag::GAME_BEGIN);
}

void UStoryBoardEditorSubsystem::OnScenarioPropChange(UStoryScenario* Scenario) {
    if (CurrentScenario != nullptr && Scenario == CurrentScenario.Get()) {
        SetupScenario(Scenario);
    }
}

void UStoryBoardEditorSubsystem::HandleNodeScenarioChange(AStoryNode* Node) {
    FStoryNodeWrapper* wrapper = StoryNodeHelper->StoryNodeWrappers.Find(Node);
    if (EdSetCurrentNodeEvent.IsBound()) {
        EdSetCurrentNodeEvent.Broadcast(wrapper);
    }

    SetupScenario(Node->Scenario.Get());
}

void UStoryBoardEditorSubsystem::HandleNodeNextPointsChange(AStoryNode* Node) {
    StoryNodeHelper->ReallocateStoryNodes(GEditor->GetEditorWorldContext().World());
    
    FStoryNodeWrapper* wrapper = StoryNodeHelper->StoryNodeWrappers.Find(Node);
    if (EdSetCurrentNodeEvent.IsBound()) {
        EdSetCurrentNodeEvent.Broadcast(wrapper);
    }
}

void UStoryBoardEditorSubsystem::OnEnterEdMode() {
    isEdMode = true;
    CreateStoryNodeHelper();
    CreateStoryBoardViewportDrawer();

    // assign a default StoryNode actor
    TArray<UObject*> selectedActors;
    GEditor->GetSelectedActors()->GetSelectedObjects(AStoryNode::StaticClass(), selectedActors);
    if (!selectedActors.IsEmpty()) {
        StoryNodeHelper->SelectedNode = Cast<AStoryNode>(selectedActors[0]);
        return;
    }

    AActor* actor = UGameplayStatics::GetActorOfClass(GEditor->GetEditorWorldContext().World(), AStoryNode::StaticClass());
    StoryNodeHelper->SelectedNode = Cast<AStoryNode>(actor);

    auto world = GEditor->GetEditorWorldContext().World();
    auto sveSubsys = world->GetSubsystem<UStoryBoardUISubsystem>();
    sveSubsys->BindIndicatorDelegate(EdSetHindNodeEvent);
}

void UStoryBoardEditorSubsystem::OnExitEdMode() {
    auto world = GEditor->GetEditorWorldContext().World();
    auto sveSubsys = world->GetSubsystem<UStoryBoardUISubsystem>();
    sveSubsys->UnbindIndicatorDelegate(EdSetHindNodeEvent);

    RemoveStoryBoardViewportDrawer();
    RemoveStoryNodeHelper();
    isEdMode = false;
}

void UStoryBoardEditorSubsystem::EnterEdMode() {
    GLevelEditorModeTools().ActivateMode(UStoryBoardEdMode::EM_StoryBoardEdModeId);
}

void UStoryBoardEditorSubsystem::ExitEdMode() {
    GLevelEditorModeTools().DeactivateMode(UStoryBoardEdMode::EM_StoryBoardEdModeId);
}

auto FocusActor = [](AActor* Actor) {
    FBox bounds = Actor->GetComponentsBoundingBox(true);

    FLevelEditorViewportClient* viewport = static_cast<FLevelEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
    if (viewport) {
        viewport->FocusViewportOnBox(bounds);
        // viewport->SetViewportType(ELevelViewportType::LVT_Perspective);
        viewport->Invalidate();
    }
};

auto SelectActor = [](AActor* Actor) {
    GEditor->SelectNone(false, true);
    GEditor->SelectActor(Actor, true, false, true, true);
    // in this case, refresh Details panel
    GEditor->NoteSelectionChange();
    };

FReply UStoryBoardEditorSubsystem::FirstNode() {
    SetCurrentNode(StoryNodeHelper->BFSFurthestWrapper(nullptr, true)->Node, false);

    SelectActor(StoryNodeHelper->SelectedNode.Get());
    FocusActor(StoryNodeHelper->SelectedNode.Get());

    return FReply::Handled();
}

FReply UStoryBoardEditorSubsystem::PreviousNode() {
    // Point Current to Previous
    TArray<TObjectPtr<AStoryNode>> arr;
    StoryNodeHelper->GetPrevStoryNodes(arr);
    if (StoryNodeHelper->SelectedNode != nullptr && !arr.IsEmpty()) {
        SetCurrentNode(arr[0].Get(), false);

        SelectActor(StoryNodeHelper->SelectedNode.Get());
    }

    return FReply::Handled();
}

FReply UStoryBoardEditorSubsystem::CurrentNode() {
    auto node = StoryNodeHelper->SelectedNode.Get();

    FocusActor(node);
    SetCurrentScenario(node->Scenario.Get());

    return FReply::Handled();
}

FReply UStoryBoardEditorSubsystem::NextNode() {
    // Point Current to Next
    TArray<TObjectPtr<AStoryNode>> arr; 
    StoryNodeHelper->GetNextStoryNodes(arr);
    if (StoryNodeHelper->SelectedNode != nullptr && !arr.IsEmpty()) {
        SetCurrentNode(arr[0].Get(), false);

        SelectActor(StoryNodeHelper->SelectedNode.Get());
    }

    return FReply::Handled();
}

FReply UStoryBoardEditorSubsystem::LastNode() {
    SetCurrentNode(StoryNodeHelper->BFSFurthestWrapper(nullptr, false)->Node, false);

    SelectActor(StoryNodeHelper->SelectedNode.Get());
    FocusActor(StoryNodeHelper->SelectedNode.Get());

    return FReply::Handled();
}

FReply UStoryBoardEditorSubsystem::UISelectNode(AStoryNode* Node, bool bApplyScenario) {
    SetCurrentNode(Node, bApplyScenario);

    SelectActor(StoryNodeHelper->SelectedNode.Get());

    return FReply::Handled();
}

void UStoryBoardEditorSubsystem::SetCurrentNode(AStoryNode* Node, bool bApplyScenario) {
    if (Node == nullptr) {
        return;
    }

    StoryNodeHelper->SelectedNode = Node;
    if (Node && bApplyScenario) {
        SetCurrentScenario(Node->Scenario.Get());
    }

    if (EdSetCurrentNodeEvent.IsBound()) {
        FStoryNodeWrapper* wrapper = StoryNodeHelper->StoryNodeWrappers.Find(Node);
        EdSetCurrentNodeEvent.Broadcast(wrapper);
    }
}

void UStoryBoardEditorSubsystem::SetCurrentScenario(UStoryScenario* Scenario) {
    if (CurrentScenario != nullptr && Scenario == CurrentScenario.Get()) {
        return;
    }

    if (CurrentScenario != nullptr && CurrentScenario->OnStoryScenarioChanged.IsBound()) {
        CurrentScenario->OnStoryScenarioChanged.Unbind();
    }

    CurrentScenario = Scenario;

    if (CurrentScenario != nullptr) {
        SetupScenario(CurrentScenario.Get());
        CurrentScenario->OnStoryScenarioChanged.BindUObject(this, &UStoryBoardEditorSubsystem::OnScenarioPropChange);
    }
}

void UStoryBoardEditorSubsystem::SetupScenario(UStoryScenario* Scenario) {
    if (GEditor->PlayWorld) {
        return;
    }

    if (Scenario == nullptr) {
        UE_LOG(LogStoryBoardEditor, Warning, TEXT("Input SceneSetting is nullptr."));
        return;
    }

    GetStoryBoardSubsystem()->CurrentScenario = Scenario;

    ExecuteCommands(Scenario->ConsoleCommands);

    SetupWeather(Scenario->WeatherStatus);

    SetupDataLayerStatus(Scenario->DataLayerStatuses);
}

UStoryScenario* UStoryBoardEditorSubsystem::GetDefaultScenario() {
    return GetStoryBoardSubsystem()->DefaultScenario;
}

UStoryScenario* UStoryBoardEditorSubsystem::GetCurrentScenario() {
    return GetStoryBoardSubsystem()->CurrentScenario;
}

void UStoryBoardEditorSubsystem::SetDefaultScenario(UStoryScenario* in) {
    GetStoryBoardSubsystem()->SetDefaultScenario(in);
}

void UStoryBoardEditorSubsystem::ExecuteCommands(const TArray<FStatusCommand>& ConsoleCommands) {
    UWorld* world = GEditor->GetEditorWorldContext().World();
    if (world == nullptr) {
        UE_LOG(LogStoryBoardEditor, Warning, TEXT("Invalid World context while executing command"));
        return;
    }    

    for (const FStatusCommand& cmd : ConsoleCommands) {
        if (cmd.Command.IsEmpty()) {
            continue;
        }

        // call execute command
        GEditor->Exec(world, *cmd.Command);
    }
}

void UStoryBoardEditorSubsystem::SetupDataLayerStatus(const TArray<FDataLayerStatus>& DataLayerStatuses) {
    UDataLayerEditorSubsystem* dataLayerEditorSubsystem = GEditor ? GEditor->GetEditorSubsystem<UDataLayerEditorSubsystem>() : nullptr;

    if (dataLayerEditorSubsystem == nullptr) {
        UE_LOG(LogStoryBoardEditor, Display, TEXT("Cannot get UDataLayerEditorSubsystem singleton."));
        return;
    }

    for (const FDataLayerStatus& dataLayerStatus : DataLayerStatuses) {
        UDataLayerInstance* inst = dataLayerEditorSubsystem->GetDataLayerInstance(dataLayerStatus.DataLayerAsset.Get());
        if (inst == nullptr) {
            continue;
        }

        dataLayerEditorSubsystem->SetDataLayerVisibility(inst, dataLayerStatus.bEditorVisible);
        dataLayerEditorSubsystem->SetDataLayerIsLoadedInEditor(inst, dataLayerStatus.bLoaded, true);
    }
}

void UStoryBoardEditorSubsystem::SetupWeather(const FWeatherStatus& WeatherStatus) {
    // time of day
    if (AActor* uds = WeatherStatus.UDSActor.Get()) {
#if 0
        // This function did be invoked but the blueprint setting mark it 'not call in editor'
        FName funcName = FName("Set Time with Time Code");
        if (UFunction* func = uds->FindFunction(funcName)) {
            const FTimecode* inp = &WeatherStatus.WeatherParams.TimeOfDay;

            int size = sizeof(FTimecode); // just for breakpoint

            UE_LOG(LogStoryBoardEditor, Display, TEXT("Call UDS function %s(%d:%d:%d:%d, %d)! Input Timecode size %d"),
                *funcName.ToString(),
                WeatherStatus.WeatherParams.TimeOfDay.Hours,
                WeatherStatus.WeatherParams.TimeOfDay.Minutes,
                WeatherStatus.WeatherParams.TimeOfDay.Seconds,
                WeatherStatus.WeatherParams.TimeOfDay.Frames,
                WeatherStatus.WeatherParams.TimeOfDay.bDropFrameFormat,
                sizeof(FTimecode));

            uds->ProcessEvent(func, (void*)(inp));
        } else {
            UE_LOG(LogStoryBoardEditor, Warning, TEXT("UDS does not has function named %s"), *funcName.ToString());
        }
#else
        // instead, set the Time Of Day float property
        FProperty* property = uds->GetClass()->FindPropertyByName(TEXT("Time Of Day"));
        if (property == nullptr) {
            UE_LOG(LogStoryBoardEditor, Warning, TEXT("UDS does not have property named 'Time Of Day'!"));
            return;
        }

        auto TimecodeToFloat = [](FTimecode timecode) -> double {
            double ret = 0.0;
            ret += timecode.Hours * 100.0;
            double coef = 100.0 / 60.0;
            ret += timecode.Minutes * coef;
            int milli = FMath::TruncToInt(timecode.Frames * 16.66);
            ret += FMath::GetMappedRangeValueClamped(FVector2D(0.0, 60.0), FVector2D(0.0, coef), (timecode.Seconds + milli / 1000.0));
            return ret;
        };

        uds->PreEditChange(property);
        double value = TimecodeToFloat(WeatherStatus.WeatherParams.TimeOfDay);
        property->SetValue_InContainer(uds, (const void*)(&value));
        FPropertyChangedEvent propertyChangedEvent(property);
        propertyChangedEvent.ChangeType = EPropertyChangeType::ValueSet;
        uds->PostEditChangeProperty(propertyChangedEvent);
#endif
    }

    // cloud density
    if (AActor* udw = WeatherStatus.USWActor.Get()) {
        FProperty* property = udw->GetClass()->FindPropertyByName(TEXT("Cloud Coverage"));
        if (property == nullptr) {
            UE_LOG(LogStoryBoardEditor, Warning, TEXT("UDW does not have property named 'Cloud Coverage'!"));
            return;
        }

        udw->PreEditChange(property);
        double value = WeatherStatus.WeatherParams.CloudCoverage;
        property->SetValue_InContainer(udw, (const void*)(&value));
        FPropertyChangedEvent propertyChangedEvent(property);
        propertyChangedEvent.ChangeType = EPropertyChangeType::ValueSet;
        udw->PostEditChangeProperty(propertyChangedEvent);
    }
}

#pragma region Editor GUI
void UStoryBoardEditorSubsystem::RegisterEntry() {
    FToolMenuOwnerScoped OwnerScoped(this);

    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");

    FToolMenuSection& Section = Menu->AddSection("ACT", LOCTEXT("ACT", "Action"));

    Section.AddEntry(FToolMenuEntry::InitMenuEntry(
        "Scenarios",
        INVTEXT("Scenario Window"),
        INVTEXT("Open ACT Scenario Window."),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "DeveloperTools.MenuIcon"),
        FUIAction(FExecuteAction::CreateLambda([]() {
            GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>()->SummonScenarioEditor();
        }))
    ));
}

void UStoryBoardEditorSubsystem::OnLevelEditorCreatedEvent(TSharedPtr<ILevelEditor> Editor) {
    RegisterEntry();
}

TSharedRef<SDockTab> UStoryBoardEditorSubsystem::SummonScenarioEditor() {
    // this interface and object filter is gud
    // but the window should be concerned like a graph editor

    TArray<UStoryScenario*> matchingScenarios;
    UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
    if (currentWorld == nullptr) {
        UE_LOG(LogStoryBoardEditor, Warning, TEXT("Invalid World context."));
        return SNew(SDockTab);
    }

    for (TObjectIterator<UStoryScenario> It; It; ++It) {
        UStoryScenario* scenario = *It;
        if (scenario == nullptr) continue;

        FString currentLevelShortName = currentWorld->GetMapName();
        
        FSoftObjectPath softPath = scenario->Map;
        UObject* resolved = softPath.ResolveObject();
        FString mapPath = softPath.GetAssetName();

        if (mapPath.Equals(currentLevelShortName)) {
            matchingScenarios.Add(scenario);
            UE_LOG(LogStoryBoardEditor, Display, TEXT("Find Scenarios [%s] matching current map [%s]"),
                *scenario->Name.ToString(), *currentLevelShortName);
        }
    }

    FMenuBuilder MenuBuilder(true, nullptr);
    for (auto status : matchingScenarios) {
        MenuBuilder.AddMenuEntry(
            status->Name,
            status->Tooltip,
            FSlateIcon(),
            FUIAction(
                FExecuteAction::CreateLambda([status, this]() {
                    this->SetupScenario(status);})
            ),
            NAME_None,
            EUserInterfaceActionType::Button);
    }

    FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bShowObjectLabel = true;
    DetailsViewArgs.bAllowSearch = true;
    DetailsViewArgs.bAllowFavoriteSystem = true;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::ENameAreaSettings::ObjectsUseNameArea;
    DetailsViewArgs.ViewIdentifier = FName("BlueprintDefaults");
    auto DetailView = EditModule.CreateDetailView(DetailsViewArgs);
    DetailView->SetObject(matchingScenarios.IsEmpty() ? nullptr : matchingScenarios[0]);

    TSharedRef<SDockTab> ScenarioEditor = SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        .Label(FText::FromString("Scenario Editor"))
        .Content()
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                MenuBuilder.MakeWidget()
            ]
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                DetailView
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SButton)
                .Text(FText::FromString("yo"))
                .OnClicked_Lambda([]() -> FReply {
                    UE_LOG(LogStoryBoardEditor, Display, TEXT("yo"));
                    return FReply::Handled(); }
                )
            ]
        ];

    return ScenarioEditor;
}
#pragma endregion

FStoryNodeEditorHelper::FStoryNodeEditorHelper(UWorld* World) {
    AllocateStoryNodes(World);
    BuildGraph();

    FEditorDelegates::OnNewActorsDropped.AddLambda([this](const TArray<UObject*>& uobjects, const TArray<AActor*>& actors) {
        if (!actors.IsEmpty()) ReallocateStoryNodes(actors[0]->GetWorld());
        });
    FEditorDelegates::OnDuplicateActorsEnd.AddRaw(this, &FStoryNodeEditorHelper::OnStoryNodeAddedOrRemoved);
    FEditorDelegates::OnEditPasteActorsEnd.AddRaw(this, &FStoryNodeEditorHelper::OnStoryNodeAddedOrRemoved);
    FEditorDelegates::OnDeleteActorsEnd.AddRaw(this, &FStoryNodeEditorHelper::OnStoryNodeAddedOrRemoved);
}

FStoryNodeEditorHelper::~FStoryNodeEditorHelper() {
    FEditorDelegates::OnDeleteActorsEnd.RemoveAll(this);
    FEditorDelegates::OnEditPasteActorsEnd.RemoveAll(this);
    FEditorDelegates::OnDuplicateActorsEnd.RemoveAll(this);
    FEditorDelegates::OnNewActorsDropped.RemoveAll(this);

    for (auto node : StoryNodes) {
        if (node) {
            if (node->OnScenarioPropChanged.IsBound())
                node->OnScenarioPropChanged.Unbind();

            if (node->OnNextPointsPropChanged.IsBound())
                node->OnNextPointsPropChanged.Unbind();
        }
    }

    StoryNodeWrappers.Empty();
    StoryNodes.Empty();
}

void FStoryNodeEditorHelper::AllocateStoryNodes(UWorld* World) {
    TArray<AActor*> actors;
    UGameplayStatics::GetAllActorsOfClass(World, AStoryNode::StaticClass(), actors);
    for (auto actor : actors) {
        AStoryNode* node = Cast<AStoryNode>(actor);
        if (!node) {
            continue;
        }

        StoryNodes.Add(node);

        auto subSys = GEditor->GetEditorSubsystem<UStoryBoardEditorSubsystem>();
        node->OnScenarioPropChanged.BindUObject(subSys, &UStoryBoardEditorSubsystem::HandleNodeScenarioChange);
        node->OnNextPointsPropChanged.BindUObject(subSys, &UStoryBoardEditorSubsystem::HandleNodeNextPointsChange);
    }
}

void FStoryNodeEditorHelper::ReallocateStoryNodes(UWorld* World) {
    for (auto node : StoryNodes) {
        if (node) {
            if (node->OnScenarioPropChanged.IsBound())
                node->OnScenarioPropChanged.Unbind();

            if (node->OnNextPointsPropChanged.IsBound())
                node->OnNextPointsPropChanged.Unbind();
        }
    }
    StoryNodes.Empty();
    StoryNodeWrappers.Empty();

    AllocateStoryNodes(World);
    BuildGraph();
}

void FStoryNodeEditorHelper::OnStoryNodeAddedOrRemoved() {
    UWorld* World = GEditor->GetEditorWorldContext().World();
    ReallocateStoryNodes(World);
}

UStoryScenario* FStoryNodeEditorHelper::BFSNearestPrevScenario() {
    if (SelectedNode != nullptr) {
        return FStoryNodeHelper::BFSNearestPrevScenario(SelectedNode.Get());
    }
    return nullptr;
}

FStoryNodeWrapper* FStoryNodeEditorHelper::BFSFurthestWrapper(FStoryNodeWrapper* Wrapper, bool bPrev) {
    if (Wrapper == nullptr) {
        Wrapper = StoryNodeWrappers.Find(SelectedNode.Get());
    }

    struct FDepthNode {
        FStoryNodeWrapper* m_Wrapper {nullptr};
        int m_Depth {0};
    };

    auto SelArray = [](FStoryNodeWrapper* wrapper, bool bPrev) -> TArray<FStoryNodeWrapper*>& {
        return bPrev ? wrapper->PrevNodes : wrapper->NextNodes;
    };
    
    TQueue<FDepthNode> queue;
    queue.Enqueue({Wrapper, 0});
    int maxDepth = 0;
    TSet<FStoryNodeWrapper*> visited;
    TArray<FDepthNode> lastLayer;

    while (!queue.IsEmpty()) {
        FDepthNode depthNode;
        queue.Peek(depthNode);
        queue.Pop();

        FStoryNodeWrapper* curr = depthNode.m_Wrapper;
        int depth = depthNode.m_Depth;

        visited.Add(curr);

        if (depth > maxDepth) {
            maxDepth = depth;
            lastLayer.Empty();
        }

        lastLayer.Add(depthNode);

        TArray<FStoryNodeWrapper*>& array = SelArray(curr, bPrev);
        for (auto wrapper : array) {
            if (visited.Contains(wrapper)) {
                continue;
            }
            queue.Enqueue({wrapper, depth + 1});
        }
    }

    lastLayer.Sort([&SelArray, &bPrev](const FDepthNode& a, const FDepthNode& b) {
        return SelArray(a.m_Wrapper, bPrev).Num() < SelArray(b.m_Wrapper, bPrev).Num();
    });

    return lastLayer[0].m_Wrapper;
}

void FStoryNodeEditorHelper::GetPrevStoryNodes(TArray<TObjectPtr<AStoryNode>>& Ret) {
    GetImmeidateNodes(Ret, SelectedNode.Get());
}

void FStoryNodeEditorHelper::GetNextStoryNodes(TArray<TObjectPtr<AStoryNode>>& Ret) {
    GetImmeidateNodes(Ret, SelectedNode.Get(), false);
}

void FStoryNodeEditorHelper::GetImmeidateNodes(TArray<TObjectPtr<AStoryNode>>& Ret, AStoryNode* Node, bool bParent) {    
    FStoryNodeWrapper* wrapper = StoryNodeWrappers.Find(Node);
    if (wrapper != nullptr) {
        const TArray<FStoryNodeWrapper*>& arr = bParent ? wrapper->PrevNodes : wrapper->NextNodes;
        for (auto imme : arr) {
            Ret.Add(imme->Node.Get());
        }
    }
}

FString FStoryAssetHelper::CreateScenario(UStoryScenario* TemplateScenario) {
    FString assetPath, packagepath;
    RaiseSaveAssetWindow(assetPath);

    packagepath = FPackageName::ObjectPathToPackageName(assetPath);
    const FName assetName(FPackageName::GetLongPackageAssetName(packagepath));

    if (assetPath.IsEmpty() || packagepath.IsEmpty()) {
        return assetPath;
    }

    UPackage* const package = CreatePackage(*packagepath);
    UStoryScenario* scenario;

    if (TemplateScenario) {
        scenario = DuplicateObject(TemplateScenario, package, assetName);
    } else {
        scenario = NewObject<UStoryScenario>(package, UStoryScenario::StaticClass(), *assetName.ToString(), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);

        FSavePackageArgs saveArgs;
        saveArgs.TopLevelFlags = RF_Standalone;
        saveArgs.SaveFlags = SAVE_NoError;
        bool result = UPackage::SavePackage(package, scenario, *assetPath, saveArgs);
        UE_LOG(LogStoryBoardEditor, Display, TEXT("%s"), result ? *FString("save succeeded") : *FString("save failed"));
    }

    scenario->MarkPackageDirty();
    FAssetRegistryModule::AssetCreated(scenario);

    return assetPath;
}

void FStoryAssetHelper::RaiseSaveAssetWindow(FString& AssetPath) {
    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

    FSaveAssetDialogConfig dialogConfig;
    dialogConfig.DefaultPath = FEditorDirectories::Get().GetLastDirectory(ELastDirectory::NEW_ASSET);
    dialogConfig.DefaultAssetName = TEXT("NewStoryScenario");
    dialogConfig.AssetClassNames.Add(UStoryScenario::StaticClass()->GetClassPathName());
    dialogConfig.DialogTitleOverride = LOCTEXT("CreateStoryScenario", "Create New Story Scenario");
    dialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::Disallow;

    AssetPath = contentBrowserModule.Get().CreateModalSaveAssetDialog(dialogConfig);
}

FStoryBoardViewportDrawer::FStoryBoardViewportDrawer(UStoryBoardEditorSubsystem* Owner) :
    Owner(Owner) {
    World = Owner->GetWorld();
}

FStoryBoardViewportDrawer::FStoryBoardViewportDrawer(UStoryBoardEditorSubsystem* Owner,
    const FDrawAttribute& PrevAttrib,
    const FDrawAttribute& NextAttrib,
    const FDrawAttribute& HintAttrib) :
    Owner(Owner),
    PrevAttrib(PrevAttrib),
    NextAttrib(NextAttrib),
    HintAttrib(HintAttrib) {
    World = Owner->GetWorld();
}

FStoryBoardViewportDrawer::~FStoryBoardViewportDrawer() {

}

void FStoryBoardViewportDrawer::Tick(float DeltaTime) {
    if (!Owner->IsEdMode()) {
        return;
    }

    DrawEdges();
    DrawHint();
}

bool FStoryBoardViewportDrawer::IsTickable() const {
    UWorld* world = GEditor->GetEditorWorldContext().World();
    return World != nullptr;
}

TStatId FStoryBoardViewportDrawer::GetStatId() const {
    return TStatId();
}

void FStoryBoardViewportDrawer::DrawEdges() {
    if (Owner->StoryNodeHelper->SelectedNode == nullptr) {
        return;
    }

    auto cur = Owner->StoryNodeHelper->StoryNodeWrappers.Find(Owner->StoryNodeHelper->SelectedNode.Get());
    if (cur == nullptr || cur->Node == nullptr) {
        return;
    }

    for (auto prev : cur->PrevNodes) {
        if (prev->Node == nullptr) {
            continue;
        }

        UKismetSystemLibrary::DrawDebugArrow(World,
            prev->Node->GetActorLocation(),
            cur->Node->GetActorLocation(),
            PrevAttrib.Size,
            PrevAttrib.Color,
            PrevAttrib.LifeTime,
            PrevAttrib.ThickNess);
    }

    for (auto next : cur->NextNodes) {
        if (next->Node == nullptr) {
            continue;
        }

        UKismetSystemLibrary::DrawDebugArrow(World,
            cur->Node->GetActorLocation(),
            next->Node->GetActorLocation(),
            NextAttrib.Size,
            NextAttrib.Color,
            NextAttrib.LifeTime,
            NextAttrib.ThickNess);
    }
}

void FStoryBoardViewportDrawer::DrawHint() {
    if (HintNode == nullptr) {
        return;
    }

    UKismetSystemLibrary::DrawDebugPoint(World,
        HintNode->GetActorLocation(),
        HintAttrib.Size,
        HintAttrib.Color,
        HintAttrib.LifeTime);
}

#undef LOCTEXT_NAMESPACE