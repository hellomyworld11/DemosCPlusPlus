#include<iostream>
#include "../LoggerX.h"
#include <stdarg.h>

using namespace std;
#define ARGDEFINE(format, ...)  testArg(format, ##__VA_ARGS__)

void testArg(const char *format, ...)
{
	
	va_list valst;
	va_start(valst, format);

	char szBuff[MAX_BUFF_SIZE];
	memset(szBuff, 0, sizeof(szBuff));
	vsprintf_s(szBuff, format, valst);

	cout << "ret : " << szBuff << endl;
	va_end(valst);
}

int main(int argc, char *argv[])
{
	using namespace moduleX;

	string name1 = "你好xp";
	ARGDEFINE("hello: %s, age: %d", name1.data(), 12);
	
//	getchar();
//	return 0;

	time_t begin, end;
	double ret;
	begin = clock();
	string name = "你好xp";
	for (int i = 0; i < 10000; i++) {
		LOG_DEBUG("test %s %d", name.data(), i);
		LOG_DEBUG("test %s %d", name.data(), i);
		LOG_DEBUG("test %s %d", name.data(), i);
		LOG_DEBUG("test %s %d", name.data(), i);
		LOG_DEBUG("test %s %d", name.data(), i);
	}
	end = clock();
	ret = double(end - begin) / CLOCKS_PER_SEC;
	cout << "runtime:   " << ret << endl;


	return 0;
}

