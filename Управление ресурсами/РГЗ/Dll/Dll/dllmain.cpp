#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include<stdio.h>

extern "C" _declspec(dllexport) TCHAR  Information(TCHAR * InfoString)
{
	wsprintf(InfoString, L"The number of keyboard function keys is defined:%d", GetKeyboardType(2));

	return 0;
}