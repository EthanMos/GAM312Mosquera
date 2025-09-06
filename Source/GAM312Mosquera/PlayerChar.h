// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
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

	UFUNCTION()
		void MoveForward(float axisValue); // Move on the x axis function initialization

	UFUNCTION()
		void MoveRight(float axisValue); // Move on the y axis function initialization

	UFUNCTION()
		void StartJump(); // Start jump function initialization

	UFUNCTION()
		void StopJump(); // Stop jump function initialization

	UFUNCTION()
		void FindObject();

	UPROPERTY(VisibleAnywhere)
		UCameraComponent* PlayerCamComp; // Add camera component as PlayerCamComp
};
