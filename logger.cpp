#include <QCoreApplication>
#include <ctime>
#include "logger.hpp"
#include "main.hpp"

unsigned char Logger::LogLevel=LOG_DEBUG;
bool Logger::LogFileEnabled=true;
bool Logger::pIsBusy=false;

Logger *gAppLogger;

const char mgstypestring[8][16]=
{
	"EMERGENCY",
	"ALERT",
	"CRITICAL",
	"ERROR",
	"WARNING",
	"NOTICE",
	"INFO",
	"DEBUG"
};

Logger::Logger()
{
	pLogFilePath=string(PROGRAM_NAME ".log");
	pLogFile=nullptr;
}

Logger::~Logger()
{
    if(pLogFile)
    {
        fclose(pLogFile);
    }
}

void Logger::SetLogFilePath(const char *newPath)
{
	string npath(newPath);
	if(npath.length())
	{
		SetLogFilePath(npath);
	}
}

void Logger::SetLogFilePath(string newPath)
{
	while(pIsBusy)
	{
		QCoreApplication::processEvents();
	}
	pIsBusy=true;
	if(newPath.size())
	{
		pLogFilePath=newPath;
	}
	pIsBusy=false;
}

void Logger::Log(const char *message, unsigned char msg_level)
{
	if(msg_level>LogLevel)
	{
		return;
	}
	if(strlen(message))
	{
		Log(string(message), msg_level);
	}
}

void Logger::Log(string message, unsigned char msg_level)
{
	if(msg_level>LogLevel)
	{
		return;
	}
	while(pIsBusy)
	{
		QCoreApplication::processEvents();
	}
	pIsBusy=true;
	time(&pRawTime);
	pTimeInfo=localtime(&pRawTime);
	sprintf(pLogDTstr, "%.2i.%.2i.%i | %.2i:%.2i:%.2i", pTimeInfo->tm_mday, 1+pTimeInfo->tm_mon, 1900+pTimeInfo->tm_year, pTimeInfo->tm_hour, pTimeInfo->tm_min, pTimeInfo->tm_sec);
	if(message.size())
	{
		fprintf(stdout, "%s [%s] %s\n", pLogDTstr, mgstypestring[msg_level], message.data());
		fflush(stdout);
		if(LogFileEnabled)
		{
			pLogFile=fopen(pLogFilePath.data(), "a");
			if(pLogFile)
			{
				fprintf(pLogFile, "%s [%s] %s\n", pLogDTstr, mgstypestring[msg_level], message.data());
				fflush(pLogFile);
				fclose(pLogFile);
			}
		}
	}
	pIsBusy=false;
}
