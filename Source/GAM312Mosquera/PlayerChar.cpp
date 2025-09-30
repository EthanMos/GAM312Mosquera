// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerChar.h"

// Sets default values
APlayerChar::APlayerChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create first person camera
	PlayerCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Cam"));

	// Attach camera to player's head
	PlayerCamComp->SetupAttachment(GetMesh(), "head");

	// Rotate camera with pawn
	PlayerCamComp->bUsePawnControlRotation = true;

	// Set up building array
	BuildingArray.SetNum(3);

	// Set up resource array
	ResourcesArray.SetNum(3);
	ResourcesNameArray.Add(TEXT("Wood"));
	ResourcesNameArray.Add(TEXT("Stone"));
	ResourcesNameArray.Add(TEXT("Berry"));

}

// Called when the game starts or when spawned
void APlayerChar::BeginPlay()
{
	Super::BeginPlay();
	
	// Set timer to decrease hunger/health and increase stamina
	FTimerHandle StatsTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StatsTimerHandle, this, &APlayerChar::DecreaseStats, 2.0f, true);

	// If the objective widget exists set the objectives for mats collected and builds built
	if (objWidget)
	{
		objWidget->UpdateMatsObj(0.0f);
		objWidget->UpdateBuildsObj(0.0f);

	}
}

// Called every frame
void APlayerChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Updates player's stats to player widget every tick
	playerUI->UpdateBars(Health, Hunger, Stamina);

	// Checks if player is building
	if (isBuilding)
	{
		// Checks if part exists 
		if (spawnedPart)
		{
			// Traces location to build
			FVector StartLocation = PlayerCamComp->GetComponentLocation();
			FVector Direction = PlayerCamComp->GetForwardVector() * 400.0f;
			FVector EndLocation = StartLocation + Direction;
			spawnedPart->SetActorLocation(EndLocation);

		}
	}
}

// Called to bind functionality to input
void APlayerChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Inputs call functions
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerChar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerChar::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerChar::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerChar::AddControllerYawInput);
	PlayerInputComponent->BindAction("JumpEvent", IE_Pressed, this, &APlayerChar::StartJump);
	PlayerInputComponent->BindAction("JumpEvent", IE_Released, this, &APlayerChar::StopJump);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerChar::FindObject);
	PlayerInputComponent->BindAction("RotPart", IE_Pressed, this, &APlayerChar::RotateBuilding);


}

// Rotate and move based on input
void APlayerChar::MoveForward(float axisValue)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, axisValue);

}

void APlayerChar::MoveRight(float axisValue)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, axisValue);

}

// Jump based on input press
void APlayerChar::StartJump()
{
	bPressedJump = true;

}

void APlayerChar::StopJump()
{
	bPressedJump = false;

}

//  Interact with objects
void APlayerChar::FindObject()
{
	// Find object using camera forward vector
	FHitResult HitResult;
	FVector StartLocation = PlayerCamComp->GetComponentLocation();
	FVector Direction = PlayerCamComp->GetForwardVector() * 800.0f;
	FVector EndLocation = StartLocation + Direction;

	// Trace line to object 
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnFaceIndex = true;

	// If player is not building
	if (!isBuilding)
	{
		// If line trace is succesful
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
		{
			// Set hit resource to true if a resource actor was hit by the line
			AResource_M* HitResource = Cast<AResource_M>(HitResult.GetActor());

			// Add resource to inventory if stamina is greater than five
			if (Stamina >= 5.0f)
			{

				// Check if a resource was hit
				if (HitResource)
				{
					// Get resource stats
					FString hitName = HitResource->resourceName;
					int resourceValue = HitResource->resourceAmount;

					// Lower resource amount from actor
					HitResource->totalResource = HitResource->totalResource - resourceValue;

					// Check if resource can be gathered
					if (HitResource->totalResource >= resourceValue)
					{
						// Add resource to inventory
						GiveResource(resourceValue, hitName);

						// Tracks resources collected in the objectives widget
						matsCollected = matsCollected + resourceValue;
						objWidget->UpdateMatsObj(matsCollected);

						// Display debug line showing that the resource is being collected
						check(GEngine != nullptr);
						GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Resource Collected"));

						// Show hit marker on object's surface
						UGameplayStatics::SpawnDecalAtLocation(GetWorld(), hitDecal, FVector(10.0f, 10.0f, 10.0f), HitResult.Location, FRotator(-90, 0, 0), 2.0f);

						// Lower stamina
						SetStamina(-5.0f);

					}
					else
					{
						// Add resource to inventory
						GiveResource(resourceValue, hitName);

						// Display debug line that the resource can no longer be collected as there are no resources left in the actor
						HitResource->Destroy();
						check(GEngine != nullptr);
						GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Resource Depleted"));

					}
				}
			}
		}
	}
	else
	{
		// When player interacts while building is true, set building to false
		isBuilding = false;

		// Updates the amount on objects built when player builds in the objectives widget
		objectsBuilt = objectsBuilt + 1.0f;
		objWidget->UpdateBuildsObj(objectsBuilt);

	}
}

