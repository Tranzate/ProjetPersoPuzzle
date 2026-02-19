#include "DebugMenuWidget.h"
#include "PuzzleCharacter.h"
#include "DebugSpectator.h"
#include "InGameHUD.h"
#include "InGameWidget.h"
#include "Node.h"
#include "NodesWorldSubsystem.h"
#include "MenuLineWidget.h"
#include "NotificationListWidget.h"
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
	// Lancement du timer de mise à jour
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
	// Racine du menu
	rootMenu = NewObject<UDebugMenuItem>(this);
	rootMenu->Label = "Main Menu";
	rootMenu->Type = EMenuItemType::SubMenu;

	// Reset du dictionnaire de correspondance Objets/Menus
	objectToMenuMap.Empty();

	_subNode = GetWorld()->GetSubsystem<UNodesWorldSubsystem>();
	// --- CONSTRUCTION DES BRANCHES ---
	InitSystemMenu();
	InitPlayerMenu();
	InitPlayerMovementMenu();
	InitCameraMenu();
	InitWaypointMenu();
	InitNodeMenu();

	// Initialisation de la navigation
	menuStack.Empty();
	menuStack.Push(rootMenu);

	LabelText->SetText(FText::FromString(rootMenu->Label));
	UpdateMenuVisuals();
	OnMenuUpdated();
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

	// Création de la TextBox
	UTextBoxUserWidget* _textBox = CreateWidget<UTextBoxUserWidget>(this, textBoxClass);
	_textBox->AddToViewport();

	// On lie la validation du texte à la fonction d'exécution
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

			// Création de l'item résultat
			FName _objTag = FName(*FString::Printf(TEXT("RES_%p"), _obj));
			UDebugMenuItem* _objItem = AddItemToMenu(_resultsMenu->SubItems, _name, EMenuItemType::SubMenu, _objTag);

			// On utilise le builder
			if (currentResultBuilder && IsValid(_obj))
			{
				currentResultBuilder(_obj, _objItem);
			}
		}
	}

	// Affichage des résultats si trouvés
	if (_resultsMenu->SubItems.Num() > 0)
	{
		FString _errorMsg = FString::Printf(TEXT("%d Results"), _resultsMenu->SubItems.Num());
		if (hudOwner && hudOwner->GetInGameWidget())
		{
			hudOwner->GetInGameWidget()->GetNotificationListWidget()->AddNotification(_errorMsg);
		}
		menuStack.Push(_resultsMenu);
		currentIndex = 0;
		firstVisibleIndex = 0;
		LabelText->SetText(FText::FromString(_resultsMenu->Label));
		UpdateMenuVisuals();
		OnMenuUpdated();
	}
	else
	{
		FString _errorMsg = FString::Printf(TEXT("No results found for : %s"), *_searchText);
		if (hudOwner && hudOwner->GetInGameWidget())
		{
			hudOwner->GetInGameWidget()->GetNotificationListWidget()->AddNotification(_errorMsg);
		}
	}

	GetWorld()->GetTimerManager().UnPauseTimer(refreshTimerHandle);
}

#pragma endregion

#pragma region INITIALISATION_SUBMENUS

void UDebugMenuWidget::InitSystemMenu()
{
	systemSubMenu = AddItemToMenu(rootMenu->SubItems, "System", EMenuItemType::SubMenu, "SYSTEM_TAG");

	infoHandlers.Add(
		"SYS_FPS", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateUObject(this, &UDebugMenuWidget::GetFPS)));
	infoHandlers.Add(
		"SYS_RAM", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateUObject(this, &UDebugMenuWidget::GetMemoryUsage)));
	infoHandlers.
		Add("SYS_NC", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateUObject(this, &UDebugMenuWidget::NoClip)));

	AddItemToMenu(systemSubMenu->SubItems, "Performance", EMenuItemType::DisplayOnly, "SYS_FPS");
	AddItemToMenu(systemSubMenu->SubItems, "Memory", EMenuItemType::DisplayOnly, "SYS_RAM");
	AddItemToMenu(systemSubMenu->SubItems, "NoClip", EMenuItemType::Toggle, "SYS_NC");
}

void UDebugMenuWidget::InitPlayerMenu()
{
	playerSubMenu = AddItemToMenu(rootMenu->SubItems, "Player", EMenuItemType::SubMenu, "PLAYER_TAG");

	infoHandlers.Add("PL_LOC", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		return player ? player->GetActorLocation().ToCompactString() : "N/A";
	})));

	AddItemToMenu(playerSubMenu->SubItems, "Location", EMenuItemType::DisplayOnly, "PL_LOC");
}

