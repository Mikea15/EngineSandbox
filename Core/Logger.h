
#include <windows.h>

#pragma once

enum class LEVEL : int
{
	NORMAL = 0,
	INFO,
	WARNING,
	ERR,
	FATAL
};

#define LOG(text, __VA_ARGS__)			Logger::Log(LEVEL::NORMAL,	text, __VA_ARGS__);
#define LOG_INFO(text, __VA_ARGS__)		Logger::Log(LEVEL::INFO,	text, __VA_ARGS__);
#define LOG_WARNING(text, __VA_ARGS__)	Logger::Log(LEVEL::WARNING, text, __VA_ARGS__);
#define LOG_ERROR(text, __VA_ARGS__)	Logger::Log(LEVEL::ERR,		text, __VA_ARGS__);
#define LOG_FATAL(text, __VA_ARGS__)	Logger::Log(LEVEL::FATAL,	text, __VA_ARGS__);

class Logger
{
private:
	Logger() = delete;

public:
	static void Log(LEVEL level, const char* szFormat, ...)
	{
		char szBuff[1024];
		va_list arg;
		va_start(arg, szFormat);
		_vsnprintf(szBuff, sizeof(szBuff), szFormat, arg);
		va_end(arg);

		char warning[80];
		switch (level)
		{
		case LEVEL::INFO: 
			strcpy(warning, "[Info] "); 
			break;
		case LEVEL::WARNING: 
			strcpy(warning, "[Warning] "); 
			break;
		case LEVEL::ERR: 
			strcpy(warning, "[Error] "); 
			break;
		case LEVEL::FATAL: 
			strcpy(warning, "[Fatal Error] "); 
			break;
		case LEVEL::NORMAL:
		default: strcpy(warning, "[Log] ");
			break;
		}

		char finalBuffer[1024];
		strcpy(finalBuffer, warning);
		strcat(finalBuffer, szBuff);
		strcat(finalBuffer, "\n");

		OutputDebugString(finalBuffer);
	}
};