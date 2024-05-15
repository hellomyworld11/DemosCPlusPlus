#include "LoggerX.h"
#include <stdarg.h>

using namespace moduleX;

moduleX::LoggerX::LoggerX()
{
	_config.readConfig();
	init();
}

moduleX::LoggerX::~LoggerX()
{
	_bStart = false;
	_file.close();
}

//init static var
//TODO
LoggerX* moduleX::LoggerX::instance()
{
	static LoggerX instance;
	return &instance;
}

void moduleX::LoggerX::asyncLog(LoggerX* logger)
{
	string single_log;
	//从阻塞队列中取出一个日志string，写入文件
	if (logger == nullptr)
	{
		cout << "日志系统未开辟空间\n";
		return;
	}

	while (logger->_bStart)
	{	
		std::unique_lock <std::mutex> lck(logger->_lock_queue);
		while (logger->_logQueue.size() <= 0)
		{			
			logger->_cond.wait(lck);
		}
		single_log = logger->_logQueue.front();
		logger->_logQueue.pop();
		logger->_lock_file.lock();
		logger->_file << single_log;
		logger->_lock_file.unlock();
	}
	
}

bool moduleX::LoggerX::init()
{
	string filename = getFilefullname();


	string filefullname = getFilefullname();
	string filePath = getFilePath();
	cout << filefullname << " : " << filePath << endl;
	if (_config["logFileSwitch"] == "on") {
		//检查路径
		_filemgr.createFilePath(filePath);
		//检测文件有效性
		if (!_filemgr.isFileExist(filefullname)) {
			_filemgr.createFile(filefullname);
		}
		else {
			long fileSize = _filemgr.getFileSize(filefullname);
			if (fileSize > (long)atoi(_config["logMixSize"].data()) * MEGABYTES && _config["logBehavior"] == "1") {
				string newFileName = getFilefullnameWithTime();
				_filemgr.fileRename(filefullname, newFileName);
				_filemgr.createFile(filefullname);
			}
		}

		_file.open(filename, ::ios::app | ios::out);
		if (!_file.is_open())
		{
			cout << "log file open error\n";
			return false;
		}
	}

	//开启日志队列
	if (_config["logFileQueueSwitch"] == "on")
	{
		std::thread t(asyncLog, this);
		_threadId = t.get_id();
		t.detach();
	}
	
	return true;
}

void moduleX::LoggerX::writeToTerminal(LogLevel level, const char *format, va_list args)
{
	string line = getLogCoutTime() + __NAME__(level) + getUserName() + getLogCoutThreadId() + BRACKETS_LEFT +
		__FILE__ + SPACE + __FUNCTION__ + COLON + std::to_string(__LINE__) + BRACKETS_RIGHT;

	char szBuff[MAX_BUFF_SIZE];
	memset(szBuff, 0, sizeof(szBuff));
	vsprintf_s(szBuff, format, args);

	std::cout << line << szBuff << "\n";
	fflush(stdout); 
}

void moduleX::LoggerX::writeToFile(LogLevel level, const char *format, va_list args)
{
	std::ostringstream oss; 
	std::streambuf* pOldBuf = std::cout.rdbuf(oss.rdbuf());

	char szBuff[MAX_BUFF_SIZE];
	memset(szBuff, 0, sizeof(szBuff));
	vsprintf_s(szBuff, format, args);

	std::cout << szBuff;
	std::string ret = oss.str();
	std::cout.rdbuf(pOldBuf);

	string line = getLogCoutTime() + __NAME__(level) + getUserName() + getLogCoutThreadId() + BRACKETS_LEFT +
		__FILE__ + SPACE + __FUNCTION__ + COLON + std::to_string(__LINE__) + BRACKETS_RIGHT;
	
	string fullname = getFilefullname();

	long fileSize = _filemgr.getFileSize(fullname);
	if (fileSize > (long)atoi(_config["logMixSize"].data()) * MEGABYTES && _config["logBehavior "] == "1") {
		_file.close();
		string newFileName = getFilefullnameWithTime();
		_filemgr.fileRename(fullname, newFileName);
		_filemgr.createFile(fullname);

		_file.open(fullname, ::ios::app | ios::out);
		if (!_file.is_open())
		{
			cout << "log file open error\n";
			return ;
		}
	}
	if (_config["logFileQueueSwitch"] == "off") {
		_lock_file.lock();			
		_file << line << ret << "\n";
		_lock_file.unlock();
	}
	else {
		addLog(line + ret + "\n");
	}
}

std::string moduleX::LoggerX::getLogCoutTime(bool bBracket)
{
	time_t timep;
	time(&timep);
	char tmp[SIZE_64];

	struct tm curtime ;
	localtime_s(&curtime, &timep);
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", &curtime);
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

std::string moduleX::LoggerX::getFilePath()
{
	return _config["logFilePath"];
}

std::string moduleX::LoggerX::getFilefullnameWithTime()
{
	return _config["logFilePath"] + _config["logName"] + getLogCoutTime(false) + ".log";
}

bool moduleX::LoggerX::addLog(string messages)
{
	_lock_queue.lock();
	_logQueue.push(messages);

	if (_logQueue.size() > 0)
	{
		_cond.notify_all();
	}
	_lock_queue.unlock();
	return true;
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
		writeToFile(level, format, args);
		
	}

	if (_config["logTerminalSwitch"] == "on")
	{
		writeToTerminal(level, format, args);
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

		memset(str_c, 0, sizeof(str_c));
		sscanf_s(str_copy.data(), "%[^=]", str_c);
		auto iter = _configMap.find(str_c);
		if (iter != _configMap.end()) {
			sscanf_s(str_copy.data(), "%*[^=]=%s", str_c);
			iter->second = string(str_c);
		}
		else {
		}
		
	}
	file.close();
}

void LoggerConfig::initConfig()
{
	_configMap.clear();

	_configMap["logSwitch"] = "on";
	_configMap["logFileSwitch"] = "off";
	_configMap["logTerminalSwitch"] = "on";
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

	struct tm p;
	localtime_s(&p, &t);

	return mktime(&p);
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
		if (0 != _access(path.c_str(), 0)) {
			if (-1 == _mkdir(path.c_str())) {
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
			if (-1 == _access(fileName_cy.c_str(), 0)) {
				if (0 != _mkdir(fileName_cy.c_str())) {
					return false;
				}
			}
#endif
		}
	}
	return true;
}

bool moduleX::FileManage::createFile(string filename)
{
	ofstream file;
	file.open(filename, ::ios::app | ios::out);
	if (!file) {
		cout << "Failed to create file" << endl;
		return false;
	}
	file.close();
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
	return (_access(fileName.data(), 0) != -1);
#endif
}

bool moduleX::FileManage::fileRename(string oldname, string newname)
{
	if (!rename(oldname.data(), newname.data())) {
		cout << "File rename failed" << endl;
	}
	return 0;
}