void UDebugMenuWidget::InitPlayerMovementMenu()
{
	playerMovementSubMenu = AddItemToMenu(playerSubMenu->SubItems, "Movement", EMenuItemType::SubMenu, "PM_TAG");

	infoHandlers.Add("PM_Acc", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		return playerMovement ? FString::SanitizeFloat(playerMovement->MaxAcceleration) : "N/A";
	})));
	infoHandlers.Add("PM_Speed", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		return player ? FString::Printf(TEXT("%.2f cm/s"), player->GetVelocity().Size()) : "N/A";
	})));
	infoHandlers.Add("PM_Mode", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		if (!playerMovement) return FString("N/A");
		const UEnum* EnumPtr = StaticEnum<EMovementMode>();
		return EnumPtr ? EnumPtr->GetNameStringByValue(playerMovement->MovementMode) : FString("Error");
	})));
	infoHandlers.Add("PM_Grav", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		return playerMovement ? FString::SanitizeFloat(playerMovement->GravityScale) : "N/A";
	})));

	AddItemToMenu(playerMovementSubMenu->SubItems, "Acceleration", EMenuItemType::DisplayOnly, "PM_Acc");
	AddItemToMenu(playerMovementSubMenu->SubItems, "Current Speed", EMenuItemType::DisplayOnly, "PM_Speed");
	AddItemToMenu(playerMovementSubMenu->SubItems, "Movement Mode", EMenuItemType::DisplayOnly, "PM_Mode");
	AddItemToMenu(playerMovementSubMenu->SubItems, "Gravity Scale", EMenuItemType::DisplayOnly, "PM_Grav");
}

void UDebugMenuWidget::InitCameraMenu()
{
	cameraSubMenu = AddItemToMenu(playerSubMenu->SubItems, "Camera", EMenuItemType::SubMenu, "CAM_TAG");
	infoHandlers.Add(
		"CAM_FOV", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateUObject(this, &UDebugMenuWidget::GetCameraFov)));
	AddItemToMenu(cameraSubMenu->SubItems, "Field of View", EMenuItemType::DisplayOnly, "CAM_FOV");
}

void UDebugMenuWidget::InitWaypointMenu()
{
	waypointSubMenu = AddItemToMenu(rootMenu->SubItems, "Waypoints", EMenuItemType::SubMenu, "WP_TAG");
	RefreshWaypointMenu(); // Remplissage dynamique à partir de la sauvegarde
}

void UDebugMenuWidget::InitNodeMenu()
{
	nodeSubMenu = AddItemToMenu(rootMenu->SubItems, "Nodes", EMenuItemType::SubMenu, "NODE_TAG");

	// Action de Recherche de Node par ID
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
				// Réutilisation directe des sous-items déjà construits
				_resultMenuItem->SubItems = objectToMenuMap[_obj]->SubItems;
			}
		};

		StartSearch(_pool, _nodeFilter, _nodeBuilder);
		return FString("Searching Nodes...");
	})));

	infoHandlers.Add("NODE_HIDEDEBUGTEXT", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		_subNode->ShowAllDebugText(false);
		return FString("Hide Debug Text");
	})));
	infoHandlers.Add("NODE_SHOWDEBUGTEXT", FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		_subNode->ShowAllDebugText(true);
		return FString("Show Debug Text");
	})));


	AddItemToMenu(nodeSubMenu->SubItems, " [ Search by ID ] ", EMenuItemType::Action, "NODE_SEARCH");
	AddItemToMenu(nodeSubMenu->SubItems, " [ Hide Debug Text ]", EMenuItemType::Action, "NODE_HIDEDEBUGTEXT");
	AddItemToMenu(nodeSubMenu->SubItems, " [ Show Debug Text ]", EMenuItemType::Action, "NODE_SHOWDEBUGTEXT");

	// Liste exhaustive des Nodes via le Subsystem
	if (_subNode)
	{
		for (ANode* _node : _subNode->GetAllNodes())
		{
			if (!_node) continue;
			FName _nodeTag = FName(*FString::Printf(TEXT("NODE_SUB_%p"), _node));
			UDebugMenuItem* _indivMenu = AddItemToMenu(nodeSubMenu->SubItems, _node->GetNameID(), EMenuItemType::SubMenu,
			                                          _nodeTag);

			objectToMenuMap.Add(_node, _indivMenu);
			BuildNodeMenu(_node, _indivMenu);
		}
	}
}

/**
 * @summary Définit les actions possibles pour un Node (TP, Info).
 */
