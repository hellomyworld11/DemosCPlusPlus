#include "LoggerX.h"
#include <stdarg.h>

using namespace moduleX;

//init static var
//TODO
LoggerX* moduleX::LoggerX::instance()
{
	static LoggerX instance;
	instance._config.readConfig();
	return &instance;
}

void moduleX::LoggerX::writeToTerminal(LogLevel level, const char *format, va_list args)
{
	string line = getLogCoutTime() + __NAME__(level) + getUserName() + getLogCoutThreadId() + BRACKETS_LEFT +
		__FILE__ + SPACE + __FUNCTION__ + COLON + std::to_string(__LINE__) + BRACKETS_RIGHT;

	char szBuff[MAX_BUFF_SIZE];
	memset(szBuff, 0, sizeof(szBuff));
	vsprintf(szBuff, format, args);

	std::cout << line << szBuff << "\n";
	fflush(stdout); 
}

void moduleX::LoggerX::writeToFile(LogLevel level, const char *format, va_list args)
{
	std::ostringstream oss; 
	std::streambuf* pOldBuf = std::cout.rdbuf(oss.rdbuf());

	char szBuff[MAX_BUFF_SIZE];
	memset(szBuff, 0, sizeof(szBuff));
	vsprintf(szBuff, format, args);

	std::cout << szBuff;
	std::string ret = oss.str();
	std::cout.rdbuf(pOldBuf);

	string line = getLogCoutTime() + __NAME__(level) + getUserName() + getLogCoutThreadId() + BRACKETS_LEFT +
		__FILE__ + SPACE + __FUNCTION__ + COLON + std::to_string(__LINE__) + BRACKETS_RIGHT;
	
	string fullname = getFilefullname();

	long fileSize = _filemgr.getFileSize(fullname);
	if (fileSize > (long)atoi(_config["logMixSize"].data()) * MEGABYTES && _config["logBehavior "] == "1") {
		string newFileName = getFilefullnameWithTime();
		_filemgr.fileRename(fullname, newFileName);
		_filemgr.createFile(fullname);
	}
	if (_config["logFileQueueSwitch"] == "off") {
		mutex_file->lock();
		ofstream file;
		file.open(filePashAndName, ::ios::app | ios::out);
		file << messages << message << line_effd;
		file.close();
		mutex_file->unlock();
	}
	else {
		insertQueue(messages + message + line_effd, filePashAndName);
	}
	return 1;

}

