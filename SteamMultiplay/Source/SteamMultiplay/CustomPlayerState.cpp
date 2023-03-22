#include "CustomPlayerState.h"

#include "Net/UnrealNetwork.h"

#include "Util.h"


void ACustomPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACustomPlayerState, bDoAttack);
}

void ACustomPlayerState::OnDoAttackChange()
{
	Util::LogDisplay(std::format("bDoAttack changed : {}", bDoAttack));
}