void UDebugMenuWidget::BuildNodeMenu(ANode* _node, UDebugMenuItem* _targetMenuItem)
{
	if (!_node || !_targetMenuItem) return;

	// On utilise l'adresse mémoire du node pour garantir l'unicité du tag
	FString _addressKey = FString::Printf(TEXT("%p"), _node);
	FName _infoTag = FName(*(TEXT("NODE_INFO_") + _addressKey));
	FName _tpTag = FName(*(TEXT("NODE_TP_") + _addressKey));
	FName _debugTag = FName(*(TEXT("NODE_DEBUG_") + _addressKey));
	FName _debugNameTag = FName(*(TEXT("NODE_DEBUGNAME_") + _addressKey));
	FName _selectTag = FName(*(TEXT("NODE_SELECT_") + _addressKey));
	FName _forceActivateTag = FName(*(TEXT("NODE_FORCEACTIVATE_") + _addressKey));
	FName _forceDeActivateTag = FName(*(TEXT("NODE_FORCEDEACTIVATE_") + _addressKey));


	// Informations
	infoHandlers.FindOrAdd(_infoTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([_node]()
	{
		return FString::Printf(
			TEXT("ID: %d | Debug: %s | DebugText: %s"), _node->GetId(),
			_node->GetShowDebug() ? TEXT("ON") : TEXT("OFF"),
			_node->GetVisibiltyDebugText() ? TEXT("ON") : TEXT("OFF"));
	}));

	//Teleportation
	infoHandlers.FindOrAdd(_tpTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this, _node]()
	{
		if (player) player->SetActorLocation(_node->GetActorLocation());
		return FString("Teleported");
	}));

	// Afficher Debug
	infoHandlers.FindOrAdd(_debugTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this, _node]()
	{
		if (IsValid(_node))
		{
			_node->SetShowDebug(!_node->GetShowDebug());
			UpdateMenuVisuals(); // Pour rafraîchir le texte ON/OFF immédiatement
		}
		return FString("");
	}));

	infoHandlers.FindOrAdd(_debugNameTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this, _node]()
	{
		if (IsValid(_node))
		{
			_node->ToogleShowDebugText();
			UpdateMenuVisuals(); // Pour rafraîchir le texte ON/OFF immédiatement
		}
		return FString("");
	}));

	infoHandlers.FindOrAdd(_selectTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([_node]()
	{
#if WITH_EDITOR
		if (GIsEditor && IsValid(_node))
		{
			// On vide la sélection actuelle
			GEditor->SelectNone(true, true, false);

			// On sélectionne notre node
			GEditor->SelectActor(_node, true, true);

			// On force le focus de l'Outliner sur l'objet 
			GEditor->NoteSelectionChange();

			return FString("Selected in Outliner");
		}
#endif
		return FString("Selection only works in Editor");
	}));

	// Force l'activation
	infoHandlers.FindOrAdd(_forceActivateTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([_node]()
	{
		_node->ForceActivate();
		return FString("Forced To Activate");
	}));

	// Force la desactivation
	infoHandlers.FindOrAdd(_forceDeActivateTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([_node]()
	{
		_node->ForceDeActivate();
		return FString("Forced To DeActivate");
	}));

	// Menu pour la liste des activateurs
	FName _activatorSubMenuTag = FName(*FString::Printf(TEXT("NODE_ACT_LIST_%p"), _node));
	UDebugMenuItem* _activatorListMenu = AddItemToMenu(_targetMenuItem->SubItems, ">> Activators List <<",
	                                                  EMenuItemType::SubMenu, _activatorSubMenuTag);

	// On appelle la fonction pour remplir ce sous-menu
	BuildActivatorListMenu(_node, _activatorListMenu);

	// On ajoute les items
	AddItemToMenu(_targetMenuItem->SubItems, "Status", EMenuItemType::DisplayOnly, _infoTag);
	AddItemToMenu(_targetMenuItem->SubItems, "Teleport", EMenuItemType::Action, _tpTag);
	AddItemToMenu(_targetMenuItem->SubItems, "Toggle Debug", EMenuItemType::Action, _debugTag);
	AddItemToMenu(_targetMenuItem->SubItems, "Toggle Names", EMenuItemType::Action, _debugNameTag);
	AddItemToMenu(_targetMenuItem->SubItems, "Select in Outliner", EMenuItemType::Action, _selectTag);
	AddItemToMenu(_targetMenuItem->SubItems, "Force Activate", EMenuItemType::Action, _forceActivateTag);
	AddItemToMenu(_targetMenuItem->SubItems, "Force DeActivete", EMenuItemType::Action, _forceDeActivateTag);
}

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

		// On génère une clé unique basée sur l'adresse du Node ET de l'Acteur
		FString _uniqueKey = FString::Printf(TEXT("%p_%p"), _node, _actor);

		FName _actSubTag = FName(*(TEXT("ACT_SUB_") + _uniqueKey));
		FName _actTpTag = FName(*(TEXT("ACT_TP_") + _uniqueKey));
		FName _actSelectTag = FName(*(TEXT("ACT_SELECT_") + _uniqueKey));

		// Créer le sous-menu pour CET acteur précis
		FString _actorLabel = FString::Printf(TEXT("[%s]"), *_actor->GetName());
		UDebugMenuItem* ActorEntry = AddItemToMenu(_targetMenuItem->SubItems, _actorLabel, EMenuItemType::SubMenu,
		                                           _actSubTag);

		//  Teleport vers l'Acteur
		infoHandlers.FindOrAdd(_actTpTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this, _actor]()
		{
			if (player && IsValid(_actor))
			{
				player->SetActorLocation(_actor->GetActorLocation() + FVector(0, 0, 90));
				return FString::Printf(TEXT("TP to %s"), *_actor->GetName());
			}
			return FString("Error: Actor Invalid");
		}));

		// Sélection dans l'Outliner
		infoHandlers.FindOrAdd(_actSelectTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([_actor]()
		{
#if WITH_EDITOR
			if (GIsEditor && IsValid(_actor))
			{
				GEditor->SelectNone(true, true, false);
				GEditor->SelectActor(_actor, true, true);
				GEditor->NoteSelectionChange();
				return FString("Selected in Outliner");
			}
#endif
			return FString("Editor only");
		}));

		// Ajouter les actions au sous-menu de l'acteur
		AddItemToMenu(ActorEntry->SubItems, "Teleport to Actor", EMenuItemType::Action, _actTpTag);
		AddItemToMenu(ActorEntry->SubItems, "Select in Outliner", EMenuItemType::Action, _actSelectTag);
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

	// Option : Sauvegarder la position actuelle
	infoHandlers.FindOrAdd("SaveWP") = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this]()
	{
		if (!textBoxClass) return FString("Class Null");
		UTextBoxUserWidget* _tb = CreateWidget<UTextBoxUserWidget>(this, textBoxClass);
		if (_tb)
		{
			_tb->AddToViewport();
			_tb->OnNameValidated.AddDynamic(this, &UDebugMenuWidget::SaveCurrentLocation);
		}
		return FString("Saved Waypoint");
	}));
	AddItemToMenu(waypointSubMenu->SubItems, "Save New Position", EMenuItemType::Action, "SaveWP");

	// Option : Tout supprimer
	infoHandlers.FindOrAdd("DeleteAllWP") = FHandlerMenuDebug(
		FGetDebugInfoDelegate::CreateUObject(this, &UDebugMenuWidget::DeleteAllWaypoint));
	AddItemToMenu(waypointSubMenu->SubItems, "Delete All", EMenuItemType::Action, "DeleteAllWP");

	// Itération sur la sauvegarde
	UDebugMenuSaveGame* _save = Cast<UDebugMenuSaveGame>(UGameplayStatics::LoadGameFromSlot("DebugWaypoints", 0));
	if (_save)
	{
		for (int _i = 0; _i < _save->savedWaypoints.Num(); _i++)
		{
			const FWaypointData& _wp = _save->savedWaypoints[_i];
			FName _baseTag = FName(*_wp.name);

			// Action TP (Index 0)
			FName _tpTag = FName(*(_baseTag.ToString() + "_0"));
			infoHandlers.FindOrAdd(_tpTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this, _wp]()
			{
				if (player) player->SetActorLocation(_wp.location);
				return "Teleport to " + _wp.name;
			}));

			// Action Suppr (Index 1)
			FName _delTag = FName(*(_baseTag.ToString() + "_1"));
			infoHandlers.FindOrAdd(_delTag) = FHandlerMenuDebug(FGetDebugInfoDelegate::CreateLambda([this, _i, _wp]()
			{
				DeleteWaypoint(_i);
				return FString("Delete " + _wp.name);
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

void UDebugMenuWidget::DeleteWaypoint(int _index)
{
	UDebugMenuSaveGame* _save = Cast<UDebugMenuSaveGame>(UGameplayStatics::LoadGameFromSlot("DebugWaypoints", 0));
	if (_save && _save->savedWaypoints.IsValidIndex(_index))
	{
		_save->savedWaypoints.RemoveAt(_index);
		UGameplayStatics::SaveGameToSlot(_save, "DebugWaypoints", 0);

		// Recalcul de l'index pour éviter de pointer hors tableau
		int32 NewTotal = waypointSubMenu->SubItems.Num();
		if (currentIndex >= NewTotal && NewTotal > 0)
		{
			currentIndex = NewTotal - 1;
		}

		RefreshWaypointMenu();
	}
}

void UDebugMenuWidget::SaveCurrentLocation(FString _name)
{
	UDebugMenuSaveGame* _save = Cast<UDebugMenuSaveGame>(UGameplayStatics::LoadGameFromSlot("DebugWaypoints", 0));
	if (!_save)
		_save = Cast<UDebugMenuSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UDebugMenuSaveGame::StaticClass()));

	FWaypointData _newWP;
	_newWP.name = _name;
	if (player) _newWP.location = player->GetActorLocation();

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

FString UDebugMenuWidget::GetFPS() { return FString::Printf(TEXT("%.0f FPS"), 1.0f / FApp::GetDeltaTime()); }

FString UDebugMenuWidget::GetMemoryUsage()
{
	FPlatformMemoryStats Stats = FPlatformMemory::GetStats();
	return FString::Printf(TEXT("%.0f MB"), (float)Stats.UsedPhysical / (1024.f * 1024.f));
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

			UDebugMenuItem* _item = rootMenu->GetItemByName("Waypoints");
			if (_item) _item->CanBeActive = false;
			return "NoClip ON";
		}
	}
	else // DESACTIVER NOCLIP
	{
		player->SetActorLocation(_currentSpectator->GetActorLocation());
		_currentSpectator->RemoveMapping();
		_pc->Possess(player);
		player->AddMapping();
		// player->RestoreDefaultInputs();
		_currentSpectator->Destroy();

		UDebugMenuItem* _item = rootMenu->GetItemByName("Waypoints");
		if (_item) _item->CanBeActive = true;
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

/**
 * @summary Gère l'appui sur la touche de validation.
 */
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
			                   ? FName(*(_selected->ActionTag.ToString() + "_" + FString::FromInt(
				                   _selected->CurrentActionIndex)))
			                   : _selected->ActionTag;
		
		if (infoHandlers.Contains(_actionKey))
		{
			FString _newValue = infoHandlers[_actionKey].delegate.Execute();

			if (!_newValue.IsEmpty())
			{
				if (hudOwner)
				{
					hudOwner->GetInGameWidget()->GetNotificationListWidget()->AddNotification(_newValue);
				}
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
	// 1. Vérifie si la pile est vide
	if (menuStack.Num() == 0) return;

	// 2. Vérifie si le pointeur du menu actuel est valide 
	UDebugMenuItem* _currentMenu = menuStack.Last();
	if (!IsValid(_currentMenu))
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveSelection: Le menu actuel dans la stack est invalide !"));
		return;
	}

	// Calcul de la nouvelle sélection
	int _num = _currentMenu->SubItems.Num();
	if (_num > 0)
	{
		currentIndex = (currentIndex + _dir + _num) % _num;
	}
	
	UpdateMenuVisuals();
}

/**
 * @summary Permet de changer d'action sur une ligne (ex: Gauche/Droite pour choisir TP ou Suppr).
 */
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
 * @summary Recyle les widgets de ligne et les affiche selon le scroll.
 */
void UDebugMenuWidget::UpdateMenuVisuals()
{
	if (!MenuConainer_show || !MenuConainer_Hidden) return;

	TArray<UWidget*> _children = MenuConainer_show->GetAllChildren();
	for (UWidget* _widget : _children) MenuConainer_Hidden->AddChild(_widget);
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

/**
 * @summary Détermine quels items du menu actuel doivent être rendus (système de scroll).
 */
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
 * @summary Mis à jour par le timer pour actualiser les textes dynamiques (DisplayOnly).
 */
void UDebugMenuWidget::RefreshVisibleMenu()
{
	if (menuStack.Num() == 0) return;

	bool _needsVisualUpdate = false;
	for (UDebugMenuItem* _item : menuStack.Last()->SubItems)
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

/**
 * @summary Utilitaire pour créer un objet de menu et l'ajouter à une liste cible.
 */
UDebugMenuItem* UDebugMenuWidget::AddItemToMenu(TArray<UDebugMenuItem*>& _target, FString _label, EMenuItemType _type,
                                                FName _tag, TArray<FString> _actions, bool _active)
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
