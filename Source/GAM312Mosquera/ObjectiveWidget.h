// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAM312MOSQUERA_API UObjectiveWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Functions to update progress on objectives
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateMatsObj(float matsCollected);

	UFUNCTION(BlueprintImplementableEvent)
		void UpdateBuildsObj(float objectsBuilt);

};
