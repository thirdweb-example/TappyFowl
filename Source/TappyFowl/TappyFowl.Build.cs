// Copyright (c) 2024 Thirdweb. All Rights Reserved.

using UnrealBuildTool;

// ReSharper disable UseCollectionExpression

public class TappyFowl : ModuleRules
{
	public TappyFowl(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore", 
			"Thirdweb",
			"ModelViewViewModel",
			"Json",
			"HTTP",
			"Paper2D",
			"DeveloperSettings"
		});

		// PrivateDependencyModuleNames.Add("Thirdweb");
		
	}
}