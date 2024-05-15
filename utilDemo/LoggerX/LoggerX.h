#ifndef LOGGERX_H_
#define LOGGERX_H_
#include <map>
#include <string>
#include <iostream>
#include<fstream>
#include <sstream>
#include <ctime>
#ifdef __linux__
#include <unistd.h>
#include <pwd.h>
#endif

#ifdef _WIN32
#include<Windows.h>
#endif

#ifdef __linux__
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/stat.h>
#include<sys/types.h>
#elif _WIN32
#include<io.h>
#include<direct.h>
#endif

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#define __NAME__(name) #name
#define MAX_BUFF_SIZE 512
#define SIZE_64 64
#define SIZE_128 128

namespace moduleX{

	using namespace std;
	
	const std::string  BRACKETS_LEFT = " [";
	const std::string  BRACKETS_RIGHT = "] ";
	const std::string SPACE = " ";
	const std::string COLON = ":";
	const std::string SLASH = "/";
	const int MEGABYTES = 1048576;

enum class LogLevel{
	Error,
	Warn,
	Info,
	Debug,
	Trace
};

class LoggerConfig{
	typedef std::map<std::string, std::string> ConfigData;
public:
	void initConfig();

	bool readConfig();

	string operator[](string key) const;
private:
	ConfigData		_configMap;
	const string	_configfile = "./logconf.conf";
};


class FileManage{
public:
	bool createFilePath(string path);

	bool createFile(string filename);

	long getCurrentTime();

	long getFileSize(string filename);

	bool isFileExist(string fileName);

	bool fileRename(string oldname, string newname);
};


class LoggerX
{
public:
	LoggerX();

	~LoggerX();

	static LoggerX* instance();
	
	static void asyncLog(LoggerX* logger);
public:
	bool init();

	void writeLog(LogLevel level, const char *format, ...);

	void writeToTerminal(LogLevel level, const char *format, va_list args);

	void writeToFile(LogLevel level, const char *format, va_list args);

	string getLogCoutTime(bool bBracket = true);

	string getUserName();

	string getLogCoutThreadId();

	string getFilefullname();

	string getFilePath();

	string getFilefullnameWithTime();

	bool addLog(string messages);

	LoggerConfig _config;

	FileManage _filemgr;

	mutex _lock_file;
	mutex _lock_queue;

	condition_variable _cond;

	ofstream _file;

	queue<string> _logQueue;

	thread::id	_threadId;

	bool	_bStart = false;
};



}


#define LOG_DEBUG(format, ...) do{moduleX::LoggerX::instance()->writeLog(LogLevel::Debug, format, ##__VA_ARGS__);}while(0)
#define LOG_INFO(format, ...) do{moduleX::LoggerX::instance()->writeLog(LogLevel::Info, format, ##__VA_ARGS__);}while(0)
#define LOG_WARN(format, ...) do{moduleX::LoggerX::instance()->writeLog(LogLevel::Warn, format, ##__VA_ARGS__);}while(0)
#define LOG_ERROR(format, ...) do{moduleX::LoggerX::instance()->writeLog(LogLevel::Error, format, ##__VA_ARGS__);}while(0)
#define LOG_TRACE(format, ...) do{moduleX::LoggerX::instance()->writeLog(LogLevel::Trace, format, ##__VA_ARGS__);}while(0)

#endif // LOGGERX_H_