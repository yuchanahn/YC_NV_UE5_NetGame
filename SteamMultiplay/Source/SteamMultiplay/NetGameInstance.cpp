// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include "OnlineSubsystem.h"
#include "Util.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/OnlineExternalUIInterface.h"

#include <format>


const static FName SESSION_NAME = NAME_GameSession; //TEXT("GameSession");


void UNetGameInstance::CreateSessionEv(const FName InName, bool bArg) {
	Util::LogDisplay(std::format("CreateSessionEv: {}, result : {}", InName, bArg));

	//GetWorld()->ServerTravel()
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/Maps/Level_Gameplay.Level_Gameplay"), true, TEXT("listen"));

	ExternalUI->ShowInviteUI(0, SESSION_NAME);
}

void UNetGameInstance::CreateSession(const FString InServerName) {
	if (!Session.IsValid()) return;
	FOnlineSessionSettings SessionSettings;
	
	SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	SessionSettings.NumPublicConnections = 5;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.BuildUniqueId = 1;
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

void UNetGameInstance::DestroySessionEv(FName Name, bool bArg) {
	Util::LogDisplay(std::format("DestroySessionEv: {}, result : {}", Name, bArg));
}

void UNetGameInstance::FindSessions(int32 MaxSearchResults)
{
	if (!Session.IsValid())
	{
		return;
	}

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!Session->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		Util::LogDisplay("Could not find sessions.");
	}
}

void UNetGameInstance::FindSesstionEv(bool bArg) {
	Util::LogDisplay("find sessions!");
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	Session->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), SESSION_NAME, LastSessionSearch->SearchResults[0]);
}


void UNetGameInstance::LoginCompleteEv(int I, bool bArg, const FUniqueNetId& UniqueNetId, const FString& String)
{
	Util::LogDisplay(L"Logged in!");
	
	ExternalUI = Subsystem->GetExternalUIInterface();
	
	Session = Subsystem->GetSessionInterface();
	if (Session.IsValid())
	{
		Session->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::CreateSessionEv);
		Session->OnDestroySessionCompleteDelegates.AddUObject(this, &UNetGameInstance::DestroySessionEv);
		Session->OnEndSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::DestroySessionEv);
		Session->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetGameInstance::FindSesstionEv);
		Session->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::JoinSessionEv);
		//SessionInterface->OnSessionInviteReceivedDelegates.AddUObject(this, &OnSessionInviteReceived);
		Session->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UNetGameInstance::AcceptedEv);
	}
}

void UNetGameInstance::Init() {
	Super::Init();

	Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		Util::LogDisplay("Subsystem Start!");
		Util::LogDisplay(std::format("{}", Subsystem->GetSubsystemName()));

		// check login
		if (const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface())
		{
			Identity->OnLoginCompleteDelegates->AddUObject(this, &UNetGameInstance::LoginCompleteEv);
			
			FOnlineAccountCredentials Credentials;
			Credentials.Type = FString("AccountPortal");
			Credentials.Id = "";
			Credentials.Token = "";
			
			if (Identity->Login(0, Credentials))
			{
				Util::LogDisplay("Try Login");
			}
			else
			{
				Util::LogDisplay("Try Login Failed");
			}
		}
	}
	else
	{
		Util::LogDisplay(L"Online Subsystem not found!");
	}
	
	if (GEngine != nullptr)
	{
		//GEngine->OnNetworkFailure().AddUObject(this, &UPuzzlePlatformsGameInstance::OnNetworkFailure);
	}
}