std::string moduleX::LoggerX::getLogCoutTime(bool bBracket)
{
	time_t timep;
	time(&timep);
	char tmp[SIZE_64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
	if (!bBracket)
	{
		return tmp;
	}
	string tmp_str = tmp;
	return BRACKETS_LEFT + tmp_str + BRACKETS_RIGHT;
}




std::string moduleX::LoggerX::getUserName()
{
#if defined __linux__   //linux system
		uid_t userid;
		struct passwd* pwd;
		userid = getuid();
		pwd = getpwuid(userid);
		return pwd->pw_name;

#elif defined _WIN32  //windows system
		const int MAX_LEN = 100;
		char szBuffer[MAX_LEN];
		DWORD len = MAX_LEN;
		if (GetUserName(szBuffer, &len))     //用户名保存在szBuffer中,len是用户名的长度
			return szBuffer;
#else  //outher system
		return "";
#endif
}

std::string moduleX::LoggerX::getLogCoutThreadId()
{
#ifdef __linux__
	return to_string(syscall(__NR_gettid));
#elif _WIN32
	return to_string(GetCurrentThreadId());
#endif
}

std::string moduleX::LoggerX::getFilefullname()
{
	return _config["logFilePath"]  + _config["logName"] + ".log";
}

std::string moduleX::LoggerX::getFilefullnameWithTime()
{
	return _config["logFilePath"] + _config["logName"] + getLogCoutTime(false) + ".log";
}

void LoggerX::writeLog(LogLevel level, const char *format, ...)
{
	//是否写日志
	if (_config["logSwitch"] == "off")
		return;

	va_list args;
	va_start(args, format);
	if (_config["logFileSwitch"] == "on")
	{
		writeToTerminal(level, format, args);
	}

	if (_config["logTerminalSwitch"] == "on")
	{
		writeToFile(level, format, args);
	}

	va_end(args);

}

bool LoggerConfig::readConfig()
{
	_configMap.clear();

	initConfig();

	ifstream file;
	file.open(_configfile);

	if (!file.is_open())
	{
		cout << "readConfig error : " << _configfile << endl;
		return false;
	}

	char str_c[SIZE_128] = { 0 };

	string line;
	while (getline(file, line)) {
		if (!line.length()) {
			continue;
		}
		string str_copy = line;
		int j = 0;
		for (int i = 0; i < line.length(); i++) {
			if (line[i] == ' ')continue;
			str_copy[j] = line[i];
			j++;
		}
		str_copy.erase(j);
		if (str_copy[0] == '#')  continue;

		sscanf_s(str_copy.data(), "%[^=]", str_c);
		auto iter = _configMap.find(str_c);
		if (iter != _configMap.end()) {
			sscanf_s(str_copy.data(), "%*[^=]=%s", str_c);
			iter->second = str_c;
		}
		else {
		}
		
	}
	file.close();
}

void LoggerConfig::initConfig()
{
	_configMap.clear();

	_configMap["logSwitch"] = "off";
	_configMap["logFileSwitch"] = "off";
	_configMap["logTerminalSwitch"] = "off";
	_configMap["logFileQueueSwitch"] = "off";
	_configMap["logName"] = "";
	_configMap["logFilePath"] = "";
	_configMap["logMixSize"] = "";
	_configMap["logBehavior"]= "";
	_configMap["logOutputLevelFile"] = "";
	_configMap["logOutputLevelTerminal"] = "";
	return ;
}

std::string LoggerConfig::operator[](string key) const
{
	auto iter = _configMap.find(key);
	if (iter != _configMap.end())
	{
		return iter->second;
	}
}

long FileManage::getCurrentTime()
{
	time_t t;
	time(&t);

	struct tm *p;
	localtime_s(p, &t);

	return mktime(p);
}

bool moduleX::FileManage::createFilePath(string path)
{
	int len = path.length();
	if (!len) {
		path = "log";
#ifdef __linux__
		if (0 != access(path.c_str(), F_OK)) {
			if (-1 == mkdir(path.c_str(), 0)) {
				return false;
			}
		}
#elif _WIN32
		if (0 != access(path.c_str(), 0)) {
			if (-1 == mkdir(path.c_str())) {
				return false;
			}
		}
#endif
	}
	std::string fileName_cy(len, '\0');
	for (int i = 0; i < len; i++) {
		fileName_cy[i] = path[i];
		if (fileName_cy[i] == '/' || fileName_cy[i] == '\\') {
#ifdef __linux__
			if (-1 == access(fileName_cy.c_str(), F_OK)) {
				if (0 != mkdir(fileName_cy.c_str(), 0)) {
					return false;
				}
			}
#elif _WIN32
			if (-1 == access(fileName_cy.c_str(), 0)) {
				if (0 != mkdir(fileName_cy.c_str())) {
					return false;
				}
			}
#endif
		}
	}
	return true;
}

long moduleX::FileManage::getFileSize(std::string filename)
{
	struct stat statbuf;
	if (stat(filename.data(), &statbuf) == 0) {
		return statbuf.st_size;
	}
	return -1;
}

bool moduleX::FileManage::isFileExist(string fileName)
{
#ifdef __linux__
	return (access(fileName.data(), F_OK) != -1);
#elif _WIN32
	return (access(fileName.data(), 0) != -1);
#endif
}

bool moduleX::FileManage::fileRename(string oldname, string newname)
{
	if (!rename(oldname.data(), newname.data())) {
		cout << "File rename failed" << endl;
	}
	return 0;
}
