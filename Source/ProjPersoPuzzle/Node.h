#pragma once

#include "CoreMinimal.h"
#include "PuzzleCharacter.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Node.generated.h"

class UNotificationListWidget;

UCLASS(Abstract)
class PROJPERSOPUZZLE_API ANode : public AActor
{
	GENERATED_BODY()
	UPROPERTY() APuzzleCharacter* player = nullptr;
	UPROPERTY() UCameraComponent* playerCamera = nullptr;
public:
	ANode();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Node Settings")
	int id = 0;
	UPROPERTY(VisibleAnywhere, Category = "Node Settings")
	FString name = "";

	UPROPERTY(VisibleAnywhere, Category = "Node Settings")
	TArray<AActor*> actorActiveNode = TArray<AActor*>();
	
	UPROPERTY(VisibleAnywhere, Category = "Node Settings")
	UNotificationListWidget* notifUIRef = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Node Settings")
	bool showDebug = false;

	UPROPERTY(EditAnywhere, Category = "Node Settings")
	bool isActivated = false;

	UPROPERTY(EditAnywhere, Category = "Node Settings")
	TArray<ANode*> nodeToActivate;

	UPROPERTY(EditAnywhere, Category = "Node Settings")
	USoundBase* activationSound;

	UPROPERTY(EditAnywhere, Category = "Node Feedback")
	UTextRenderComponent* textDebug = nullptr;
	
	bool isProcessing = false;

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void Tick(float DeltaTime) override;
#endif

	virtual bool ShouldTickIfViewportsOnly() const override { return true; }


	UFUNCTION(BlueprintCallable)
	virtual void OnActivate(AActor* _other);
	UFUNCTION(BlueprintCallable)
	virtual void OnDeActivate(AActor* _other);
	virtual void SendSignal(bool _status, AActor* _other);

	virtual void ShowDebug();

	void PlayNodeFeedback();

public:
	virtual void SetId(int _id) { id = _id; }
	virtual int GetId() { return id; }
	virtual FString GetName() { return name; }
	virtual void SetName(FString _name) { name = _name; }
	virtual FString GetNameID() { return name + "_" + FString::FromInt(id); }
	virtual void SetShowDebug(bool _showDebug) { showDebug = _showDebug; }
	virtual bool GetShowDebug() { return showDebug; }
	virtual void ToogleShowDebugText() { textDebug->SetVisibility(!textDebug->IsVisible()); }
	virtual void SetVisibiltyDebugText(const bool _value) { textDebug->SetVisibility(_value); }
	virtual bool GetVisibiltyDebugText() { return textDebug->IsVisible(); }
	UFUNCTION(BlueprintCallable) virtual void AddActorToList(AActor* _actor) { actorActiveNode.Add(_actor); }
	UFUNCTION(BlueprintCallable) virtual void RemoveActorToList(AActor* _actor) { actorActiveNode.Remove(_actor); }
	UFUNCTION(BlueprintCallable) virtual TArray<AActor*> GetAllActorToList() { return actorActiveNode; }
	UFUNCTION(BlueprintCallable) virtual void ForceActivate();
	UFUNCTION(BlueprintCallable) virtual void ForceDeActivate();

	virtual void GetNotifUI();

};
