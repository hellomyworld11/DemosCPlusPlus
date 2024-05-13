#ifndef LOGGERX_H_
#define LOGGERX_H_
#include <map>
#include <string>

#include<fstream>





namespace moduleX {

	using namespace std;

enum class LogLevel{
	Error,
	Warn,
	Info,
	Debug,
	Trace
};

class LoggerConfig{
	typedef std::map<std::string, std::string> ConfigData;

	bool readConfig();
private:
	ConfigData _configMap;
};


class FileManage{
public:

	bool createFile(std::string filename);

	long getCurrentTime();
};


class LoggerX
{
public:
	LoggerX()=default;

	~LoggerX()=default;

	static LoggerX* instance();
	
public:
	void writeLog(LogLevel level, const char *format, ...);

};





}


#define LOG_DEBUG(format, ...) do{moduleX::LoggerX::instance()->write_log(0, format, ##__VA_ARGS__); moduleX::Log::get_instance()->flush();}while(0)



#endif // LOGGERX_H_