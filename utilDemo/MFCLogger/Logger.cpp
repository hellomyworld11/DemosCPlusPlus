#include "Logger.h"

#include <assert.h>

#include "function.h"
#include "MFCLoggerDlg.h"

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

bool Logger::log(Level level, Target target, const char *format, ...)
{
	va_list args;
	va_start(args, format);

	std::string line = getLogCoutTime() + " " + __NAME__(level)  + "["
		__FILE__ + " " + __FUNCTION__ + ":" + std::to_string(__LINE__) + "] >  ";

	char szBuff[MAX_BUFF_SIZE];
	memset(szBuff, 0, sizeof(szBuff));
	vsprintf_s(szBuff, format, args);

	std::string output = line + szBuff + "\n";

	if (target == Target::file || target == Target::all)
	{
		file_ << output /*<< "\n"*/;
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
