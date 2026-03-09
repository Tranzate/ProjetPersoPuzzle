#include "DebugMenuWidget.h"
#include "Character/PuzzleCharacter.h"
#include "Character/Debug/DebugSpectator.h"
#include "UI/InGameHUD.h"
#include "UI/InGameWidget.h"
#include "GPE/Node/Node.h"
#include "SubSystem/NodesWorldSubsystem.h"
#include "MenuLineWidget.h"
#include "UI/NotificationListWidget.h"
#include "Utility.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpectatorPawn.h"

#pragma region INITIALISATION_ET_CYCLE_DE_VIE

void UDebugMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Récupération sécurisée du joueur local
	player = Cast<APuzzleCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player)
	{
		playerCamera = player->GetPlayerCamera();
		playerMovement = player->GetCharacterMovement();
	}

	// Construction de l'arborescence des données du menu
	InitMenus();

	// Lancement du timer de mise à jour des valeurs dynamiques
	GetWorld()->GetTimerManager().SetTimer(refreshTimerHandle, this, &UDebugMenuWidget::RefreshVisibleMenu, 0.1f, true);
}

/**
 * @summary Nettoyage du timer pour éviter les accès mémoire invalides après destruction.
 */
void UDebugMenuWidget::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(refreshTimerHandle);
	Super::NativeDestruct();
}

/**
 * @summary Point central de construction du menu. Vide les anciennes données et recrée tout.
 */
void UDebugMenuWidget::InitMenus()
{
	rootMenu = NewObject<UDebugMenuItem>(this);
	rootMenu->Label = "Main Menu";
	rootMenu->Type = EMenuItemType::SubMenu;

	objectToMenuMap.Empty();

	subNode = GetWorld()->GetSubsystem<UNodesWorldSubsystem>();

	InitSystemMenu();
	InitPlayerMenu();
	InitPlayerMovementMenu();
	InitCameraMenu();
	InitWaypointMenu();
	InitNodeMenu();

	menuStack.Empty();
	menuStack.Push(rootMenu);

	LabelText->SetText(FText::FromString(rootMenu->Label));
	UpdateMenuVisuals();
	OnMenuUpdated();
}

#pragma endregion

#pragma region HELPERS_SAVE

/**
 * @summary Charge le save des waypoints depuis le disque. Retourne nullptr si inexistant.
 */
UDebugMenuSaveGame* UDebugMenuWidget::LoadWaypointSave() const
{
	return Cast<UDebugMenuSaveGame>(UGameplayStatics::LoadGameFromSlot("DebugWaypoints", 0));
}

/**
 * @summary Charge ou crée le save des waypoints.
 */
UDebugMenuSaveGame* UDebugMenuWidget::LoadOrCreateWaypointSave() const
{
	UDebugMenuSaveGame* _save = LoadWaypointSave();
	if (!_save)
		_save = Cast<UDebugMenuSaveGame>(UGameplayStatics::CreateSaveGameObject(UDebugMenuSaveGame::StaticClass()));
	return _save;
}

#pragma endregion

#pragma region SYSTEME_DE_RECHERCHE

/**
 * @summary Prépare les données de recherche et affiche la boîte de saisie.
 */
void UDebugMenuWidget::StartSearch(TArray<UObject*> _pool, TFunction<bool(UObject*, FString)> _filter,
                                   FResultMenuBuilder _customMenuBuilder)
{
	if (!textBoxClass) return;

	currentSearchPool = _pool;
	currentSearchFilter = _filter;
	currentResultBuilder = _customMenuBuilder;

	UTextBoxUserWidget* _textBox = CreateWidget<UTextBoxUserWidget>(this, textBoxClass);
	_textBox->AddToViewport();
	_textBox->OnNameValidated.AddDynamic(this, &UDebugMenuWidget::ExecuteUniversalSearch);
}

/**
 * @summary Filtre les objets et construit le menu de résultats dynamiquement.
 */
