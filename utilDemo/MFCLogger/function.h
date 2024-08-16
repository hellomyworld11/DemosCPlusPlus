#pragma once

#include "stdafx.h"


//cstring -> std::string
std::string convert(CString str);
//std::string -> cstring
CString convert(std::string str);

bool GetStrBefore(CString strIn, CString strSep,CString& strOut);


float CStrToFloat(CString str);
