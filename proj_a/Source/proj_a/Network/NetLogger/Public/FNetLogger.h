#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogNetwork, Log, All);
extern DEFINE_LOG_CATEGORY(LogNetwork);

DECLARE_LOG_CATEGORY_EXTERN(Monster, Log, All);
extern DEFINE_LOG_CATEGORY(Monster);

class FNetLogger {
public:
	static void LogError(const TCHAR* format, ...) {
		va_list ArgPtr;
		va_start(ArgPtr, format);
		TCHAR buffer[256];
		FCString::GetVarArgs(buffer, sizeof(buffer), format, ArgPtr);
		va_end(ArgPtr);
		UE_LOG(LogNetwork, Error, TEXT("%s"), buffer);
	}

	static void LogWarning(const TCHAR* format, ...) {
		va_list ArgPtr;
		va_start(ArgPtr, format);
		TCHAR buffer[256];
		FCString::GetVarArgs(buffer, sizeof(buffer), format, ArgPtr);
		va_end(ArgPtr);
		UE_LOG(LogNetwork, Warning, TEXT("%s"), buffer);
	}

	static void LogInfo(const TCHAR* format, ...) {
		va_list ArgPtr;
		va_start(ArgPtr, format);
		TCHAR buffer[256];
		FCString::GetVarArgs(buffer, sizeof(buffer), format, ArgPtr);
		va_end(ArgPtr);
		UE_LOG(LogNetwork, Log, TEXT("%s"), buffer);
	}

	template<typename FmtType, typename... Args>
	static void EditerLog(FColor color, const FmtType& format, Args&&... args)
	{
		FString message = FString::Printf(format, std::forward<Args>(args)...);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, color, message);
		}
	}

private:
	FNetLogger() {}
};