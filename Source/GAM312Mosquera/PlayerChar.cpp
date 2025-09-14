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

}

// Called every frame
void APlayerChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	// If line trace is succesful
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
	{
		// Set hit resource to true if a resource actor was hit by the line
		AResource_M* HitResource = Cast<AResource_M>(HitResult.GetActor());

		// Add resource to inventory if stamina is greater than five
		if (Stamina > 5.0f)
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
				if (HitResource->totalResource > resourceValue)
				{
					// Add resource to inventory
					GiveResource(resourceValue, hitName);

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
					// Display debug line that the resource can no longer be collected as there are no resources left in the actor
					HitResource->Destroy();
					check(GEngine != nullptr);
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Resource Depleted"));

				}
			}
		}
	}
}

// Set player stat functions
void APlayerChar::SetHealth(float amount)
{
	if (Health + amount <= 100)
	{
		Health = Health + amount;

	}
}

void APlayerChar::SetHunger(float amount)
{
	if (Hunger + amount <= 100)
	{
		Hunger = Hunger + amount;

	}
}

void APlayerChar::SetStamina(float amount)
{
	if (Stamina + amount <= 100)
	{
		Stamina = Stamina + amount;

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

