#pragma warning(disable:4834)

#include "StoryBoardEditorSubsystem.h"
#include "StoryBoardEdMode.h"

#include "Kismet/GameplayStatics.h"
#include "EditorModeManager.h"
#include "EditorModes.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "DataLayer/DataLayerEditorSubsystem.h"
#include "Subsystems/ActorEditorContextSubsystem.h"
#include "LevelEditor.h"
#include "LevelEditorViewport.h"
#include "Selection.h"

#define LOCTEXT_NAMESPACE "StoryBoardEditorSubsystem"

DEFINE_LOG_CATEGORY_STATIC(LogStoryBoardEditor, Log, All);

FNodeSelectedEvent UStoryBoardEditorSubsystem::EdNodeSelectedEvent;

void UStoryBoardEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
    Super::Initialize(Collection);

    UE_LOG(LogStoryBoardEditor, Display, TEXT("UStoryBoardEditorSubsystem Initializing."));

    FEditorDelegates::OnMapOpened.AddUObject(this, &UStoryBoardEditorSubsystem::HandleOnMapOpened);
    FWorldDelegates::OnCurrentLevelChanged.AddUObject(this, &UStoryBoardEditorSubsystem::OnCurrentLevelChanged);
    
    // register ui extensions here
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.OnLevelEditorCreated().AddUObject(this, &UStoryBoardEditorSubsystem::OnLevelEditorCreatedEvent);
}

