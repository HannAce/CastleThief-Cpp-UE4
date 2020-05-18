// Copyright Hannah Fisher, University of Brighton, 2020

#include "GrabObject.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UGrabObject::UGrabObject() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabObject::BeginPlay() {
	Super::BeginPlay();

	FindPhysicsHandle();
	SetUpInputComponent();
}

// Check for PhysicsHandle Component
void UGrabObject::FindPhysicsHandle() {
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle) { // Safety net to avoid potential errors/crashes if pointer returns null
		UE_LOG(LogTemp, Error, TEXT("No Physics Handle component found on %s!"), *GetOwner()->GetName());
	}
}

// Sets up the input for grabbing and releasing objects
void UGrabObject::SetUpInputComponent() {
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabObject::Grab);
		InputComponent->BindAction("ReleaseGrab", IE_Pressed, this, &UGrabObject::ReleaseGrab);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("No Input component found on %s!"), *GetOwner()->GetName());
	}
}

// Gets the player's location
FVector UGrabObject::GetPlayerLocation() const {

	FVector ViewpointLocation;
	FRotator ViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT ViewpointLocation, OUT ViewpointRotation);
	// DrawDebugSphere(GetWorld(), ViewpointLocation, 5, 16, FColor::Cyan, false, 1, 0, 1);


	return ViewpointLocation;
}

// Gets the player's rotation
FRotator UGrabObject::GetPlayerRotation() const {

	FVector ViewpointLocation;
	FRotator ViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT ViewpointLocation, OUT ViewpointRotation);

	return ViewpointRotation;
}

// Raycast gets the distance the player can reach to pick up an object
FVector UGrabObject::GetPlayerReach() const {
	FVector ReachLocation = GetPlayerLocation() + (GetPlayerRotation().RotateVector(FVector::ForwardVector)	* PlayerReachDistance);
	return ReachLocation;
}

// Function to pick up an object
void UGrabObject::Grab() {
	FHitResult HitResult;
	bool Success = GetFirstPhysicsBodyInReach(HitResult);
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	if (Success)
	{
		AActor* ActorHit = HitResult.GetActor();

		// If something is hit, attach physics handle
		if (ActorHit) {
			if (!PhysicsHandle) { // Safety net to avoid potential errors/crashes if pointer returns null
				return;
			}
			PhysicsHandle->GrabComponentAtLocationWithRotation(ComponentToGrab, NAME_None, GetPlayerReach(),
				GetPlayerRotation());
		}
	}
}

// Function to release a held object
void UGrabObject::ReleaseGrab() {
	if (!PhysicsHandle) { // Safety net to avoid potential errors/crashes if pointer returns null
		return;
	}
	PhysicsHandle->ReleaseComponent();
}

// Return the first actor with a physics body within player reach
bool UGrabObject::GetFirstPhysicsBodyInReach(FHitResult& Hit) const {

	// Ray-cast out to a certain distance
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	bool Success = GetWorld()->LineTraceSingleByObjectType(OUT Hit, GetPlayerLocation(), GetPlayerReach(),
	FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParams);

	// Check what object is hit
	if (Success) {
		AActor* ActorHit = Hit.GetActor();
		if (ActorHit) {
			UE_LOG(LogTemp, Warning, TEXT("Line Trace has hit %s"), *(ActorHit->GetName()));
		}
	}

	return Success;
}

// Called every frame
void UGrabObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!PhysicsHandle) { // Safety net to avoid potential errors/crashes if pointer returns null
		UE_LOG(LogTemp, Error, TEXT("No Physics Handle"));
		return;
	}
	if (PhysicsHandle->GrabbedComponent) { 
		PhysicsHandle->SetTargetLocation(GetPlayerReach());
		}
}