void UDebugMenuWidget::ExecuteUniversalSearch(FString _searchText)
{
	GetWorld()->GetTimerManager().PauseTimer(refreshTimerHandle);

	if (_searchText.IsEmpty() || !currentSearchFilter)
	{
		GetWorld()->GetTimerManager().UnPauseTimer(refreshTimerHandle);
		return;
	}

	UDebugMenuItem* _resultsMenu = NewObject<UDebugMenuItem>(this);
	_resultsMenu->Label = FString::Printf(TEXT("Results: %s"), *_searchText);
	_resultsMenu->Type = EMenuItemType::SubMenu;

	for (UObject* _obj : currentSearchPool)
	{
		if (IsValid(_obj) && currentSearchFilter(_obj, _searchText))
		{
			FString _name = _obj->GetName();
			if (AActor* _actor = Cast<AActor>(_obj)) _name = _actor->GetName();

			FName _objTag = FName(*FString::Printf(TEXT("RES_%p"), _obj));
			UDebugMenuItem* _objItem = AddItemToMenu(_resultsMenu->SubItems, _name, EMenuItemType::SubMenu, _objTag);

			if (currentResultBuilder && IsValid(_obj))
			{
				currentResultBuilder(_obj, _objItem);
			}
		}
	}

	auto _notifWidget = hudOwner && hudOwner->GetInGameWidget()
		? hudOwner->GetInGameWidget()->GetNotificationListWidget()
		: nullptr;

	if (_resultsMenu->SubItems.Num() > 0)
	{
		if (_notifWidget)
			_notifWidget->AddNotification(FString::Printf(TEXT("%d Results"), _resultsMenu->SubItems.Num()));

		menuStack.Push(_resultsMenu);
		currentIndex = 0;
		firstVisibleIndex = 0;
		LabelText->SetText(FText::FromString(_resultsMenu->Label));
		UpdateMenuVisuals();
		OnMenuUpdated();
	}
	else
	{
		if (_notifWidget)
			_notifWidget->AddNotification(FString::Printf(TEXT("No results found for: %s"), *_searchText));
	}

	GetWorld()->GetTimerManager().UnPauseTimer(refreshTimerHandle);
}

#pragma endregion

#pragma region INITIALISATION_SUBMENUS

void UDebugMenuWidget::InitSystemMenu()
{
	systemSubMenu = AddItemToMenu(rootMenu->SubItems, "System", EMenuItemType::SubMenu, "SYSTEM_TAG");

	infoHandlers.Add("SYS_FPS", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateUObject(this, &UDebugMenuWidget::GetFPS)));
	infoHandlers.Add("SYS_RAM", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateUObject(this, &UDebugMenuWidget::GetMemoryUsage)));
	infoHandlers.Add("SYS_NC",  FHandlerMenuDebug(FGetDebugInfoDelegate::CreateUObject(this, &UDebugMenuWidget::NoClip)));

	AddItemToMenu(systemSubMenu->SubItems, "Performance", EMenuItemType::DisplayOnly, "SYS_FPS");
	AddItemToMenu(systemSubMenu->SubItems, "Memory",      EMenuItemType::DisplayOnly, "SYS_RAM");
	AddItemToMenu(systemSubMenu->SubItems, "NoClip",      EMenuItemType::Toggle,      "SYS_NC");
}

void UDebugMenuWidget::InitPlayerMenu()
{
	playerSubMenu = AddItemToMenu(rootMenu->SubItems, "Player", EMenuItemType::SubMenu, "PLAYER_TAG");

	infoHandlers.Add("PL_LOC", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		return player ? player->GetActorLocation().ToCompactString() : FString("N/A");
	})));

	AddItemToMenu(playerSubMenu->SubItems, "Location", EMenuItemType::DisplayOnly, "PL_LOC");
}

void UDebugMenuWidget::InitPlayerMovementMenu()
{
	playerMovementSubMenu = AddItemToMenu(playerSubMenu->SubItems, "Movement", EMenuItemType::SubMenu, "PM_TAG");

	infoHandlers.Add("PM_Acc", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		return playerMovement ? FString::SanitizeFloat(playerMovement->MaxAcceleration) : FString("N/A");
	})));
	infoHandlers.Add("PM_Speed", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		return player ? FString::Printf(TEXT("%.2f cm/s"), player->GetVelocity().Size()) : FString("N/A");
	})));
	infoHandlers.Add("PM_Mode", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		if (!playerMovement) return FString("N/A");
		const UEnum* _enumPtr = StaticEnum<EMovementMode>();
		return _enumPtr ? _enumPtr->GetNameStringByValue(playerMovement->MovementMode) : FString("Error");
	})));
	infoHandlers.Add("PM_Grav", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		return playerMovement ? FString::SanitizeFloat(playerMovement->GravityScale) : FString("N/A");
	})));

	AddItemToMenu(playerMovementSubMenu->SubItems, "Acceleration",   EMenuItemType::DisplayOnly, "PM_Acc");
	AddItemToMenu(playerMovementSubMenu->SubItems, "Current Speed",  EMenuItemType::DisplayOnly, "PM_Speed");
	AddItemToMenu(playerMovementSubMenu->SubItems, "Movement Mode",  EMenuItemType::DisplayOnly, "PM_Mode");
	AddItemToMenu(playerMovementSubMenu->SubItems, "Gravity Scale",  EMenuItemType::DisplayOnly, "PM_Grav");
}

