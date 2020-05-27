#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <string.h>
#include <intrin.h>

//If the network is exist, then function return 1 in first position. Else return 0.
extern "C" _declspec(dllexport) void  check_network(TCHAR* val) 
{
	int value = GetSystemMetrics(SM_NETWORK);
	__asm 
	{
      mov eax, value
      and eax, 1
      mov value, eax
	}

	if (value == 1) val[0] = 1;
	else  val[0] = 0;
}


// If Hyper Threading is supported then return 1 in second position, else return 0
extern "C" __declspec(dllexport) void check_availability_HyperThreading(TCHAR* val) 
{
   int hyperThreads = 0;
   unsigned int sup_value, sup_value_2, sup_value_3;
   char vendor[12];
   int sup_value_4;
  

   //Если выполнить cpuid с EAX=0, то:
   //EAX - максимальное значение, с которым можно выполнять cpuid
   //EBX:EDX:ECX - строка - 12-байтный идентификатор производителя


   // Узнаем производителя процессора
   __asm
   {
      mov eax, 0
      cpuid
      MOV sup_value_4, ECX
      MOV sup_value_2, EDX
      mov sup_value, ebx
      mov eax, 1
      cpuid
      shr edx, 28
      mov sup_value_3, edx;
   }
   ((unsigned*)vendor)[0] = sup_value;
   ((unsigned*)vendor)[1] = sup_value_2;
   ((unsigned*)vendor)[2] = sup_value_4;


   std::string cpuVendor = std::string(vendor, 12);
   if (cpuVendor == "GenuineIntel" && (sup_value_3 & 1)) // Сравнение с индентификатором EBX (Intel)
   {
      __asm
      {
         mov eax, 1
         cpuid
         mov sup_value, ebx; // >> 16 & 0x0FF Отвечает за лог ядра
         mov sup_value_2, eax; // >> 26 & 0x3f + 1 Отвечает за ядра

      }
      if (((sup_value >> 16) & 0x0FF) / (((sup_value_2 >> 26) & 0x3f) + 1))
         hyperThreads = 1; // здесь уже тру
   }

   if (hyperThreads == 1) val[1] = 1;
   else val[1] = 0;
}