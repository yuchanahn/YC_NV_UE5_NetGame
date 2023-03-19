// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include "OnlineSubsystem.h"
#include "Util.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/OnlineExternalUIInterface.h"

#include <Steamworks/Steamv153/sdk/public/steam/steam_api.h>
#include <format>
#include <string>


const static FName SESSION_NAME = NAME_GameSession; //TEXT("GameSession");


void UNetGameInstance::Init()
{
	Super::Init();

	InitOnlineSubsystem();

	if (GEngine != nullptr)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UNetGameInstance::NetworkFailEv);
	}
}

bool UNetGameInstance::InitOnlineSubsystem()
{
	Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		CurrentSubsystemName = Subsystem->GetSubsystemName();

		Util::LogDisplay(std::format("Subsystem started : {}", CurrentSubsystemName));

		InitSessionInterfaceDelegates();

		Util::LogDisplay(Login() ? L"Login: Success" : L"Login: Failed");

		return true;
	}

	// TODO: 여기서 뭔가 더 처리
	Util::LogDisplay(L"Online Subsystem not found!");

	CurrentSubsystemName = "";
	return false;
}

void UNetGameInstance::InitSessionInterfaceDelegates()
{
	// FIXME: 여러번 등록됨
	if (!SessionInterface.IsValid())
	{
		Util::LogDisplay(L"SessionInterface is not valid!");
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::CreateSessionEv);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UNetGameInstance::DestroySessionEv);
			SessionInterface->OnEndSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::DestroySessionEv);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetGameInstance::FindSessionEv);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::JoinSessionEv);
			//SessionInterface->OnSessionInviteReceivedDelegates.AddUObject(this, &OnSessionInviteReceived);
			SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UNetGameInstance::AcceptedEv);
		}
	}
}

void UNetGameInstance::CreateSession(const FString InServerName)
{
	if (!SessionInterface.IsValid()) return;
	FOnlineSessionSettings SessionSettings;

	SessionSettings.bIsLANMatch = Subsystem->GetSubsystemName() == "NULL";
	SessionSettings.bIsDedicated = false;
	SessionSettings.NumPublicConnections = 5;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowInvites = false;
	// SessionSettings.bAllowJoinViaPresenceFriendsOnly = true; // 친구만 가능
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bAllowJoinInProgress = true;
	// SessionSettings.BuildUniqueId = 1;
	//SessionSettings.Set(SERVER_NAME_SETTING_KEY, InServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}

void UNetGameInstance::CreateSessionEv(const FName InName, bool bArg)
{
	Util::LogDisplay(std::format("CreateSessionEv: {}, result : {}", InName, bArg));

	UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/Maps/Level_Gameplay.Level_Gameplay"), true, TEXT("listen"));

	ExternalUI->ShowInviteUI(0, SESSION_NAME);
}

void UNetGameInstance::DestroySessionEv(FName Name, bool bArg)
{
	Util::LogDisplay(std::format("DestroySessionEv: {}, result : {}", Name, bArg));
}

void UNetGameInstance::AcceptedEv(bool bSuccess, int I, TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UniqueNetId,
                                  const FOnlineSessionSearchResult& OnlineSessionSearchResult)
{
	if (OnlineSessionSearchResult.IsValid())
	{
		Util::LogDisplay(std::format("AcceptedEv: {}, result : {}", I, bSuccess));
		Util::LogDisplay(std::format("Session Name {}", OnlineSessionSearchResult.Session.OwningUserName));
		Util::LogDisplay(std::format("Session MSessionName {}", SESSION_NAME.ToString()));

		SessionInterface->JoinSession(0, SESSION_NAME, OnlineSessionSearchResult);
	}
	else
	{
		Util::LogDisplay(std::format("AcceptedEv: {}, result : {}", I, bSuccess));
	}
}

void UNetGameInstance::JoinSessionEv(FName Name, EOnJoinSessionCompleteResult::Type Arg)
{
	if (!SessionInterface.IsValid()) return;

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(Name, Address))
	{
		Util::LogDisplay("Could not get connect string.");
		return;
	}

	Util::LogDisplay(std::format("Joining {}", Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr))
	{
		Util::LogDisplay("PlayerController is nullptr");
		return;
	}

	PlayerController->ClientTravel(Address, TRAVEL_Absolute);
}

void UNetGameInstance::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid()) return;

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	if (!SessionInterface->FindSessions(0, LastSessionSearch.ToSharedRef()))
	{
		Util::LogDisplay("Could not find sessions.");
	}
}

void UNetGameInstance::FindSessionEv(bool bArg)
{
	Util::LogDisplay("find sessions!");

	if (!bArg)
	{
		Util::LogDisplay("find sessions fail!");
		return;
	}
	Util::LogDisplay(std::format("Num : {}", LastSessionSearch->SearchResults.Num()));
	if (LastSessionSearch->SearchResults.Num() > 0)
	{
		if (SessionInterface->JoinSession(0, SESSION_NAME, LastSessionSearch->SearchResults[0]))
		{
			Util::LogDisplay(std::format("JoinSessionEv: {}, result : {}", SESSION_NAME, true));
		}
	}
	else
	{
		Util::LogDisplay("Could not find sessions.");
	}
}

bool UNetGameInstance::Login(FString EpicID)
{
	if (const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface())
	{
		const ELoginStatus::Type LoginStatus = Identity->GetLoginStatus(0);
		if (LoginStatus == ELoginStatus::Type::LoggedIn)
		{
			// FIXME: ???
			return true;
		}

		// TODO: 이것도 한번만 등록
		Identity->OnLoginCompleteDelegates->AddUObject(this, &UNetGameInstance::LoginCompleteEv);

		ISteamUser* User = SteamUser();
		if (User == nullptr)
		{
			return false;
		}

		const uint32 CrossPlatformID = User->GetSteamID().GetAccountID();

		FOnlineAccountCredentials Credentials;
		// Credentials.Type = FString("Developer");
		// Credentials.Id = FString("3.34.51.212:7788");
		// Credentials.Token = EpicID;
		Credentials.Type = FString("EXTERNAL");
		Credentials.Id = FString::FromInt(CrossPlatformID);

		if (Identity->Login(0, Credentials))
		{
			PostLogin();
			return true;
		}
	}

	return false;
}

void UNetGameInstance::PostLogin()
{
	ExternalUI = Subsystem->GetExternalUIInterface();
}

// Login Button Press Event
void UNetGameInstance::LoginStart(FString ID)
{
	if (CurrentSubsystemName == "" || CurrentSubsystemName == "NULL")
	{
		IOnlineSubsystem::ReloadDefaultSubsystem();
		SteamAPI_Init();
	}

	if (InitOnlineSubsystem() && CurrentSubsystemName != "NULL")
	{
		Util::LogDisplay(Login(ID) ? L"Login: Success" : L"Login: Failed");
	}
}

void UNetGameInstance::LoginCompleteEv(int I, bool bArg, const FUniqueNetId& UniqueNetId, const FString& String)
{
	Util::LogDisplay(std::format("Logged in! state : {}", bArg));
}

void UNetGameInstance::NetworkFailEv(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg,
                                     const FString& String)
{
	Util::LogDisplay(std::format("NetworkFail, result : {}", String));
}
