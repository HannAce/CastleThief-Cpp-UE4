// Copyright Hannah Fisher, University of Brighton, 2020

#include "RotateDoor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
URotateDoor::URotateDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URotateDoor::BeginPlay()
{
	Super::BeginPlay();

	// Set initial values for door before rotating
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	// Helps protect against error and UE crashing due to NULL pointer
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component attached, but no PressurePlate set!"), *GetOwner()->GetName());
	}

	ActorThatTriggers = GetWorld()->GetFirstPlayerController()->GetPawn(); // Sets Player/Default Pawn to be the trigger to open door
}


// Called every frame
void URotateDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Checks if a PressurePlate has been assigned to prevent NULL pointer error, and if trigger conditions are true to open door.
	if (PressurePlate && PressurePlate->IsOverlappingActor(ActorThatTriggers)) {
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else {
		if ((GetWorld()->GetTimeSeconds() - DoorLastOpened) > DoorCloseDelay) {
			CloseDoor(DeltaTime);
		}
	}
}

// Updates/sets the yaw rotation to open doors
void URotateDoor::OpenDoor(float DeltaTime) {

	CurrentYaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, DoorOpenSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
}

// Updates/sets the yaw rotation to close doors
void URotateDoor::CloseDoor(float DeltaTime) {

	CurrentYaw = FMath::FInterpConstantTo(CurrentYaw, InitialYaw, DeltaTime, DoorCloseSpeed); //Door slam shut quicker and linear speed
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
}

