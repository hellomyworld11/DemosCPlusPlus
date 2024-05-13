#include "LoggerX.h"

using namespace moduleX;

//init static var
//TODO

LoggerX::LoggerX()
{

}


LoggerX::~LoggerX()
{

}

LoggerX* moduleX::LoggerX::instance()
{
	static LoggerX instance;
	return &instance;
}

bool LoggerConfig::readConfig()
{
	_configMap.clear();



}

bool moduleX::FileManage::createFile(std::string filename)
{
	ofstream file;

	file.open();


}
