// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class Capstone_ToxicOasisTarget : TargetRules
{
	public Capstone_ToxicOasisTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "Capstone_ToxicOasis" } );
	}
}