void UDebugMenuWidget::InitCameraMenu()
{
	cameraSubMenu = AddItemToMenu(playerSubMenu->SubItems, "Camera", EMenuItemType::SubMenu, "CAM_TAG");
	infoHandlers.Add("CAM_FOV", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateUObject(this, &UDebugMenuWidget::GetCameraFov)));
	AddItemToMenu(cameraSubMenu->SubItems, "Field of View", EMenuItemType::DisplayOnly, "CAM_FOV");
}

void UDebugMenuWidget::InitWaypointMenu()
{
	waypointSubMenu = AddItemToMenu(rootMenu->SubItems, "Waypoints", EMenuItemType::SubMenu, "WP_TAG");
	RefreshWaypointMenu();
}

void UDebugMenuWidget::InitNodeMenu()
{
	nodeSubMenu = AddItemToMenu(rootMenu->SubItems, "Nodes", EMenuItemType::SubMenu, "NODE_TAG");

	// Recherche de Node par ID
	infoHandlers.Add("NODE_SEARCH", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		TArray<UObject*> _pool;
		objectToMenuMap.GetKeys(_pool);

		TFunction<bool(UObject*, FString)> _nodeFilter = [](UObject* _obj, FString _text)
		{
			ANode* _node = Cast<ANode>(_obj);
			return _node && FString::FromInt(_node->GetId()).Contains(_text);
		};

		TFunction<void(UObject*, UDebugMenuItem*)> _nodeBuilder = [this](UObject* _obj, UDebugMenuItem* _resultMenuItem)
		{
			if (objectToMenuMap.Contains(_obj))
			{
				_resultMenuItem->SubItems = objectToMenuMap[_obj]->SubItems;
			}
		};

		StartSearch(_pool, _nodeFilter, _nodeBuilder);
		return FString("Searching Nodes...");
	})));

	infoHandlers.Add("NODE_HIDEDEBUGTEXT", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		if (subNode) subNode->ShowAllDebugText(false);
		return FString("Debug Text Hidden");
	})));
	infoHandlers.Add("NODE_SHOWDEBUGTEXT", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		if (subNode) subNode->ShowAllDebugText(true);
		return FString("Debug Text Shown");
	})));
	infoHandlers.Add("NODE_SHOWDEBUG", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		if (subNode) subNode->ShowAllDebug(true);
		return FString("Debug Shown");
	})));
	infoHandlers.Add("NODE_HIDEDEBUG", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		if (subNode) subNode->ShowAllDebug(false);
		return FString("Debug Hidden");
	})));

	AddItemToMenu(nodeSubMenu->SubItems, " [ Search by ID ] ",    EMenuItemType::Action, "NODE_SEARCH");
	AddItemToMenu(nodeSubMenu->SubItems, " [ Hide Debug Text ]",  EMenuItemType::Action, "NODE_HIDEDEBUGTEXT");
	AddItemToMenu(nodeSubMenu->SubItems, " [ Show Debug Text ]",  EMenuItemType::Action, "NODE_SHOWDEBUGTEXT");
	AddItemToMenu(nodeSubMenu->SubItems, " [ Show Debug ]",       EMenuItemType::Action, "NODE_SHOWDEBUG");
	AddItemToMenu(nodeSubMenu->SubItems, " [ Hide Debug ]",       EMenuItemType::Action, "NODE_HIDEDEBUG");

	if (subNode)
	{
		for (ANode* _node : subNode->GetAllNodes())
		{
			if (!_node) continue;
			FName _nodeTag = FName(*FString::Printf(TEXT("NODE_SUB_%p"), _node));
			UDebugMenuItem* _indivMenu = AddItemToMenu(nodeSubMenu->SubItems, _node->GetNameID(),
			                                           EMenuItemType::SubMenu, _nodeTag);
			objectToMenuMap.Add(_node, _indivMenu);
			BuildNodeMenu(_node, _indivMenu);
		}
	}
}

