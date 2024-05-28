// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System.Diagnostics;
using System;

public class proj_a : ModuleRules
{
	public proj_a(ReadOnlyTargetRules Target) : base(Target)
	{
		// XXX: 배포시에 SetProtobuf() 메소드 내부로 이동.
		string protobufPath = System.Environment.GetEnvironmentVariable("Protobuf");
		
		if (string.IsNullOrEmpty(protobufPath))
		{
			throw new Exception("PROTOBUF_PATH environment variable is not set");
		}
		
		SetProtobuf(protobufPath);
		
		// XXX: 배포시에 컴파일 코드 삭제(혹은 주석 처리)
		string protocPath = Path.Combine(protobufPath, "tools", "protobuf", "protoc.exe");
		string protoFilesPath = Path.Combine(ModuleDirectory, "Protobuf", "Proto_file");
		string generatedProtoFilesPath = Path.Combine(ModuleDirectory, "Protobuf", "Pb_File");
		
		System.Console.WriteLine("Compiling .proto files...");
		CompileProtoFiles(protocPath, protoFilesPath, generatedProtoFilesPath);
		//
		
		PublicIncludePaths.AddRange(new string[] {
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
			"OnlineSubsystemSteam",
			"OnlineSubsystemUtils",
			"SlateCore",
		});
		
		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}

	private void SetProtobuf(string protobufPath)
	{
		PublicIncludePaths.Add(Path.Combine(protobufPath, "include"));
		PublicAdditionalLibraries.Add(Path.Combine(protobufPath, "lib", "libprotoc.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(protobufPath, "lib", "libprotobuf-lite.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(protobufPath, "lib", "libprotobuf.lib"));
		PublicDefinitions.Add("GOOGLE_PROTOBUF_INTERNAL_DONATE_STEAL_INLINE");
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