// Set player stat functions
void APlayerChar::SetHealth(float amount)
{
	// Check if amount added will result in health that is greater than the cap, if so set health to the cap
	if (Health + amount < 100)
	{
		Health = Health + amount;

	}
	else
	{
		Health = 100.0f;

	}
}

void APlayerChar::SetHunger(float amount)
{
	// Check if amount added will result in hunger that is greater than the cap, if so set hunger to the cap
	if (Hunger + amount < 100)
	{
		Hunger = Hunger + amount;

	}
	else
	{
		Hunger = 100.0f;

	}
}

void APlayerChar::SetStamina(float amount)
{
	// Check if amount added will result in stamina that is greater than the cap, if so set stamina to the cap
	if (Stamina + amount < 100)
	{
		Stamina = Stamina + amount;

	}
	else
	{
		Stamina = 100.0f;

	}
}

void APlayerChar::DecreaseStats()
{
	// Decrease hunger until honger hits zero, then decrease health
	if (Hunger > 0)
	{
		SetHunger(-1.0f);

	}

	if (Hunger <= 0)
	{
		SetHealth(-3.0f);

	}

	// Add stamina
	SetStamina(10.0f);

}

// Add resources to array function
void APlayerChar::GiveResource(float amount, FString resourceType)
{
	if (resourceType == "Wood")
	{
		ResourcesArray[0] = ResourcesArray[0] + amount;

	}

	if (resourceType == "Stone")
	{
		ResourcesArray[1] = ResourcesArray[1] + amount;

	}

	if (resourceType == "Berry")
	{
		ResourcesArray[2] = ResourcesArray[2] + amount;

	}
}

// Lower resources and adds builds to inventory when called
void APlayerChar::UpdateResource(float woodAmount, float stoneAmount, FString buildingObject)
{
	if (woodAmount <= ResourcesArray[0])
	{
		if (stoneAmount <= ResourcesArray[1])
		{
			ResourcesArray[0] = ResourcesArray[0] - woodAmount;
			ResourcesArray[1] = ResourcesArray[1] - stoneAmount;

			if (buildingObject == "Wall")
			{
				BuildingArray[0] = BuildingArray[0] + 1;

			}

			if (buildingObject == "Floor")
			{
				BuildingArray[1] = BuildingArray[1] + 1;

			}

			if (buildingObject == "Ceiling")
			{
				BuildingArray[2] = BuildingArray[2] + 1;

			}
		}
	}
}

// Spawns builds
void APlayerChar::SpawnBuilding(int buildingID, bool& isSuccess)
{
	// Checks if player is currently in build mode
	if (!isBuilding)
	{
		// Checks if player has builds
		if (BuildingArray[buildingID] >= 1)
		{
			// Allows player to build and sets up tracing
			isBuilding = true;
			FActorSpawnParameters SpawnParams;
			FVector StartLocation = PlayerCamComp->GetComponentLocation();
			FVector Direction = PlayerCamComp->GetForwardVector() * 400.0f;
			FVector EndLocation = StartLocation + Direction;
			FRotator myRot(0, 0, 0);

			// Update builds remaining
			BuildingArray[buildingID] = BuildingArray[buildingID] - 1;

			// Spawn build
			spawnedPart = GetWorld()->SpawnActor<ABuildingPart>(BuildPartClass, EndLocation, myRot, SpawnParams);

			isSuccess = true;

		}
		// Returns false bool if build does not place
		isSuccess = false;

	}
}

// Roatates builds
void APlayerChar::RotateBuilding()
{
	if (isBuilding)
	{
		spawnedPart->AddActorWorldRotation(FRotator(0, 90, 0));

	}
}

