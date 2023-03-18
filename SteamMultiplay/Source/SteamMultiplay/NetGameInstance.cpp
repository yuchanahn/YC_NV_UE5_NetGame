// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include "OnlineSubsystem.h"
#include "Util.h"

#include <format>


const static FName SESSION_NAME = NAME_GameSession; //TEXT("GameSession");


void UNetGameInstance::CreateSessionEv(const FName InName, bool bArg) {
	Util::LogDisplay(std::format("CreateSessionEv: {}, result : {}", InName, bArg));
}

void UNetGameInstance::CreateSession(const FString InServerName) {
	if (!Session.IsValid()) return;
	
	FOnlineSessionSettings SessionSettings;
	
	SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	SessionSettings.NumPublicConnections = 5;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	//SessionSettings.Set(SERVER_NAME_SETTING_KEY, InServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//스팀 초대 구현부
	SessionSettings.bAllowInvites = false;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bAllowJoinInProgress = true;

	Session->CreateSession(0, SESSION_NAME, SessionSettings);
}

void UNetGameInstance::AcceptedEv(bool bSuccess, int I, TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UniqueNetId,
	const FOnlineSessionSearchResult& OnlineSessionSearchResult) {
	if(OnlineSessionSearchResult.IsValid()) {
		Util::LogDisplay(std::format("AcceptedEv: {}, result : {}", I, bSuccess));
		Util::LogDisplay(std::format("Session Name {}", OnlineSessionSearchResult.Session.OwningUserName));
		Util::LogDisplay(std::format("Session MSessionName {}", SESSION_NAME.ToString()));
		Session->JoinSession(0, SESSION_NAME, OnlineSessionSearchResult);
	} else {
		Util::LogDisplay(std::format("AcceptedEv: {}, result : {}", I, bSuccess));
	}
}

void UNetGameInstance::JoinSessionEv(FName Name, EOnJoinSessionCompleteResult::Type Arg) {
	if (!Session.IsValid()) return;

	FString Address;
	if (!Session->GetResolvedConnectString(Name, Address))
	{
		Util::LogDisplay("Could not get connect string.");
		return;
	}

	Util::LogDisplay(std::format("Joining {}", Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) {
		Util::LogDisplay("PlayerController is nullptr");
		return;
	}

	PlayerController->ClientTravel(Address, TRAVEL_Absolute);
}

void UNetGameInstance::Init() {
	Super::Init();

	const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get("Steam");
	if (Subsystem != nullptr)
	{
		Util::LogDisplay("Subsystem Start!");
		Session = Subsystem->GetSessionInterface();
		if (Session.IsValid())
		{
			Session->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::CreateSessionEv);
			//SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &OnDestroySessionComplete);
			//SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &OnFindSessionComplete);
			Session->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::JoinSessionEv);
			//SessionInterface->OnSessionInviteReceivedDelegates.AddUObject(this, &OnSessionInviteReceived);
			Session->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UNetGameInstance::AcceptedEv);
		}
	}
	else
	{
		Util::LogDisplay(L"Steam subsystem not found!\n스팀을 켜라!");
	}
	
	if (GEngine != nullptr)
	{
		//GEngine->OnNetworkFailure().AddUObject(this, &UPuzzlePlatformsGameInstance::OnNetworkFailure);
	}
}
