// Copyright Hannah Fisher, University of Brighton, 2020

#include "GrabObject.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UGrabObject::UGrabObject()
{
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
	if (!PhysicsHandle) {
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

// Function to pick up object
void UGrabObject::Grab() {
	UE_LOG(LogTemp, Warning, TEXT("Grabber pressed"));

	FVector ViewpointLocation;
	FRotator ViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT ViewpointLocation, OUT ViewpointRotation);

	FVector LineTraceEnd = ViewpointLocation + ViewpointRotation.Vector() * PlayerReach;

	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	// If something is hit, attach physics handle
	if (HitResult.GetActor()) {
		PhysicsHandle->GrabComponentAtLocationWithRotation(ComponentToGrab, NAME_None, LineTraceEnd, ViewpointRotation);
	}
}

// Function to release object
void UGrabObject::ReleaseGrab() {
	UE_LOG(LogTemp, Warning, TEXT("Grab released"));
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector ViewpointLocation;
	FRotator ViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT ViewpointLocation, OUT ViewpointRotation);

	FVector LineTraceEnd = ViewpointLocation + ViewpointRotation.Vector() * PlayerReach;

	if (PhysicsHandle->GrabbedComponent) { 
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
		}
}

// Return the first actor with a physics body within player reach
FHitResult UGrabObject::GetFirstPhysicsBodyInReach() const {
	FVector ViewpointLocation;
	FRotator ViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT ViewpointLocation, OUT ViewpointRotation);

	FVector LineTraceEnd = ViewpointLocation + ViewpointRotation.Vector() * PlayerReach;

	// Ray-cast out to a certain distance
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(OUT Hit, ViewpointLocation, LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParams);

	// Check what object is hit
	AActor* ActorHit = Hit.GetActor();
	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Line Trace has hit %s"), *(ActorHit->GetName()));
	}

	return Hit;
}

/*FVector UGrabObject::GetLineTraceEnd() {
	FVector ViewpointLocation;
	FRotator ViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT ViewpointLocation, OUT ViewpointRotation);

	FVector LineTraceEnd = ViewpointLocation + ViewpointRotation.Vector() * PlayerReach;

	return LineTraceEnd;
}*/

