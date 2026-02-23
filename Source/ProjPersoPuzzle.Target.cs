// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ProjPersoPuzzleTarget : TargetRules
{
	public ProjPersoPuzzleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		//IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ProjPersoPuzzle");
	}
}
