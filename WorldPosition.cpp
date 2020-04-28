// Copyright Hannah Fisher, University of Brighton, 2020


#include "WorldPosition.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UWorldPosition::UWorldPosition()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWorldPosition::BeginPlay()
{
	Super::BeginPlay();

	FString ObjName = GetOwner()->GetName(); // Access object's name, pointers and referencing
	FString ObjPosition = GetOwner()->GetActorLocation().ToString(); // Access object's position in the world
	UE_LOG(LogTemp, Warning, TEXT("%s's position in world is: %s"), *ObjName, *ObjPosition); // Prints object's name and position to the log

}


// Called every frame
void UWorldPosition::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

