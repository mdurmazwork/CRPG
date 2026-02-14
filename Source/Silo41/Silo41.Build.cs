// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Silo41 : ModuleRules
{
    public Silo41(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "AIModule",
            "UMG" // [KRITÝK] UI (Widget) sýnýflarý için zorunludur.
		});

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",     // UI render motoru
			"SlateCore"  // UI çekirdek yapýlarý
		});

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}