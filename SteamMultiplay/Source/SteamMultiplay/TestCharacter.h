#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CustomPlayerState.h"

#include "TestCharacter.generated.h"

UCLASS()
class STEAMMULTIPLAY_API ATestCharacter : public ACharacter
{
	GENERATED_BODY()
	
	void InitPlayerState();
	
public:
	// Sets default values for this character's properties
	ATestCharacter();

	UPROPERTY(BlueprintReadWrite)
	ACustomPlayerState* CustomPlayerState;

	UFUNCTION(Server, Reliable)
	void OnConnect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
