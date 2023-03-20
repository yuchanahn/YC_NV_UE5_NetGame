// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SteamMultiplay : ModuleRules
{
	public SteamMultiplay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"HeadMountedDisplay",
			"UMG",

			"OnlineSubsystem",
			"OnlineSubsystemNull",
			"OnlineSubsystemSteam",
			"OnlineSubsystemEOS",
			"OnlineSubsystemEOSPlus",
			"Steamworks",
		});

		CppStandard = CppStandardVersion.Latest;
	}
}