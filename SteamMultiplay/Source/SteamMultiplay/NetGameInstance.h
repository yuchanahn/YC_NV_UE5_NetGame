// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "OnlineSubsystemUtils.h"

#include "NetGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class STEAMMULTIPLAY_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	IOnlineSessionPtr SessionInterface;
};
