#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DebugMenuItem.h"
#include "DebugMenuSaveGame.h"
#include "Node.h"
#include "TextBoxUserWidget.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "DebugMenuWidget.generated.h"

/**
 * @summary Délégué renvoyant une String. Utilisé pour récupérer dynamiquement 
 * les infos de debug (FPS, Position, etc.) à chaque rafraîchissement.
 */
DECLARE_DELEGATE_RetVal(FString, FGetDebugInfoDelegate);

class UCameraComponent;
class UCharacterMovementComponent;
class APuzzleCharacter;
class UMenuLineWidget;
class UNodesWorldSubsystem;
class AInGameHUD;

/**
 * @summary Structure liant un délégué d'exécution à une description.
 * Sert de "Handler" pour toutes les actions déclenchées par le menu.
 */
USTRUCT()
struct FHandlerMenuDebug
{
	GENERATED_BODY()

	FGetDebugInfoDelegate delegate;
	FName description;

	FHandlerMenuDebug() : description("")
	{
	}

	FHandlerMenuDebug(FGetDebugInfoDelegate _delegate, FName _description = "")
		: delegate(_delegate), description(_description)
	{
	}
};

/**
 * @summary Widget principal du Menu de Debug.
 * Gère l'arborescence des menus, la navigation, le système de recherche et l'affichage dynamique.
 */
UCLASS()
class PROJPERSOPUZZLE_API UDebugMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY() AInGameHUD* hudOwner = nullptr;
	
#pragma region CYCLE_DE_VIE

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

#pragma endregion

#pragma region UI_COMPONENTS

	/// @summary Container affichant les lignes actives du menu.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> MenuConainer_show = nullptr;

	/// @summary Pool de widgets de lignes cachés pour le recyclage (optimisation).
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> MenuConainer_Hidden = nullptr;

	/// @summary Texte affichant le titre du menu ou du sous-menu actuel.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelText = nullptr;

#pragma endregion

#pragma region CONFIGURATION

	/// @summary Classe du widget de saisie de texte pour la recherche ou le nommage.
	UPROPERTY(EditAnywhere, Category = "Debug Menu | Config")
	TSubclassOf<UTextBoxUserWidget> textBoxClass;

	/// @summary Classe du widget représentant une ligne de menu.
	UPROPERTY(EditAnywhere, Category = "Debug Menu | Config")
	TSubclassOf<UMenuLineWidget> menuLineClass;

	/// @summary Classe du Pawn utilisé pour le mode NoClip.
	UPROPERTY(EditAnywhere, Category = "Debug Menu | Config")
	TSubclassOf<class ASpectatorPawn> spectatorClassBP;

	/// @summary Nombre maximum d'éléments affichés simultanément avant de scroller.
	UPROPERTY(EditAnywhere, Category = "Debug Menu | Config")
	int maxVisibleItems = 10;

#pragma endregion

#pragma region NAVIGATION_INTERNE

	/// @summary Pile de menus permettant de gérer le retour en arrière (Back).
	UPROPERTY()
	TArray<UDebugMenuItem*> menuStack;

	/// @summary Index de l'élément actuellement sélectionné.
	int currentIndex = 0;

	/// @summary Index du premier élément affiché dans la liste (gestion du scroll).
	int firstVisibleIndex = 0;

#pragma endregion

#pragma region SYSTEME_RECHERCHE

	/// @summary Définition du type de fonction pour construire le contenu d'un résultat de recherche.
	using FResultMenuBuilder = TFunction<void(UObject*, UDebugMenuItem*)>;

	/// @summary Pool d'objets dans lequel la recherche actuelle s'effectue.
	TArray<UObject*> currentSearchPool;

	/// @summary Filtre de recherche actif (comparaison texte/objet).
	TFunction<bool(UObject*, FString)> currentSearchFilter;

	/// @summary Builder actif pour injecter les sous-items dans les résultats.
	FResultMenuBuilder currentResultBuilder;

	/**
	 * @summary Lance l'interface de recherche.
	 * @param _pool : Liste d'objets (Nodes, etc.).
	 * @param _filter : Logique de filtrage (ex: par ID).
	 * @param _customMenuBuilder : Logique de construction du sous-menu résultat.
	 */
	void StartSearch(TArray<UObject*> _pool, TFunction<bool(UObject*, FString)> _filter,
	                 FResultMenuBuilder _customMenuBuilder);

	/// @summary Exécute le filtrage et génère le menu de résultats.
	UFUNCTION()
	void ExecuteUniversalSearch(FString _searchText);

#pragma endregion

