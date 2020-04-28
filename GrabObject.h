// Copyright Hannah Fisher, University of Brighton, 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GrabObject.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CASTLETHIEF_API UGrabObject : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabObject();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	float PlayerReachDistance = 150.f;
		
	void FindPhysicsHandle();
	void SetUpInputComponent();
	void Grab();
	void ReleaseGrab();

	// Return the first actor with a physics body within player reach
	FHitResult GetFirstPhysicsBodyInReach() const;

	FVector GetPlayerLocation() const;
	FRotator GetPlayerRotation() const;
	FVector GetPlayerReach() const;
	
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UInputComponent* InputComponent = nullptr;
};
