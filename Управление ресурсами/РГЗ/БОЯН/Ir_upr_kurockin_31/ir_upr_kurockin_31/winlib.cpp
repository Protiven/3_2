#include <windows.h>
extern "C"
__declspec(dllexport)
int ifconnected() // Function to detect of connection
{
    int wc = GetSystemMetrics(SM_NETWORK);
	__asm // Receive a first bit
	{
		mov edx, wc
		and edx, 1 // A very simple bit mask
		mov wc, edx
	}
	return wc;
}
extern "C"
__declspec(dllexport)
int htt_support() // Function of detection of Hyper Threating Technology
{
    int htt;
    __asm
    {
        mov eax, 1
        cpuid
        and edx, 10000000h // A bit mask
        shr edx, 28
        mov htt, edx
    }
    return htt;
}