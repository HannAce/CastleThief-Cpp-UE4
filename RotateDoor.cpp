// Copyright Hannah Fisher, University of Brighton, 2020

#include "RotateDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"

#define OUT

// Sets default values for this component's properties
URotateDoor::URotateDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void URotateDoor::BeginPlay() {
	Super::BeginPlay();

	// Set initial values for door before rotating
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	FindPressurePlate();
	FindAudioComponent();
}

// Helps protect against error and UE crashing due to NULL pointer
void URotateDoor::FindPressurePlate() {
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component attached, but no PressurePlate set!"),
			*GetOwner()->GetName());
	}
}

// Check for audio component
void URotateDoor::FindAudioComponent() {

	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (!AudioComponent) { // Safety net to avoid potential errors/crashes if pointer returns null
		UE_LOG(LogTemp, Error, TEXT("No audio component found on %s!"), *GetOwner()->GetName());
	}
}

float URotateDoor::TotalMassOfActors() const {

	float TotalMass = 0.f;

	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) { // Safety net to avoid potential errors/crashes if pointer returns null
		return TotalMass;
	}
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	for (AActor* Actor : OverlappingActors) {
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	
	return TotalMass;
}

// Called every frame
void URotateDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Checks if a PressurePlate has been assigned to prevent NULL pointer error, and if trigger conditions are true to open door.
	if (TotalMassOfActors() >= MassToOpenDoor) {
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

	if (!AudioComponent) {
		return;
	}
	CloseDoorSound = false;
	if (!OpenDoorSound) {
		AudioComponent->Play();
		OpenDoorSound = true;
	}
}

// Updates/sets the yaw rotation to close doors
void URotateDoor::CloseDoor(float DeltaTime) {

	CurrentYaw = FMath::FInterpConstantTo(CurrentYaw, InitialYaw, DeltaTime, DoorCloseSpeed); //Door slam shut quicker and linear speed
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
	if (!AudioComponent) { 
		return;
	}
	OpenDoorSound = false;
	if (!CloseDoorSound) {
		AudioComponent->Play();
		CloseDoorSound = true;
	}
}