/**
 * @summary Définit les actions possibles pour un Node (TP, Info, Debug, ForceActivate...).
 *      pour éviter les crashes si l'objet est détruit avant l'appel du delegate.
 */
void UDebugMenuWidget::BuildNodeMenu(ANode* _node, UDebugMenuItem* _targetMenuItem)
{
	if (!_node || !_targetMenuItem) return;

	FString _addressKey = FString::Printf(TEXT("%p"), _node);
	FName _infoTag          = FName(*(TEXT("NODE_INFO_")         + _addressKey));
	FName _tpTag            = FName(*(TEXT("NODE_TP_")           + _addressKey));
	FName _debugTag         = FName(*(TEXT("NODE_DEBUG_")        + _addressKey));
	FName _debugNameTag     = FName(*(TEXT("NODE_DEBUGNAME_")    + _addressKey));
	FName _selectTag        = FName(*(TEXT("NODE_SELECT_")       + _addressKey));
	FName _forceActivateTag = FName(*(TEXT("NODE_FORCEACTIVATE_")+ _addressKey));
	FName _forceDeActTag    = FName(*(TEXT("NODE_FORCEDEACT_")   + _addressKey));
	
	TWeakObjectPtr<ANode> _weakNode = _node;

	// Informations
	infoHandlers.FindOrAdd(_infoTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([_weakNode]()
	{
		if (!_weakNode.IsValid()) return FString("Node destroyed");
		return FString::Printf(
			TEXT("ID: %d | Debug: %s | DebugText: %s"),
			_weakNode->GetId(),
			_weakNode->GetShowDebug() ? TEXT("ON") : TEXT("OFF"),
			_weakNode->GetVisibiltyDebugText() ? TEXT("ON") : TEXT("OFF"));
	}));

	// Téléportation
	infoHandlers.FindOrAdd(_tpTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this, _weakNode]()
	{
		if (!_weakNode.IsValid()) return FString("Node destroyed");
		if (player) player->SetActorLocation(_weakNode->GetActorLocation());
		return FString("Teleported");
	}));

	// Toggle debug visuel
	infoHandlers.FindOrAdd(_debugTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this, _weakNode]()
	{
		if (!_weakNode.IsValid()) return FString("Node destroyed");
		_weakNode->SetShowDebug(!_weakNode->GetShowDebug());
		UpdateMenuVisuals();
		return FString("");
	}));

	// Toggle nom debug
	infoHandlers.FindOrAdd(_debugNameTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this, _weakNode]()
	{
		if (!_weakNode.IsValid()) return FString("Node destroyed");
		_weakNode->ToogleShowDebugText();
		UpdateMenuVisuals();
		return FString("");
	}));

	// Sélection dans l'Outliner (Editor uniquement)
	infoHandlers.FindOrAdd(_selectTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([_weakNode]()
	{
#if WITH_EDITOR
		if (GIsEditor && _weakNode.IsValid())
		{
			GEditor->SelectNone(true, true, false);
			GEditor->SelectActor(_weakNode.Get(), true, true);
			GEditor->NoteSelectionChange();
			return FString("Selected in Outliner");
		}
#endif
		return FString("Selection only works in Editor");
	}));

	// Force activation
	infoHandlers.FindOrAdd(_forceActivateTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([_weakNode]()
	{
		if (!_weakNode.IsValid()) return FString("Node destroyed");
		_weakNode->ForceActivate();
		return FString("Forced To Activate");
	}));

	// Force désactivation
	infoHandlers.FindOrAdd(_forceDeActTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([_weakNode]()
	{
		if (!_weakNode.IsValid()) return FString("Node destroyed");
		_weakNode->ForceDeActivate();
		return FString("Forced To DeActivate");
	}));

	// Sous-menu liste des activateurs
	FName _activatorSubMenuTag = FName(*FString::Printf(TEXT("NODE_ACT_LIST_%p"), _node));
	UDebugMenuItem* _activatorListMenu = AddItemToMenu(_targetMenuItem->SubItems, ">> Activators List <<",
	                                                   EMenuItemType::SubMenu, _activatorSubMenuTag);
	BuildActivatorListMenu(_node, _activatorListMenu);

	AddItemToMenu(_targetMenuItem->SubItems, "Status",             EMenuItemType::DisplayOnly, _infoTag);
	AddItemToMenu(_targetMenuItem->SubItems, "Teleport",           EMenuItemType::Action,      _tpTag);
	AddItemToMenu(_targetMenuItem->SubItems, "Toggle Debug",       EMenuItemType::Action,      _debugTag);
	AddItemToMenu(_targetMenuItem->SubItems, "Toggle Names",       EMenuItemType::Action,      _debugNameTag);
	AddItemToMenu(_targetMenuItem->SubItems, "Select in Outliner", EMenuItemType::Action,      _selectTag);
	AddItemToMenu(_targetMenuItem->SubItems, "Force Activate",     EMenuItemType::Action,      _forceActivateTag);
	AddItemToMenu(_targetMenuItem->SubItems, "Force DeActivate",   EMenuItemType::Action,      _forceDeActTag);
}

