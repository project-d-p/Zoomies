#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogNetwork, Log, All);
extern DEFINE_LOG_CATEGORY(LogNetwork);

class FNetLogger {
public:
	static FNetLogger& GetInstance() {
		static FNetLogger instance;
		return instance;
	}
	
	void LogError(const TCHAR* format, ...) {
		va_list ArgPtr;
		va_start(ArgPtr, format);
		TCHAR buffer[256];
		vswprintf(buffer, sizeof(buffer), format, ArgPtr);
		va_end(ArgPtr);
		UE_LOG(LogNetwork, Error, TEXT("%s"), buffer);
	}

	void LogWarning(const TCHAR* format, ...) {
		va_list ArgPtr;
		va_start(ArgPtr, format);
		TCHAR buffer[256];
		vswprintf(buffer, sizeof(buffer), format, ArgPtr);
		va_end(ArgPtr);
		UE_LOG(LogNetwork, Warning, TEXT("%s"), buffer);
	}

	void LogInfo(const TCHAR* format, ...) {
		va_list ArgPtr;
		va_start(ArgPtr, format);
		TCHAR buffer[256];
		vswprintf(buffer, sizeof(buffer), format, ArgPtr);
		va_end(ArgPtr);
		UE_LOG(LogNetwork, Log, TEXT("%s"), buffer);
	}

	static void EditerLog(FColor color, const TCHAR* format, ...)
	{
		va_list ArgPtr;
		va_start(ArgPtr, format);
		TCHAR buffer[256];
		vswprintf(buffer, sizeof(buffer), format, ArgPtr);
		va_end(ArgPtr);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				color,
				FString::Printf(TEXT("%s"), buffer));
		}
	}

private:
	FNetLogger() {}
};