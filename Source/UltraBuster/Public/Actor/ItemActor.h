// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BusterTypes.h"
#include "ItemActor.generated.h"

UCLASS()
class ULTRABUSTER_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActor();

	virtual void OnEquipped();
	virtual void OnUnequipped();
	virtual void OnDropped();

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	void Init(UInventoryItemInstance* InInstance);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_ItemInstance)
	UInventoryItemInstance* ItemInstance = nullptr;

	UFUNCTION()
	void OnRep_ItemInstance(UInventoryItemInstance* OldItemInstance);

	UPROPERTY(ReplicatedUsing = OnRep_ItemState)
	EItemState ItemState = EItemState::None;

	UFUNCTION()
	void OnRep_ItemState();

	UPROPERTY()
	class USphereComponent* SphereComponent = nullptr;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

	UPROPERTY(EditAnywhere, Replicated)
	int32 Quantity = 1;

	virtual void InitInternal();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
