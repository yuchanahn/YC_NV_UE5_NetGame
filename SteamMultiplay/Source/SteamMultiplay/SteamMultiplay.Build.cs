// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SteamMultiplay : ModuleRules
{
	public SteamMultiplay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new string[] {
			"OnlineSubsystem",
			"OnlineSubsystemNull",
			"OnlineSubsystemSteam",
			"OnlineSubsystemEOS",
			"OnlineSubsystemEOSPlus",
			"Steamworks",
		});
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay", 
			"EnhancedInput", 
			"UMG", 
		});
		
		CppStandard = CppStandardVersion.Latest;
	}
}
