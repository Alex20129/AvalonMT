#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

#if defined(Q_OS_WIN)
#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */
#else
#include <sys/syslog.h>
#endif

using namespace std;

class Logger
{
public:
    Logger();
    ~Logger();
    void SetLogFilePath(const char *newPath);
	void SetLogFilePath(string newPath);
	void Log(const char *message, unsigned char msg_level);
	void Log(string message, unsigned char msg_level);
	static unsigned char LogLevel;
    static bool LogFileEnabled;
private:
	string pLogFilePath;
	char pLogDTstr[256];
	time_t pRawTime;
	tm *pTimeInfo;
    FILE *pLogFile;
	static bool pIsBusy;
};

extern Logger *gAppLogger;

#endif // LOGGER_HPP
