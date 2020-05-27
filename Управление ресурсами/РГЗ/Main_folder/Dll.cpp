#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>

//If the network is exist, then function return 1 in first position. Else return 0.
extern "C" _declspec(dllexport) void  check_network(TCHAR* val) 
{
	int value = GetSystemMetrics(SM_NETWORK);
	__asm 
	{
      mov eax, value; // value копируем в EAX
      and eax, 1; // берем первый бит из EAX
      mov value, eax; // результат возвращаем в value
	}

   // Возвращение результата
	if (value == 1) val[0] = 1;
	else  val[0] = 0;
}

// If Hyper Threading is supported then return 1 in second position, else return 0
extern "C" __declspec(dllexport) void check_availability_HyperThreading(TCHAR* val) 
{
   int value;
   __asm
   {
      mov eax, 1
      cpuid
      shr eax, 28
      and edx, 1
      mov value, edx
   }
   
   // Возвращение результата
   if (value == 1) val[1] = 1;
   else val[1] = 0;
}