/**
 * @summary Construit le sous-menu listant les acteurs activateurs d'un Node.
 */
void UDebugMenuWidget::BuildActivatorListMenu(ANode* _node, UDebugMenuItem* _targetMenuItem)
{
	if (!_node || !_targetMenuItem) return;

	const TArray<AActor*>& _activatorList = _node->GetAllActorToList();

	if (_activatorList.Num() == 0)
	{
		AddItemToMenu(_targetMenuItem->SubItems, "Empty List", EMenuItemType::DisplayOnly, NAME_None);
		return;
	}

	for (AActor* _actor : _activatorList)
	{
		if (!IsValid(_actor)) continue;

		FString _uniqueKey = FString::Printf(TEXT("%p_%p"), _node, _actor);
		FName _actSubTag    = FName(*(TEXT("ACT_SUB_")    + _uniqueKey));
		FName _actTpTag     = FName(*(TEXT("ACT_TP_")     + _uniqueKey));
		FName _actSelectTag = FName(*(TEXT("ACT_SELECT_") + _uniqueKey));

		FString _actorLabel = FString::Printf(TEXT("[%s]"), *_actor->GetName());
		UDebugMenuItem* _actorEntry = AddItemToMenu(_targetMenuItem->SubItems, _actorLabel,
		                                            EMenuItemType::SubMenu, _actSubTag);
		
		TWeakObjectPtr<AActor> _weakActor = _actor;

		infoHandlers.FindOrAdd(_actTpTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this, _weakActor]()
		{
			if (!_weakActor.IsValid()) return FString("Error: Actor destroyed");
			if (player)
				player->SetActorLocation(_weakActor->GetActorLocation() + FVector(0, 0, 90));
			return FString::Printf(TEXT("TP to %s"), *_weakActor->GetName());
		}));

		infoHandlers.FindOrAdd(_actSelectTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([_weakActor]()
		{
#if WITH_EDITOR
			if (GIsEditor && _weakActor.IsValid())
			{
				GEditor->SelectNone(true, true, false);
				GEditor->SelectActor(_weakActor.Get(), true, true);
				GEditor->NoteSelectionChange();
				return FString("Selected in Outliner");
			}
#endif
			return FString("Editor only");
		}));

		AddItemToMenu(_actorEntry->SubItems, "Teleport to Actor",  EMenuItemType::Action, _actTpTag);
		AddItemToMenu(_actorEntry->SubItems, "Select in Outliner", EMenuItemType::Action, _actSelectTag);
	}
}

#pragma endregion

#pragma region LOGIQUE_FONCTIONNELLE_WAYPOINTS

/**
 * @summary Vide et recrée la liste des waypoints à partir du fichier de sauvegarde.
 */
