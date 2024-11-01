// Copyright (c) 2024 Thirdweb. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TappyFowlTarget : TargetRules
{
	public TappyFowlTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			bUseLoggingInShipping = true;
		}
		ExtraModuleNames.Add("TappyFowl");
	}
}
