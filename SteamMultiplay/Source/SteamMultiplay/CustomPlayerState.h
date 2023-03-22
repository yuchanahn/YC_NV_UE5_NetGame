#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CustomPlayerState.generated.h"


UCLASS()
class STEAMMULTIPLAY_API ACustomPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY(ReplicatedUsing = OnDoAttackChange, BlueprintReadWrite)
	bool bDoAttack = false;

	UFUNCTION()
	void OnDoAttackChange();
};