void UDebugMenuWidget::RefreshWaypointMenu()
{
	if (!waypointSubMenu) return;
	waypointSubMenu->SubItems.Empty();

	// Sauvegarder la position actuelle
	infoHandlers.FindOrAdd("SaveWP") = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		if (!textBoxClass) return FString("Class Null");
		UTextBoxUserWidget* _tb = CreateWidget<UTextBoxUserWidget>(this, textBoxClass);
		if (_tb)
		{
			_tb->AddToViewport();
			_tb->OnNameValidated.AddDynamic(this, &UDebugMenuWidget::SaveCurrentLocation);
		}
		return FString("Enter waypoint name...");
	}));
	AddItemToMenu(waypointSubMenu->SubItems, "Save New Position", EMenuItemType::Action, "SaveWP");

	// Tout supprimer
	infoHandlers.FindOrAdd("DeleteAllWP") = FHandlerMenuDebug(
		FGetDebugInfoDelegate::CreateUObject(this, &UDebugMenuWidget::DeleteAllWaypoint));
	AddItemToMenu(waypointSubMenu->SubItems, "Delete All", EMenuItemType::Action, "DeleteAllWP");
	
	UDebugMenuSaveGame* _save = LoadWaypointSave();
	if (_save)
	{
		for (int _i = 0; _i < _save->savedWaypoints.Num(); _i++)
		{
			const FWaypointData& _wp = _save->savedWaypoints[_i];
			FName _baseTag = FName(*_wp.name);

			FName _tpTag  = FName(*(_baseTag.ToString() + "_0"));
			FName _delTag = FName(*(_baseTag.ToString() + "_1"));

			infoHandlers.FindOrAdd(_tpTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this, _wp]()
			{
				if (player)
				{
					player->SetActorLocation(_wp.location);
					if (AController* _ctrl = player->GetController())
						_ctrl->SetControlRotation(_wp.rotation);
				}
				return FString("Teleport to ") + _wp.name;
			}));

			infoHandlers.FindOrAdd(_delTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this, _i, _wp]()
			{
				DeleteWaypoint(_i);
				return FString("Delete ") + _wp.name;
			}));

			AddItemToMenu(waypointSubMenu->SubItems, _wp.name, EMenuItemType::Action, _baseTag, {"TP", "Suppr"});
		}
	}

	if (menuStack.Num() > 0 && menuStack.Last() == waypointSubMenu)
	{
		UpdateMenuVisuals();
	}
	OnMenuUpdated();
}

/**
 * @summary Supprime un waypoint par index.
 */
void UDebugMenuWidget::DeleteWaypoint(int _index)
{
	UDebugMenuSaveGame* _save = LoadWaypointSave();
	if (_save && _save->savedWaypoints.IsValidIndex(_index))
	{
		_save->savedWaypoints.RemoveAt(_index);
		UGameplayStatics::SaveGameToSlot(_save, "DebugWaypoints", 0);

		int32 _newTotal = waypointSubMenu->SubItems.Num();
		if (currentIndex >= _newTotal && _newTotal > 0)
			currentIndex = _newTotal - 1;

		// RefreshWaypointMenu fait sa propre lecture — pas de double chargement ici
		RefreshWaypointMenu();
	}
}

void UDebugMenuWidget::SaveCurrentLocation(FString _name)
{
	UDebugMenuSaveGame* _save = LoadOrCreateWaypointSave();
	if (!_save) return;

	FWaypointData _newWP;
	_newWP.name = _name;
	if (player)
	{
		_newWP.location = player->GetActorLocation();
		if (AController* _ctrl = player->GetController())
			_newWP.rotation = _ctrl->GetControlRotation();
	}

	_save->savedWaypoints.Add(_newWP);
	UGameplayStatics::SaveGameToSlot(_save, "DebugWaypoints", 0);
	RefreshWaypointMenu();
}

FString UDebugMenuWidget::DeleteAllWaypoint()
{
	UGameplayStatics::DeleteGameInSlot("DebugWaypoints", 0);
	RefreshWaypointMenu();
	return "Waypoints Cleared";
}

#pragma endregion

#pragma region METHODES_SYSTEME_UTILITAIRES

FString UDebugMenuWidget::GetFPS()
{
	return FString::Printf(TEXT("%.0f FPS"), 1.0f / FApp::GetDeltaTime());
}

FString UDebugMenuWidget::GetMemoryUsage()
{
	FPlatformMemoryStats _stats = FPlatformMemory::GetStats();
	return FString::Printf(TEXT("%.0f MB"), (float)_stats.UsedPhysical / (1024.f * 1024.f));
}

