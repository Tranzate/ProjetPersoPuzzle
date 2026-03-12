using UnrealBuildTool;

public class ProjPersoPuzzle : ModuleRules
{
	public ProjPersoPuzzle(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore",
			"EnhancedInput", "UMG", "SlateCore", "AIModule",
			"Niagara", "RHI", "RenderCore", "ApplicationCore", "Slate"
		});
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {
				"UnrealEd",
				"Blutility",
				"EditorSubsystem"
			});
		}

		PublicIncludePaths.Add(ModuleDirectory);
	}
}