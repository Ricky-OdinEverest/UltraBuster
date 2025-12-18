// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryList.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "FastArrayTagCounter.h"
#include "InventoryComponent.generated.h"

//Manages Current Held Inventory

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ULTRABUSTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void InitializeComponent() override;

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	UFUNCTION(BlueprintCallable)
	void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	UFUNCTION(BlueprintCallable)
	void AddItemInstance(UInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	UFUNCTION(BlueprintCallable)
	void RemoveItemInstance(UInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable)
	void RemoveItemWithInventoryTag(FGameplayTag Tag, int32 Count = 1);

	UFUNCTION(BlueprintCallable)
	void EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	UFUNCTION(BlueprintCallable)
	void EquipItemInstance(UInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable)
	void UnequipItem();

	UFUNCTION(BlueprintCallable)
	void DropItem();

	UFUNCTION(BlueprintCallable)
	void EquipNext();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UInventoryItemInstance* GetEquippedItem() const;

	virtual void GameplayEventCallback(const FGameplayEventData* Payload);

	static FGameplayTag EquipItemActorTag;
	static FGameplayTag DropItemTag;
	static FGameplayTag EquipNextTag;
	static FGameplayTag UnequipTag;

	UFUNCTION(BlueprintCallable)
	int32 GetInventoryTagCount(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable)
	void AddInventoryTagCount(FGameplayTag InTag, int32 CountDelta);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void AddInventoryTags();

	//Replicated List Of Items in players possesion
	UPROPERTY(Replicated)
	FInventoryList InventoryList;

	//Starting Inventory Given To The PLayer On Initialization
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UItemStaticData>> DefaultItems;

	UPROPERTY(Replicated)
	UInventoryItemInstance* CurrentItem = nullptr;

	UPROPERTY(Replicated)
	FFastArrayTagCounter InventoryTags;

	TArray<UInventoryItemInstance*> GetAllInstancesWithTag(FGameplayTag Tag);

	void HandleGameplayEventInternal(FGameplayEventData Payload);

	UFUNCTION(Server, Reliable)
	void ServerHandleGameplayEvent(FGameplayEventData Payload);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};