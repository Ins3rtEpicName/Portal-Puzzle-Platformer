using UnrealBuildTool;

// This module must be loaded "PreLoadingScreen" in the .uproject file,
// otherwise it will not hook in time!

public class LoadingScreenModule : ModuleRules
{
	public LoadingScreenModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Public/LoadingScreenModule.h";

		PCHUsage = PCHUsageMode.UseSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new string[]{
				"Core",
				"CoreUObject",
				"MoviePlayer", "Slate", "SlateCore",
			}
		);
	}
}