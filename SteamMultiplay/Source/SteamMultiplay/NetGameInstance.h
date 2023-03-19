#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystemUtils.h"

#include "NetGameInstance.generated.h"


UCLASS()
class STEAMMULTIPLAY_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	bool InitOnlineSubsystem();
	void InitSessionInterfaceDelegates();

	UFUNCTION(BlueprintCallable, Category = "YCSteamMultiplay")
	void CreateSession(FString InServerName);
	void CreateSessionEv(FName InName, bool bArg);
	void DestroySessionEv(FName Name, bool bArg);
	void AcceptedEv(bool bSuccess, int I, TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UniqueNetId,
	                const FOnlineSessionSearchResult& OnlineSessionSearchResult);
	void JoinSessionEv(FName Name, EOnJoinSessionCompleteResult::Type Arg);

	UFUNCTION(BlueprintCallable, Category = "YCSteamMultiplay")
	void FindSessions(int32 MaxSearchResults);

	UFUNCTION(BlueprintCallable, Category = "YCSteamMultiplay")
	void LoginStart(FString ID);

	bool Login(FString EpicID = "");
	void PostLogin();

	void FindSessionEv(bool bArg);
	void LoginCompleteEv(int I, bool bArg, const FUniqueNetId& UniqueNetId, const FString& String);
	void NetworkFailEv(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg, const FString& String);
	virtual void Init() override;

	IOnlineSubsystem* Subsystem;
	FName CurrentSubsystemName = "";
	IOnlineSessionPtr SessionInterface;
	IOnlineExternalUIPtr ExternalUI;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
};
