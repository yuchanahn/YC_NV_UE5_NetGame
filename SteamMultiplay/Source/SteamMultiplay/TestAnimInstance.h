#pragma once

#include "CoreMinimal.h"
#include "CustomPlayerState.h"
#include "Animation/AnimInstance.h"
#include "TestAnimInstance.generated.h"


UCLASS()
class STEAMMULTIPLAY_API UTestAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool StartAnim = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ACustomPlayerState* CustomPlayerState;
};