/**
 * @summary Bascule entre le joueur et un spectateur sans collision (NoClip).
 */
FString UDebugMenuWidget::NoClip()
{
	APlayerController* _pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!_pc || !player) return "Error";

	ADebugSpectator* _currentSpectator = Cast<ADebugSpectator>(_pc->GetPawn());

	if (!_currentSpectator) // ACTIVER NOCLIP
	{
		FActorSpawnParameters _spawnParams;
		_spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ASpectatorPawn* _newSpectator = GetWorld()->SpawnActor<ADebugSpectator>(
			spectatorClassBP, player->GetActorLocation(), _pc->GetControlRotation(), _spawnParams);

		if (_newSpectator)
		{
			_newSpectator->SetActorEnableCollision(false);
			player->RemoveMapping();
			_pc->Possess(_newSpectator);
			_newSpectator->EnableInput(_pc);

			if (UDebugMenuItem* _item = rootMenu->GetItemByName("Waypoints"))
				_item->CanBeActive = false;
			return "NoClip ON";
		}
	}
	else // DESACTIVER NOCLIP
	{
		player->SetActorLocation(_currentSpectator->GetActorLocation());
		_currentSpectator->RemoveMapping();
		_pc->Possess(player);
		player->AddMapping();
		_currentSpectator->Destroy();

		if (UDebugMenuItem* _item = rootMenu->GetItemByName("Waypoints"))
			_item->CanBeActive = true;
		return "NoClip OFF";
	}
	return "Failed";
}

FString UDebugMenuWidget::GetCameraFov()
{
	return playerCamera ? FString::SanitizeFloat(playerCamera->FieldOfView) : "N/A";
}

#pragma endregion

#pragma region NAVIGATION_CORE

void UDebugMenuWidget::SelectItem()
{
	if (menuStack.Num() == 0) return;
	UDebugMenuItem* _current = menuStack.Last();
	if (!_current->SubItems.IsValidIndex(currentIndex)) return;

	UDebugMenuItem* _selected = _current->SubItems[currentIndex];
	if (!_selected->CanBeActive) return;

	if (_selected->Type == EMenuItemType::SubMenu)
	{
		menuStack.Push(_selected);
		currentIndex = 0;
		firstVisibleIndex = 0;
		LabelText->SetText(FText::FromString(_selected->Label));
		UpdateMenuVisuals();
	}
	else
	{
		FName _actionKey = (_selected->ActionLabels.Num() > 0)
			? FName(*(_selected->ActionTag.ToString() + "_" + FString::FromInt(_selected->CurrentActionIndex)))
			: _selected->ActionTag;

		if (infoHandlers.Contains(_actionKey))
		{
			FString _newValue = infoHandlers[_actionKey].delegate.Execute();
			if (!_newValue.IsEmpty() && hudOwner)
			{
				hudOwner->GetInGameWidget()->GetNotificationListWidget()->AddNotification(_newValue);
			}
		}
	}
	OnMenuUpdated();
}

void UDebugMenuWidget::ResetDynamicValue(UDebugMenuItem* _item)
{
	if (_item)
	{
		_item->DynamicValue = "";
		UpdateMenuVisuals();
	}
}

void UDebugMenuWidget::GoBack()
{
	if (menuStack.Num() > 1)
	{
		menuStack.Pop();
		currentIndex = 0;
		firstVisibleIndex = 0;
		LabelText->SetText(FText::FromString(menuStack.Last()->Label));
		UpdateMenuVisuals();
		OnMenuUpdated();
	}
}

void UDebugMenuWidget::MoveSelection(int32 _dir)
{
	if (menuStack.Num() == 0) return;

	UDebugMenuItem* _currentMenu = menuStack.Last();
	if (!IsValid(_currentMenu))
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveSelection: Le menu actuel dans la stack est invalide !"));
		return;
	}

	int _num = _currentMenu->SubItems.Num();
	if (_num > 0)
	{
		currentIndex = (currentIndex + _dir + _num) % _num;
	}

	UpdateMenuVisuals();
}

