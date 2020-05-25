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
   char vendor[12];
   int EAX1, EBX1, ECX1, EDX1;
   __asm 
   {
      cpuid
      mov EAX1, EAX
      MOV EBX1, EBX
      MOV ECX1, ECX
      MOV EDX1, EDX
   }

   ((unsigned*)vendor)[0] = EBX1;
   ((unsigned*)vendor)[1] = EDX1; 
   ((unsigned*)vendor)[2] = ECX1; 
  
   
   unsigned cpuFeatures = EDX1;
   std::string cpuVendor = std::string(vendor, 12);
   unsigned logical = (EBX1 >> 16) & 0xff; 
   unsigned cores = logical;
   unsigned regs[4];

   if (cpuVendor == "GenuineIntel") {
      cores = ((EAX1 >> 26) & 0x3f) + 1; 
   }
   else if (cpuVendor == "AuthenticAMD") {
      __cpuid((int*)regs, (int)0x80000008);
      cores = ((unsigned)(regs[2] & 0xff)) + 1; 
   }

   bool hyperThreads = cpuFeatures & (1 << 28) && cores < logical;
   hyperThreads = hyperThreads ? "true" : "false";

   if (hyperThreads == 1) val[1] = 1;
   else val[1] = 0;
}