#pragma region INITIALISATION_MENUS

	/// @summary Référence racine du menu.
	UPROPERTY()
	UDebugMenuItem* rootMenu;

	// Références vers les catégories principales
	UPROPERTY()
	UDebugMenuItem* systemSubMenu;
	UPROPERTY()
	UDebugMenuItem* playerSubMenu;
	UPROPERTY()
	UDebugMenuItem* playerMovementSubMenu;
	UPROPERTY()
	UDebugMenuItem* cameraSubMenu;
	UPROPERTY()
	UDebugMenuItem* waypointSubMenu;
	UPROPERTY()
	UDebugMenuItem* nodeSubMenu;

	/// @summary Mapping permettant de retrouver le menu d'un Node/Objet sans le reconstruire.
	UPROPERTY()
	TMap<UObject*, UDebugMenuItem*> objectToMenuMap;

	void InitMenus();
	void InitSystemMenu();
	void InitPlayerMenu();
	void InitPlayerMovementMenu();
	void InitCameraMenu();
	void InitWaypointMenu();
	void InitNodeMenu();

	/**
	 * @summary Construit les actions (TP, Info) pour un Node spécifique.
	 */
	void BuildNodeMenu(ANode* Node, UDebugMenuItem* TargetMenuItem);
	void BuildActivatorListMenu(ANode* Node, UDebugMenuItem* TargetMenuItem);

#pragma endregion

#pragma region LOGIQUE_AFFICHAGE

	/// @summary Met à jour les widgets visuels (lignes) selon l'état actuel.
	void UpdateMenuVisuals();

	/// @summary Appelé par timer pour rafraîchir les valeurs DisplayOnly (FPS, etc.).
	void RefreshVisibleMenu();

	/**
	 * @summary Utilitaire de création d'item de menu.
	 */
	UDebugMenuItem* AddItemToMenu(TArray<UDebugMenuItem*>& _target, FString _label, EMenuItemType _type,
	                              FName _tag = "", TArray<FString> _actions = {}, bool _canBeActive = true);

	/// @summary Calcule la liste d'items à afficher selon le scroll.
	TArray<UDebugMenuItem*> GetVisibleMenuItems();

#pragma endregion

#pragma region GESTION_WAYPOINTS

	void RefreshWaypointMenu();
	void DeleteWaypoint(int Index);

	UFUNCTION()
	FString DeleteAllWaypoint();
	UFUNCTION()
	void SaveCurrentLocation(FString _customName);

#pragma endregion

#pragma region STATS_ET_HELPERS

	FString GetFPS();
	FString GetMemoryUsage();
	FString NoClip();
	FString GetCameraFov();

#pragma endregion

#pragma region REFERENCES_ET_HANDLERS

	/// @summary Référence au personnage possédé.
	UPROPERTY()
	APuzzleCharacter* player;

	/// @summary Référence au composant de mouvement du joueur.
	UPROPERTY()
	UCharacterMovementComponent* playerMovement;

	/// @summary Référence à la caméra principale.
	UPROPERTY()
	UCameraComponent* playerCamera;

	/// @summary Timer gérant la mise à jour des données dynamiques.
	FTimerHandle refreshTimerHandle;

	/// @summary Map reliant les Tags des items aux fonctions C++ (Handlers).
	UPROPERTY()
	TMap<FName, FHandlerMenuDebug> infoHandlers;
	UPROPERTY()
	FTimerHandle resetValueTimerHandle;
	UPROPERTY()
	UNodesWorldSubsystem* _subNode = nullptr;
	
#pragma endregion

	UFUNCTION() void ResetDynamicValue(UDebugMenuItem* Item);
	

public:
#pragma region API_NAVIGATION_BLUEPRINT

	/// @summary Déplace la sélection de haut en bas.
	UFUNCTION(BlueprintCallable, Category = "Debug Menu | Navigation")
	void MoveSelection(int _direction);

	/// @summary Permet de changer d'action (TP/Suppr) sur l'item sélectionné.
	UFUNCTION(BlueprintCallable, Category = "Debug Menu | Navigation")
	void MoveActionSelection(int _direction);

	/// @summary Valide l'item actuel (Entrée).
	UFUNCTION(BlueprintCallable, Category = "Debug Menu | Navigation")
	void SelectItem();

	/// @summary Revient au menu parent (Retour).
	UFUNCTION(BlueprintCallable, Category = "Debug Menu | Navigation")
	void GoBack();

	/// @summary Event appelé quand le menu change pour déclencher des effets en Blueprint.
	UFUNCTION(BlueprintImplementableEvent, Category = "Debug Menu | Events")
	void OnMenuUpdated();

#pragma endregion

	void SetHudOwner(AInGameHUD* _hudOwner) {hudOwner = _hudOwner;}
};


