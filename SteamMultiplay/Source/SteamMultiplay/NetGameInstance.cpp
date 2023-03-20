#include "NetGameInstance.h"

#include <string>
#include <format>

#include <Steamworks/Steamv153/sdk/public/steam/steam_api.h>

#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"

#include "Util.h"


// const static FName SESSION_NAME = NAME_GameSession; //TEXT("GameSession");
const static FName SESSION_NAME = TEXT("YC_Session");


void UNetGameInstance::Init()
{
	Super::Init();

	InitOnlineSubsystem();

	if (CurrentSubsystemName != "" && CurrentSubsystemName != "NULL")
	{
		Util::LogDisplay(Login() ? "Login: Success" : "Login: Failed");
	}

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UNetGameInstance::NetworkFailureEv);
	}
}

bool UNetGameInstance::InitOnlineSubsystem()
{
	Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		CurrentSubsystemName = Subsystem->GetSubsystemName();

		Util::LogDisplay(std::format("Subsystem started : {}", CurrentSubsystemName));

		InitSessionInterfaceDelegates();

		return true;
	}

	// TODO: 여기서 뭔가 더 처리
	Util::LogDisplay(L"Online Subsystem not found!");

	CurrentSubsystemName = "";
	return false;
}

void UNetGameInstance::InitSessionInterfaceDelegates()
{
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

	SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}

void UNetGameInstance::CreateSessionEv(const FName InName, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		Util::LogDisplay("CreateSession failed!");
		return;
	}

	Util::LogDisplay(std::format("CreateSession Name : {}", InName));

	// Change level
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/Maps/Level_Gameplay.Level_Gameplay"), true, TEXT("listen"));

	// Show invite UI
	if (bIsLoggedIn)
	{
		ExternalUI->ShowInviteUI(0, SESSION_NAME);
	}
}

void UNetGameInstance::DestroySessionEv(FName Name, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		Util::LogDisplay("DestroySession failed!");
		return;
	}

	Util::LogDisplay(std::format("DestroySession Name : {}", Name));
}

void UNetGameInstance::AcceptedEv(bool bWasSuccessful, int ControllerId,
                                  TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UserId,
                                  const FOnlineSessionSearchResult& InviteResult)
{
	if (!bWasSuccessful)
	{
		Util::LogDisplay("OnSessionUserInviteAccepted failed!");
		return;
	}

	if (InviteResult.IsValid())
	{
		Util::LogDisplay(std::format("Session Owning User Name : {}",
		                             InviteResult.Session.OwningUserName));

		SessionInterface->JoinSession(0, SESSION_NAME, InviteResult);
	}
	else
	{
		Util::LogDisplay("InviteResult is not valid!");
	}
}

void UNetGameInstance::JoinSessionEv(FName Name, EOnJoinSessionCompleteResult::Type JoinCompleteType)
{
	if (!SessionInterface.IsValid()) return;

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(Name, Address))
	{
		Util::LogDisplay("Failed to get Address");
		return;
	}

	Util::LogDisplay(std::format("Join complete Name : {}, Address : {}", Name, Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!PlayerController)
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

void UNetGameInstance::FindSessionEv(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		Util::LogDisplay("FindSession failed!");
		return;
	}

	Util::LogDisplay(std::format("Session search results count : {}", LastSessionSearch->SearchResults.Num()));

	if (LastSessionSearch->SearchResults.Num() > 0)
	{
		SessionInterface->JoinSession(0, SESSION_NAME, LastSessionSearch->SearchResults[0]);
	}
}

bool UNetGameInstance::Login(FString EpicGamesID)
{
	if (const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface())
	{
		// Check login status
		const ELoginStatus::Type LoginStatus = Identity->GetLoginStatus(0);
		if (LoginStatus == ELoginStatus::Type::LoggedIn)
		{
			Util::LogDisplay("Trying to login while already logged in");
			bIsLoggedIn = true;
			return true;
		}

		if (!Identity->OnLoginCompleteDelegates->IsBound())
		{
			Identity->OnLoginCompleteDelegates->AddUObject(this, &UNetGameInstance::LoginCompleteEv);
		}

		// Get Steam User
		ISteamUser* User = SteamUser();
		if (User == nullptr)
		{
			Util::LogDisplay("Unable to get SteamUser");
			bIsLoggedIn = false;
			return false;
		}

		// Get Steam User ID
		const uint32 CrossPlatformID = User->GetSteamID().GetAccountID();

		// Use Cross Platform ID (Steam) for EOSPlus
		FOnlineAccountCredentials Credentials;
		Credentials.Type = FString("External");
		Credentials.Id = FString::FromInt(CrossPlatformID);

		// Epic games auto login
		// Credentials.Type = FString("Developer");
		// Credentials.Id = FString("3.34.51.212:7788");
		// Credentials.Token = EpicID;

		// Try login
		if (Identity->Login(0, Credentials))
		{
			bIsLoggedIn = true;
			PostLogin();
			return true;
		}
	}

	bIsLoggedIn = false;
	return false;
}

void UNetGameInstance::PostLogin()
{
	ExternalUI = Subsystem->GetExternalUIInterface();
}

// Login Button Press Event
void UNetGameInstance::LoginStart(FString ID)
{
	if (CurrentSubsystemName != "" && CurrentSubsystemName != "NULL")
	{
		Util::LogDisplay(Login(ID) ? "Login: Success" : "Login: Failed");
	}
}

void UNetGameInstance::LoginCompleteEv(int LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
                                       const FString& Error)
{
	if (!bWasSuccessful)
	{
		Util::LogDisplay(std::format("Error: {}", Error));
		return;
	}

	Util::LogDisplay(std::format("Login Complete, UserID: {}", UserId.ToString()));
}

void UNetGameInstance::NetworkFailureEv(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type ErrorType,
                                        const FString& Error)
{
	Util::LogDisplay(std::format("Network Failure! [{}] : {}", FString(ToString(ErrorType)), Error));
}
