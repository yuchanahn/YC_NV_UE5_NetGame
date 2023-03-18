// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include "OnlineSubsystem.h"
#include "Util.h"

void UNetGameInstance::Init() {
	Super::Init();

	const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		Util::LogDisplay("Subsystem Start!");
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			//SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &OnCreateSessionComplete);
			//SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &OnDestroySessionComplete);
			//SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &OnFindSessionComplete);
			//SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &OnJoinSessionComplete);
			//SessionInterface->OnSessionInviteReceivedDelegates.AddUObject(this, &OnSessionInviteReceived);
			//SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &OnSessionUserInviteAccepted);
		}
	}
	else
	{
		Util::LogDisplay("Found No subsystem");
	}
	
	if (GEngine != nullptr)
	{
		//GEngine->OnNetworkFailure().AddUObject(this, &UPuzzlePlatformsGameInstance::OnNetworkFailure);
	}
}
