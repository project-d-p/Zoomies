// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;
using System.Diagnostics;

public class proj_a : ModuleRules
{
	public proj_a(ReadOnlyTargetRules Target) : base(Target)
	{
		string projectDir = Path.Combine(ModuleDirectory, "../../");
		string thirdPartyDir = Path.Combine(projectDir, "ThirdParty");
		string editorDirPath = Environment.GetEnvironmentVariable("UE5_ROOT");
		
		if (string.IsNullOrEmpty(editorDirPath))
		{
			throw new Exception("No UE5_ROOT Error" + editorDirPath + ".");
		}

		editorDirPath = Path.Combine(editorDirPath, "steam_appid.txt");
		string sourceFile = Path.Combine(projectDir, "steam_appid.txt");
		File.Copy(sourceFile, editorDirPath, overwrite: true);
			
		if (string.IsNullOrEmpty(thirdPartyDir))
		{
			throw new Exception("No Protobuf Error" + thirdPartyDir + ".");
		}
		
		SetProtobuf(thirdPartyDir);
		
		// XXX: 배포시에 컴파일 코드 삭제(혹은 주석 처리)
		string protocPath = "";
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			protocPath = Path.Combine(thirdPartyDir, "Win64", "Protobuf", "tools", "protobuf", "protoc.exe");
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			protocPath = Path.Combine(thirdPartyDir, "Mac", "Protobuf", "bin", "protoc");
		}
		string protoFilesPath = Path.Combine(ModuleDirectory, "Protobuf", "Proto_file");
		string generatedProtoFilesPath = Path.Combine(ModuleDirectory, "Protobuf", "Pb_File");
		
		System.Console.WriteLine("Compiling .proto files...");
		CompileProtoFiles(protocPath, protoFilesPath, generatedProtoFilesPath);
		// XXX: 배포시에 컴파일 코드 삭제(혹은 주석 처리)
		
		PublicIncludePaths.AddRange(new string[] {
			"proj_a/GameMode/Public",
			"proj_a/GameState/Public",
			"proj_a/Actor/Controller/Public",
			"proj_a/Actor/Character/Public",
			"proj_a/Actor/Character/CharacterPositionSync/Public",
			"proj_a/Actor/Character/PlayerState/Public",
			"proj_a/Actor/Monster/Character/Public",
			"proj_a/Actor/Monster/Controller/Public",
			"proj_a/Actor/Monster/PlayerState/Public",
			"proj_a/Actor/Monster/Factory/Public",
			"proj_a/Actor/Lobby/Public",
			"proj_a/Component/Public",
			"proj_a/Component/AnimNotify/Public",
			"proj_a/Component/InGame/Score/Public",
			"proj_a/Component/InGame/Chat/Public",
			"proj_a/Component/InGame/HitDetection/Public",
			"proj_a/Component/InGame/Timer/Public",
			"proj_a/Component/InGame/ReturnPlace/Public",
			"proj_a/Component/ClientNetwork/Public",
            "proj_a/Widget/Public",
            "proj_a/Widget/InGame/Public",
            "proj_a/DataHub/Public",
			"proj_a/Network/Public",
			"proj_a/Network/NetLogger/Public",
			"proj_a/Network/ReceiveTask/Public",
			"proj_a/Network/SendTask/Public",
			"proj_a/GameInstance/Public",
			"proj_a/SteamGameManager/Public",
			"proj_a/ServerNetworkIO/Public",
			"proj_a/ServerMessageHandler/Public",
			"proj_a/Lobby/Public",
			"proj_a/Core/Public",
			"proj_a/Utility/Public",
			"proj_a/Protobuf/Pb_File",
		});
		
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core",
			"CoreUObject",
			"Engine", 
			"InputCore",
			"EnhancedInput",
			"Sockets",
			"Networking",
			"UMG",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"OnlineSubsystemSteam",
			"SlateCore",
			"AIModule",
			"NavigationSystem",
		});

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
    }

	private void SetProtobuf(string thirdPartyDir)
	{
		string wind64Path = Path.Combine(thirdPartyDir, "Win64");
		string macPath = Path.Combine(thirdPartyDir, "Mac");
		string protobufPath = "";
		string abseilPath = "";
		string grpcPath = "";
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			protobufPath = Path.Combine(wind64Path, "Protobuf");
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			protobufPath = Path.Combine(macPath, "Protobuf");
			abseilPath = Path.Combine(macPath, "abseil-cpp");
			grpcPath = Path.Combine(macPath, "grpc");
		}

		if (Target.Platform == UnrealTargetPlatform.Mac
			&& protobufPath != ""
			&& abseilPath != ""
			&& grpcPath != "")
		{
			PublicIncludePaths.Add(Path.Combine(protobufPath, "include"));
			PublicIncludePaths.Add(Path.Combine(abseilPath, "include"));
			PublicIncludePaths.Add(Path.Combine(grpcPath, "include"));
			AddAllLibrariesFromPath(Path.Combine(protobufPath, "lib"));
			AddAllLibrariesFromPath(Path.Combine(grpcPath, "lib"));
			AddAllLibrariesFromPath(Path.Combine(abseilPath, "lib"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Win64
				&& protobufPath != "")
		{
			PublicIncludePaths.Add(Path.Combine(protobufPath, "include"));
			AddAllLibrariesFromPath(Path.Combine(protobufPath, "lib"));
		}
		else
		{
			throw new Exception("No Protobuf Error" + thirdPartyDir + ".");
		}
		PublicDefinitions.Add("GOOGLE_PROTOBUF_INTERNAL_DONATE_STEAL_INLINE");
		PublicDefinitions.Add("PROTOBUF_ENABLE_DEBUG_LOGGING_MAY_LEAK_PII=0");
	}

    private void AddAllLibrariesFromPath(string path)
    {
        if (Directory.Exists(path))
        {
            string[] files = Directory.GetFiles(path, "*.*", SearchOption.AllDirectories);
            foreach (string file in files)
            {
				PublicAdditionalLibraries.Add(file);
            }
        }
        else
        {
            System.Console.WriteLine($"Directory {path} does not exist.");
        }
    }	

	private void CompileProtoFiles(string protocPath, string inputPath, string outputPath)
	{
		var protoFiles = Directory.GetFiles(inputPath, "*.proto");
		foreach (var protoFile in protoFiles)
		{
			var process = new Process();
			process.StartInfo.FileName = protocPath;
			process.StartInfo.Arguments = $"--proto_path={inputPath} --cpp_out={outputPath} {protoFile}";
			process.StartInfo.UseShellExecute = false;
			process.StartInfo.RedirectStandardOutput = true;
			process.StartInfo.RedirectStandardError = true;
			process.Start();
			process.WaitForExit();
	
			string output = process.StandardOutput.ReadToEnd();
			string errors = process.StandardError.ReadToEnd();
			if (!string.IsNullOrEmpty(errors) || process.ExitCode != 0)
			{
				throw new Exception("Error compiling " + protoFile + ": " + errors);
			}
			System.Console.WriteLine(output);
		}
	}
}