void UStoryBoardEditorSubsystem::Deinitialize() {
    UE_LOG(LogStoryBoardEditor, Display, TEXT("UStoryBoardEditorSubsystem Deinitializing."));

    if (StoryNodeHelper.IsValid()) {
        StoryNodeHelper.Release();
    }

    FWorldDelegates::OnCurrentLevelChanged.RemoveAll(this);
    FEditorDelegates::ChangeEditorMode.RemoveAll(this);
    FEditorDelegates::OnMapOpened.RemoveAll(this);

    Super::Deinitialize();
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

void UStoryBoardEditorSubsystem::CreateStoryNodeHelper() {
    if (StoryNodeHelper.IsValid()) {
        StoryNodeHelper.Release();
    }

    StoryNodeHelper = MakeUnique<FStoryNodeHelper>(GEditor->GetEditorWorldContext().World());
}

void UStoryBoardEditorSubsystem::HandleOnMapOpened(const FString& Filename, bool bAsTemplate) {
    FString shortName = FPackageName::GetShortName(Filename);
    
    GetStoryBoardSubsystem();

    CreateStoryNodeHelper();

    UE_LOG(LogStoryBoardEditor, Display, TEXT("HandleOnMapOpened, ShortName: %s, UStoryBoardSubsystem: %p"),
        *shortName, StoryBoardPtr);
}

void UStoryBoardEditorSubsystem::OnCurrentLevelChanged(ULevel* InNewLevel, ULevel* InOldLevel, UWorld* InWorld) {
    GetStoryBoardSubsystem();
    UE_LOG(LogStoryBoardEditor, Display, TEXT("OnCurrentLevelChanged: UStoryBoardSubsystem: %p"), StoryBoardPtr);
}

void UStoryBoardEditorSubsystem::SetupDefaultScenario() {
    StoryBoardPtr->SetupScene(StoryBoardPtr->DefaultScenario);
}

void UStoryBoardEditorSubsystem::OnScenarioChange(UStoryScenario* inp) {
    if (CurrentScenario.IsValid() && inp == CurrentScenario.Get()) {
        SetupScenario(inp);
    }
}

void UStoryBoardEditorSubsystem::OnEnterEdMode() {
    CreateStoryNodeHelper();

    // assign a default StoryNode actor

    TArray<UObject*> selectedActors;
    GEditor->GetSelectedActors()->GetSelectedObjects(AStoryNode::StaticClass(), selectedActors);
    if (!selectedActors.IsEmpty()) {
        StoryNodeHelper->SelectedNode = Cast<AStoryNode>(selectedActors[0]);
        return;
    }

    AActor* actor = UGameplayStatics::GetActorOfClass(GEditor->GetEditorWorldContext().World(), AStoryNode::StaticClass());
    StoryNodeHelper->SelectedNode = Cast<AStoryNode>(actor);
}

void UStoryBoardEditorSubsystem::OnExitEdMode() {
    if (StoryNodeHelper.IsValid()) {
        StoryNodeHelper.Release();
    }
}

void UStoryBoardEditorSubsystem::EnterEdMode() {
    GLevelEditorModeTools().ActivateMode(UStoryBoardEdMode::EM_StoryBoardEdModeId);
}

void UStoryBoardEditorSubsystem::ExitEdMode() {
    GLevelEditorModeTools().DeactivateMode(UStoryBoardEdMode::EM_StoryBoardEdModeId);
}

auto FocusActor = [](AActor* actor) {
    FBox bounds = actor->GetComponentsBoundingBox(true);

    FLevelEditorViewportClient* viewport = static_cast<FLevelEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
    if (viewport) {
        viewport->FocusViewportOnBox(bounds);
        viewport->SetViewportType(ELevelViewportType::LVT_Perspective);
        viewport->Invalidate();
    }

    GEditor->SelectNone(false, true);
    GEditor->SelectActor(actor, true, false, true, true);
};

FReply UStoryBoardEditorSubsystem::PreviousNode() {
    UE_LOG(LogStoryBoardEditor, Warning, TEXT("Select Previous Scenario"));

    // Point Current to Previous
    if (StoryNodeHelper->SelectedNode.IsValid() && !StoryNodeHelper->SelectedNode->PrevPoints.IsEmpty()) {
        SetCurrentNode(StoryNodeHelper->SelectedNode->PrevPoints[0].Get());
        FocusActor(StoryNodeHelper->SelectedNode.Get());
    }

    return FReply::Handled();
}

FReply UStoryBoardEditorSubsystem::NextNode() {
    UE_LOG(LogStoryBoardEditor, Warning, TEXT("Select Next Scenario"));

    // Point Current to Next
    if (StoryNodeHelper->SelectedNode.IsValid() && !StoryNodeHelper->SelectedNode->NextPoints.IsEmpty()) {
        SetCurrentNode(StoryNodeHelper->SelectedNode->NextPoints[0].Get());
        FocusActor(StoryNodeHelper->SelectedNode.Get());
    }

    return FReply::Handled();
}

FReply UStoryBoardEditorSubsystem::UISelectNode(AStoryNode* Node) {
    UE_LOG(LogStoryBoardEditor, Warning, TEXT("Select Node"));

    SetCurrentNode(Node);
    FocusActor(Node);

    return FReply::Handled();
}

void UStoryBoardEditorSubsystem::SetCurrentNode(AStoryNode* Node) {
    if (StoryNodeHelper->SelectedNode.IsValid() && Node == StoryNodeHelper->SelectedNode.Get()) {
        return;
    }

    StoryNodeHelper->SelectedNode = Node;

    if (StoryNodeHelper->SelectedNode.IsValid() && EdNodeSelectedEvent.IsBound()) {
        SetCurrentScenario(StoryNodeHelper->SelectedNode->Scenario.Get());
        EdNodeSelectedEvent.Broadcast(Node);
    }
}

void UStoryBoardEditorSubsystem::SetCurrentScenario(UStoryScenario* Scenario) {
    if (CurrentScenario.IsValid() && Scenario == CurrentScenario.Get()) {
        return;
    }

    if (CurrentScenario.IsValid() && CurrentScenario->OnStoryScenarioChanged.IsBound()) {
        CurrentScenario->OnStoryScenarioChanged.Unbind();
    }

    CurrentScenario = Scenario;

    if (CurrentScenario.IsValid()) {
        SetupScenario(CurrentScenario.Get());
        CurrentScenario->OnStoryScenarioChanged.BindUObject(this, &UStoryBoardEditorSubsystem::OnScenarioChange);
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

FStoryNodeHelper::FStoryNodeHelper(UWorld* World) {
    AllocateStoryNodes(World);

    FEditorDelegates::OnNewActorsPlaced.AddLambda([this](UObject* uobject, const TArray<AActor*>& actors) {
            if (!actors.IsEmpty()) AllocateStoryNodes(actors[0]->GetWorld());
        });
    FEditorDelegates::OnDuplicateActorsEnd.AddRaw(this, &FStoryNodeHelper::OnStoryNodeAddedOrRemoved);
    FEditorDelegates::OnEditPasteActorsEnd.AddRaw(this, &FStoryNodeHelper::OnStoryNodeAddedOrRemoved);
    FEditorDelegates::OnDeleteActorsEnd.AddRaw(this, &FStoryNodeHelper::OnStoryNodeAddedOrRemoved);
}

FStoryNodeHelper::~FStoryNodeHelper() {
    FEditorDelegates::OnDeleteActorsEnd.RemoveAll(this);
    FEditorDelegates::OnEditPasteActorsEnd.RemoveAll(this);
    FEditorDelegates::OnDuplicateActorsEnd.RemoveAll(this);
    FEditorDelegates::OnNewActorsPlaced.RemoveAll(this);

    StoryNodes.Empty();
}

void FStoryNodeHelper::OnStoryNodeAddedOrRemoved() {
    UWorld* World = GEditor->GetEditorWorldContext().World();
    AllocateStoryNodes(World);
}

void FStoryNodeHelper::AllocateStoryNodes(UWorld* World) {
    TArray<AActor*> actors;
    UGameplayStatics::GetAllActorsOfClass(World, AStoryNode::StaticClass(), actors);
    for (auto actor : actors) {
        AStoryNode* node = Cast<AStoryNode>(actor);
        if (!node) {
            continue;
        }

        StoryNodes.Add(node);
        
        // set up edMode-only property PrevPoints
        for (auto child : node->NextPoints) {
            if (!child->PrevPoints.Contains(node)) {
                child->PrevPoints.Add(node);
            }
        }
    }
}

#pragma region Editor GUI
void UStoryBoardEditorSubsystem::RegisterEntry() {
    FToolMenuOwnerScoped OwnerScoped(this);

    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");

    FToolMenuSection& Section = Menu->AddSection("ACT", LOCTEXT("ACT", "World Action"));

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

#undef LOCTEXT_NAMESPACE