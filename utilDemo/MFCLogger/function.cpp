#include "function.h"


std::string convert(CString str)
{
	return CT2A((LPCTSTR)str);
}

CString convert(std::string str)
{
#ifdef _UNICODE
	return CA2W(str.c_str());
#elif
	return CA2T(str.c_str());
#endif
}

bool GetStrBefore(CString strIn, CString strSep,CString& strOut)
{
	if (strIn==L""||strSep==L"")
	{
		strOut=L"";
		return false;
	}
	int pos=strIn.Find(strSep);
	if (pos==-1)
	{
		strOut=L"";
		return false;
	}
	strOut=strIn.Left(pos);
	return true;	
}



float CStrToFloat(CString str)
{
	wchar_t w_str[200];
	wcscpy_s(w_str,str);
	return(_wtof(w_str));
};