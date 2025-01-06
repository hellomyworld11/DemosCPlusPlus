#include "Logger.h"

#include <assert.h>
#include <sys/stat.h>
#include "function.h"
#include "MFCLoggerDlg.h"
const int MEGABYTES = 1048576;
const int MaxFileSize = 6 * MEGABYTES;

Logger::Logger(void):filename_("./log_minotor.txt")
{
	file_.open(filename_.c_str(), ::ios::app | ::ios::out);
	if (!file_.is_open())
	{
		std::cout << "log file open error\n";
		assert(file_.is_open());
	}

#ifdef WIN32
	AllocConsole();                     // 打开控制台资源
	freopen( "CONOUT$", "w", stdout );// 申请写
//	freopen( "CONIN$", "r", stdin );  // 申请读
#endif // WIN32

}


Logger::~Logger(void)
{
	file_.close();
#ifdef WIN32
	FreeConsole();
#endif // WIN32
}

Logger* Logger::instance()
{
	static Logger instance;
	return &instance;
}

bool Logger::log(Level level, Target target, std::string filename, std::string function, unsigned int linenumber, const char *format, ...)
{
	struct stat statbuf;
	if (stat(filename_.data(), &statbuf) == 0) {
		long fileSize = statbuf.st_size;
		if (fileSize > MaxFileSize)
		{	//清空文件
			ofstream file_writer(filename_, ios_base::out);
		}
	}
	
	va_list args;
	va_start(args, format);

	std::string line = getLogCoutTime() + " " + levelstring(level)  + SPACE +"[" +
		filename + SPACE + function + SPACE + std::to_string(linenumber) + "]" + ">  ";

	char szBuff[MAX_BUFF_SIZE];
	memset(szBuff, 0, sizeof(szBuff));
	vsprintf_s(szBuff, format, args);

	std::string output = line + szBuff + "\n";

	if (target == Target::file || target == Target::all)
	{
		file_ << output /*<< "\n"*/;
		file_.flush();
	}
	if(target == Target::terminal || target == Target::all)
	{	
		std::cout << line << szBuff << "\n";
		fflush(stdout); 
	
		CMFCLoggerDlg::gOutput.SetSel(-1, -1);
		CMFCLoggerDlg::gOutput.ReplaceSel( convert(output) );
	}

	va_end(args);
	return true;
}

std::string Logger::getLogCoutTime()
{
	time_t timep;
	time(&timep);
	char tmp[64];

	struct tm curtime ;
	localtime_s(&curtime, &timep);
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", &curtime);
	return tmp;
}

std::string Logger::levelstring(Level level)
{
	switch (level)
	{
	case Logger::Level::debug:
		return "debug";
	case Logger::Level::info:
		return "info";
	case Logger::Level::warning:
		return "warn";
	case Logger::Level::error:
		return "error";
	default:
		return "";
	}
	return "";
}
