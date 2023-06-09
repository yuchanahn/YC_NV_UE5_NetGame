#include "TestCharacter.h"

#include "TestAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

#include "Util.h"

// Sets default values
ATestCharacter::ATestCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATestCharacter::InitPlayerState()
{
	if (CustomPlayerState)
	{
		return;
	}

	CustomPlayerState = Cast<ACustomPlayerState>(GetPlayerState());

	USkeletalMeshComponent* SkeletalMesh = FindComponentByClass<USkeletalMeshComponent>();
	UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();
	UTestAnimInstance* TestAnimInstance = Cast<UTestAnimInstance>(AnimInstance);
	TestAnimInstance->CustomPlayerState = CustomPlayerState;
	TestAnimInstance->StartAnim = true;

	if (CustomPlayerState)
	{
		Util::LogDisplay("CustomPlayerState initialized");
	}
}

// Run on server
void ATestCharacter::OnConnect_Implementation()
{
	InitPlayerState();
}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		InitPlayerState();
	}
}

void ATestCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	Util::LogDisplay("OnRep_PlayerState");

	if (!CustomPlayerState)
	{
		// Server RPC
		if (!HasAuthority())
		{
			OnConnect();
		}

		InitPlayerState();
	}
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

