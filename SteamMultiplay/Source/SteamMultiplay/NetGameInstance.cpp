// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include "OnlineSubsystem.h"
#include "Util.h"

#include <format>


const static FName SESSION_NAME = NAME_GameSession; //TEXT("GameSession");
const static FName SERVER_NAME_SETTING_KEY = TEXT("ServerName");


void UNetGameInstance::CreateSessionEv(const FName InName, bool bArg) {
	std::string Name = TCHAR_TO_UTF8(*InName.ToString());
	Util::LogDisplay(std::format("CreateSessionEv: {}, result : {}", Name, bArg));
}

void UNetGameInstance::CreateSession(const FString InServerName) {
	if (!Session.IsValid()) return;
	
	FOnlineSessionSettings SessionSettings;
	
	SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	SessionSettings.NumPublicConnections = 5;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.Set(SERVER_NAME_SETTING_KEY, InServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//스팀 초대 구현부
	SessionSettings.bAllowInvites = false;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bAllowJoinInProgress = true;

	Session->CreateSession(0, SESSION_NAME, SessionSettings);
}

void UNetGameInstance::AcceptedEv(bool bArg, int I, TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UniqueNetId,
	const FOnlineSessionSearchResult& OnlineSessionSearchResult) {
	if(OnlineSessionSearchResult.IsValid()) {
		auto Name = TCHAR_TO_UTF8(*OnlineSessionSearchResult.Session.OwningUserName);
		Util::LogDisplay(std::format("AcceptedEv: {}, result : {}", I, bArg));
		Util::LogDisplay(std::format("Session Name {}", Name));
	} else {
		Util::LogDisplay(std::format("AcceptedEv: {}, result : {}", I, bArg));
	}
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
			//SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &OnJoinSessionComplete);
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
