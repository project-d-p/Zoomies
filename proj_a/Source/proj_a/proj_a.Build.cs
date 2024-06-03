// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System.Diagnostics;
using System;

public class proj_a : ModuleRules
{
	public proj_a(ReadOnlyTargetRules Target) : base(Target)
	{
		string projectDir = Path.Combine(ModuleDirectory, "../../");
		string thirdPartyDir = Path.Combine(projectDir, "ThirdParty");
		
		if (string.IsNullOrEmpty(thirdPartyDir))
		{
			throw new Exception("No Protobuf Error" + thirdPartyDir + ".");
		}
		
		SetProtobuf(thirdPartyDir);
		
		// XXX: 배포시에 컴파일 코드 삭제(혹은 주석 처리)
		// string protocPath = "";
		// if (Target.Platform == UnrealTargetPlatform.Win64)
		// {
		// 	protocPath = Path.Combine(protobufPath, "bin", "protoc.exe");
		// }
		// else if (Target.Platform == UnrealTargetPlatform.Mac)
		// {
		// 	protocPath = Path.Combine(protobufPath, "bin", "protoc");
		// }
		// string protoFilesPath = Path.Combine(ModuleDirectory, "Protobuf", "Proto_file");
		// string generatedProtoFilesPath = Path.Combine(ModuleDirectory, "Protobuf", "Pb_File");
		//
		// System.Console.WriteLine("Compiling .proto files...");
		// CompileProtoFiles(protocPath, protoFilesPath, generatedProtoFilesPath);
		//
		
		PublicIncludePaths.AddRange(new string[] {
			"proj_a/GameMode/Public",
			"proj_a/Actor/Controller/Public",
			"proj_a/Actor/Character/Public",
			"proj_a/Component/Public",
            "proj_a/Widget/Public",
            "proj_a/DataHub/Public",
			"proj_a/Network/Public",
			"proj_a/Network/NetLogger/Public",
			"proj_a/Network/ReceiveTask/Public",
			"proj_a/Network/SendTask/Public",
			"proj_a/Network/NetLogger/Public",
			"proj_a/GameInstance/Public",
			"proj_a/SteamGameManager/Public",
			"proj_a/ServerNetworkIO/Public",
			"proj_a/Lobby/Public",
			"proj_a/Core/Public",
			"proj_a/Utility/Public",
			"proj_a/Protobuf/Pb_File",
		});
		
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PublicDependencyModuleNames.AddRange(new string[]
			{ "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Sockets", "Networking", "OnlineSubsystemSteam", "OnlineSubsystem", "OnlineSubsystemUtils" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });

		string SteamSDKPath = Path.Combine(ModuleDirectory, "Steam");
		if (Directory.Exists(SteamSDKPath))
		{
			PublicIncludePaths.Add(Path.Combine(SteamSDKPath, "public"));
			if (Target.Platform == UnrealTargetPlatform.Win64)
			{
				PublicAdditionalLibraries.Add(Path.Combine(SteamSDKPath, "redistributable_bin", "win64", "steam_api64.lib"));
				RuntimeDependencies.Add("$(ProjectDir)/Binaries/Win64/steam_api64.dll", Path.Combine(SteamSDKPath, "redistributable_bin", "win64", "steam_api64.dll"));
			}
			else if (Target.Platform == UnrealTargetPlatform.Mac)
			{
				PublicAdditionalLibraries.Add(Path.Combine(SteamSDKPath, "redistributable_bin", "osx", "libsteam_api.dylib"));
				RuntimeDependencies.Add("$(ProjectDir)/Binaries/Mac/libsteam_api.dylib", Path.Combine(SteamSDKPath,"redistributable_bin", "osx", "libsteam_api.dylib"));
			}
		}
		//System.Console.WriteLine($"SteamSDKPath: {SteamSDKPath}");
		//System.Console.WriteLine("Steam SDK path exists");
		//System.Console.WriteLine(Path.Combine(SteamSDKPath, "public"));
		//System.Console.WriteLine(Path.Combine(SteamSDKPath, "redistributable_bin", "win64", "steam_api64.lib"));
		//System.Console.WriteLine($"ProjectDir: {ProjectDir}");
        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }

	private void SetProtobuf(string thirdPartyDir)
	{
		string protobufPath = Path.Combine(thirdPartyDir, "Protobuf");
		string abseilPath = Path.Combine(thirdPartyDir, "abseil-cpp");
		PublicIncludePaths.Add(Path.Combine(protobufPath, "include"));
		PublicIncludePaths.Add(Path.Combine(abseilPath, "include"));
		
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Abseil
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Win64", "absl_base.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Win64", "absl_city.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Win64", "absl_hash.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Win64", "absl_low_level_hash.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Win64", "absl_raw_hash.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Win64", "absl_raw_logging_internal.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Win64", "absl_throw_delegate.lib"));
			
			// ProtoBuf
			PublicAdditionalLibraries.Add(Path.Combine(protobufPath, "lib", "Win64", "libprotoc.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(protobufPath, "lib", "Win64", "libprotobuf.lib"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			// Abseil
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Mac", "libabsl_base.a"));
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Mac", "libabsl_city.a"));
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Mac", "libabsl_hash.a"));
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Mac", "libabsl_low_level_hash.a"));
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Mac", "libabsl_raw_hash.a"));
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Mac", "libabsl_raw_logging_internal.a"));
			PublicAdditionalLibraries.Add(Path.Combine(abseilPath, "lib", "Mac", "libabsl_throw_delegate.a"));

			// ProtoBuf
			PublicAdditionalLibraries.Add(Path.Combine(protobufPath, "lib", "Mac", "libprotoc.a"));
			PublicAdditionalLibraries.Add(Path.Combine(protobufPath, "lib", "Mac", "libprotobuf.a"));
		}
		PublicDefinitions.Add("GOOGLE_PROTOBUF_INTERNAL_DONATE_STEAL_INLINE");
		PublicDefinitions.Add("PROTOBUF_ENABLE_DEBUG_LOGGING_MAY_LEAK_PII=0");
	}
	
	// private void CompileProtoFiles(string protocPath, string inputPath, string outputPath)
	// {
	// 	var protoFiles = Directory.GetFiles(inputPath, "*.proto");
	// 	foreach (var protoFile in protoFiles)
	// 	{
	// 		var process = new Process();
	// 		process.StartInfo.FileName = protocPath;
	// 		process.StartInfo.Arguments = $"--proto_path={inputPath} --cpp_out={outputPath} {protoFile}";
	// 		process.StartInfo.UseShellExecute = false;
	// 		process.StartInfo.RedirectStandardOutput = true;
	// 		process.StartInfo.RedirectStandardError = true;
	// 		process.Start();
	// 		process.WaitForExit();
	//
	// 		string output = process.StandardOutput.ReadToEnd();
	// 		string errors = process.StandardError.ReadToEnd();
	// 		if (!string.IsNullOrEmpty(errors) || process.ExitCode != 0)
	// 		{
	// 			throw new Exception("Error compiling " + protoFile + ": " + errors);
	// 		}
	// 		System.Console.WriteLine(output);
	// 	}
	// }
}