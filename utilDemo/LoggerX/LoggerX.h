#ifndef LOGGERX_H_
#define LOGGERX_H_
#include <map>
#include <string>

namespace moduleX {

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

};


class LoggerX
{
public:
	LoggerX()=delete;
	~LoggerX()=delete;

	static LoggerX* instance();
	

};







}






#endif // LOGGERX_H_