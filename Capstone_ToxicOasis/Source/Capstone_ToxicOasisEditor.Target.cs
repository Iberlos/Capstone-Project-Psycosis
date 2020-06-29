// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class Capstone_ToxicOasisEditorTarget : TargetRules
{
	public Capstone_ToxicOasisEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "Capstone_ToxicOasis" } );
	}
}