void UDebugMenuWidget::MoveActionSelection(int _direction)
{
	if (menuStack.Num() == 0) return;
	UDebugMenuItem* _currentMenu = menuStack.Last();
	if (!_currentMenu->SubItems.IsValidIndex(currentIndex)) return;

	UDebugMenuItem* _selected = _currentMenu->SubItems[currentIndex];
	if (_selected && _selected->ActionLabels.Num() > 1)
	{
		int32 _count = _selected->ActionLabels.Num();
		_selected->CurrentActionIndex = (_selected->CurrentActionIndex + _direction + _count) % _count;
		UpdateMenuVisuals();
	}
}

#pragma endregion

#pragma region RENDU_VISUEL

/**
 * @summary Recycle les widgets de ligne et les affiche selon le scroll.
 */
void UDebugMenuWidget::UpdateMenuVisuals()
{
	if (!MenuConainer_show || !MenuConainer_Hidden) return;

	// Remettre les lignes actives dans le pool en les réinitialisant proprement
	TArray<UWidget*> _children = MenuConainer_show->GetAllChildren();
	for (UWidget* _widget : _children)
	{
		if (UMenuLineWidget* _line = Cast<UMenuLineWidget>(_widget))
			_line->ResetWidget();
		MenuConainer_Hidden->AddChild(_widget);
	}
	MenuConainer_show->ClearChildren();

	TArray<UDebugMenuItem*> _visible = GetVisibleMenuItems();
	const int _count = _visible.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		UMenuLineWidget* _line = (MenuConainer_Hidden->GetChildrenCount() > 0)
			? Cast<UMenuLineWidget>(MenuConainer_Hidden->GetChildAt(0))
			: CreateWidget<UMenuLineWidget>(this, menuLineClass);

		if (_line)
		{
			MenuConainer_show->AddChildToVerticalBox(_line);
			_line->SetValues(_visible[_i], (firstVisibleIndex + _i == currentIndex));
		}
	}
}

TArray<UDebugMenuItem*> UDebugMenuWidget::GetVisibleMenuItems()
{
	TArray<UDebugMenuItem*> _result;
	if (menuStack.Num() == 0) return _result;

	TArray<UDebugMenuItem*>& _all = menuStack.Last()->SubItems;

	if (currentIndex >= firstVisibleIndex + maxVisibleItems)
		firstVisibleIndex = currentIndex - maxVisibleItems + 1;
	else if (currentIndex < firstVisibleIndex)
		firstVisibleIndex = currentIndex;

	firstVisibleIndex = FMath::Clamp(firstVisibleIndex, 0, FMath::Max(0, _all.Num() - maxVisibleItems));

	for (int _i = 0; _i < maxVisibleItems && _all.IsValidIndex(firstVisibleIndex + _i); _i++)
	{
		_result.Add(_all[firstVisibleIndex + _i]);
	}
	return _result;
}

/**
 * @summary Mis à jour par timer pour actualiser les textes dynamiques (DisplayOnly).
 */
void UDebugMenuWidget::RefreshVisibleMenu()
{
	if (menuStack.Num() == 0) return;

	TArray<UDebugMenuItem*>& _currentItems = menuStack.Last()->SubItems;
	
	bool _hasDisplayOnly = _currentItems.ContainsByPredicate(
		[](UDebugMenuItem* _item) { return _item && _item->Type == EMenuItemType::DisplayOnly; });

	if (!_hasDisplayOnly) return;

	bool _needsVisualUpdate = false;
	for (UDebugMenuItem* _item : _currentItems)
	{
		if (_item->Type == EMenuItemType::DisplayOnly && infoHandlers.Contains(_item->ActionTag))
		{
			FString _newValue = infoHandlers[_item->ActionTag].delegate.Execute();
			if (_newValue != _item->DynamicValue)
			{
				_item->DynamicValue = _newValue;
				_needsVisualUpdate = true;
			}
		}
	}

	if (_needsVisualUpdate) UpdateMenuVisuals();
}

UDebugMenuItem* UDebugMenuWidget::AddItemToMenu(TArray<UDebugMenuItem*>& _target, FString _label,
                                                EMenuItemType _type, FName _tag,
                                                TArray<FString> _actions, bool _active)
{
	UDebugMenuItem* _item = NewObject<UDebugMenuItem>(this);
	_item->Label = _label;
	_item->Type = _type;
	_item->ActionTag = _tag;
	_item->ActionLabels = _actions;
	_item->CanBeActive = _active;
	_target.Add(_item);
	return _item;
}

#pragma endregion
