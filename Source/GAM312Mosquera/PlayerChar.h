// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Resource_M.h"
#include "Kismet/GameplayStatics.h"
#include "BuildingPart.h"
#include "PlayerWidget.h"
#include "PlayerChar.generated.h"

UCLASS()
class GAM312MOSQUERA_API APlayerChar : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerChar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Move on the x and y axes functions initialization
	UFUNCTION()
		void MoveForward(float axisValue);

	UFUNCTION()
		void MoveRight(float axisValue);

	// Start and stop jump function initialization
	UFUNCTION()
		void StartJump();

	UFUNCTION()
		void StopJump();

	// Find object fun ction initialization
	UFUNCTION()
		void FindObject();

	// Add camera component as PlayerCamComp
	UPROPERTY(VisibleAnywhere)
		UCameraComponent* PlayerCamComp;

	// Player stat initialization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
		float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
		float Hunger = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
		float Stamina = 100.0f;

	// Resource values initialization
	UPROPERTY(EditAnywhere, Category = "Resources")
		int Wood;

	UPROPERTY(EditAnywhere, Category = "Resources")
		int Stone;

	UPROPERTY(EditAnywhere, Category = "Resources")
		int Berry;

	// Resource array index and name initialization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		TArray<int> ResourcesArray;

	UPROPERTY(EditAnywhere, Category = "Resources")
		TArray<FString> ResourcesNameArray;

	// Hit decal material initialization
	UPROPERTY(EditAnywhere, Category = "Hit Marker")
		UMaterialInterface* hitDecal;

	// Building array initialization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Supplies")
		TArray<int> BuildingArray;

	// Build mode boolean initialization
	UPROPERTY()
		bool isBuilding;

	// Class initialization for building parts
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<ABuildingPart> BuildPartClass;

	// Current part being built
	UPROPERTY()
		ABuildingPart* spawnedPart;

	// Player widget reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPlayerWidget* playerUI;

	// Set player stats function initialization
	UFUNCTION(BlueprintCallable)
		void SetHealth(float amount);

	UFUNCTION(BlueprintCallable)
		void SetHunger(float amount);

	UFUNCTION(BlueprintCallable)
		void SetStamina(float amount);

	UFUNCTION()
		void DecreaseStats();

	// Add resource to array function initialization
	UFUNCTION()
		void GiveResource(float amount, FString resourceType);

	// Function that updates resources and builds
	UFUNCTION(BlueprintCallable)
		void UpdateResource(float woodAmount, float stoneAmount, FString buildingObject);

	// Spawn builds function initialization
	UFUNCTION(BlueprintCallable)
		void SpawnBuilding(int buildingID, bool& isSuccess);

	// Build rotation function initialization
	UFUNCTION()
		void RotateBuilding();

};
