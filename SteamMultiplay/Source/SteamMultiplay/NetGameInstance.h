#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystemUtils.h"

#include "NetGameInstance.generated.h"


UCLASS()
class STEAMMULTIPLAY_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()

	bool InitOnlineSubsystem();
	void InitSessionInterfaceDelegates();

	bool Login(FString EpicID = "");
	void PostLogin();
	
	void CreateSessionEv(FName InName, bool bArg);
	void DestroySessionEv(FName Name, bool bArg);
	void AcceptedEv(bool bWasSuccessful, int I, TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UniqueNetId,
					const FOnlineSessionSearchResult& OnlineSessionSearchResult);
	void JoinSessionEv(FName Name, EOnJoinSessionCompleteResult::Type Arg);
	void FindSessionEv(bool bWasSuccessful);
	void LoginCompleteEv(int LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void NetworkFailureEv(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type ErrorType, const FString& Error);
	
protected:
	virtual void Init() override;

public:
	IOnlineSubsystem* Subsystem;
	FName CurrentSubsystemName = "";
	bool bIsLoggedIn = false;
	
	IOnlineSessionPtr SessionInterface;
	IOnlineExternalUIPtr ExternalUI;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	
	UFUNCTION(BlueprintCallable, Category = "YCSteamMultiplay")
	void CreateSession(FString InServerName);

	UFUNCTION(BlueprintCallable, Category = "YCSteamMultiplay")
	void FindSessions(int32 MaxSearchResults);

	UFUNCTION(BlueprintCallable, Category = "YCSteamMultiplay")
	void LoginStart(FString ID);
};
