#pragma once
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <sstream>
#include<io.h>

#define MAX_BUFF_SIZE 512
#define __NAME__(name) #name
using namespace std;

class Logger final
{
public:

	enum class Level
	{
		debug,
		info,
		warning,
		error
	};

	enum class Target
	{
		file,
		terminal,
		all
	};

	bool log(Level level, Target target, const char *format, ...);

	static Logger* instance();

private:
	explicit Logger(void);

	~Logger(void);

	std::string getLogCoutTime();

	ofstream file_;

	std::string filename_;
};

#define LOG_DEBUG(format, ...) do{Logger::instance()->log(Logger::Level::debug,   Logger::Target::all,format, ##__VA_ARGS__);}while(0)
#define LOG_INFO(format, ...)  do{Logger::instance()->log(Logger::Level::info,    Logger::Target::all,format, ##__VA_ARGS__);}while(0)
#define LOG_WARN(format, ...)  do{Logger::instance()->log(Logger::Level::warning, Logger::Target::all,format, ##__VA_ARGS__);}while(0)
#define LOG_ERROR(format, ...) do{Logger::instance()->log(Logger::Level::error,   Logger::Target::all,format, ##__VA_ARGS__);}while(0)


