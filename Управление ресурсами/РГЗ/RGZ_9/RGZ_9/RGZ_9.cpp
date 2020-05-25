#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>
TCHAR info[256];

/* 
If the process completed successfully return 1
If the process could not load the library return -1
If the process could not load the function from library return (Network connection) -2
If the process could not load the function from library return (htt) -3
*/
DWORD WINAPI get_info(void*) 
{ 
   typedef int(*ImportFunction)(TCHAR*);
   ImportFunction check_network, check_availability_HyperThreading;
   HINSTANCE h_Lib;
   
   TCHAR info_[2];

   if ((h_Lib = LoadLibrary(TEXT("Dll.dll"))) == NULL)
      return -1;
   
   if (!(check_network = (ImportFunction)GetProcAddress(h_Lib, "check_network")))
      return -2;
   else check_network(info_);

   info[0] = info_[0];

   if (!(check_availability_HyperThreading = (ImportFunction)GetProcAddress(h_Lib, "check_availability_HyperThreading")))
      return -3;
   else check_availability_HyperThreading(info_);
   info[1] = info_[1];

   FreeLibrary(h_Lib);
   return 1;
}

LRESULT CALLBACK WindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   PAINTSTRUCT ps;
   HDC hDC;
   char c = ' ';

   switch (msg)
   {
   case WM_CREATE:
      char text_[255];
      char text_sup[30];
      char text_sup_2[30];
      

      HANDLE hThread;
      DWORD IDThread;
      hThread = CreateThread(NULL, 0, get_info, NULL, 0, &IDThread);

      DWORD iRetVal;
      WaitForSingleObject(hThread, INFINITE);
      GetExitCodeThread(hThread, &iRetVal);
      CloseHandle(hThread);
      
      if (iRetVal != 1)
      {
         switch (iRetVal)
         {
         case -1: // Ошибка загрузки библиотеки
            MessageBox(0, L"Failed to load library!\0", 
               L"Error!", MB_ICONERROR);
            break;
         case -2: // Ошибка загрузки функций из библиотеки
            MessageBox(0, L"Failed to load function from library (checking connections)!\0",
               L"Error!", MB_ICONERROR);
            break;
         case -3:
            MessageBox(0, L"Failed to load function from library (support Hyper-Threading)!\0",
               L"Error!", MB_ICONERROR);
            break;
         }
         return -1;
      }
      else
      {
         if (info[0] == 1)
            strcpy(text_sup, "exist");
         else
            strcpy(text_sup, "absent");
         
         if (info[1] == 1)
            strcpy(text_sup_2, "Yes");
         else
            strcpy(text_sup_2, "No");

         sprintf(text_, "Network: %s. Support Hyper-Threading: %s.", text_sup, text_sup_2);
      }
      
      c = text_[0];
      for (int i = 1; c != '\0'; i++)
      {
         info[i - 1] = c;
         c = text_[i];
      }

      break;
   case WM_PAINT:
      hDC = BeginPaint(hWnd, &ps);
      TextOut(hDC, 10, 10, info, lstrlen(info));
      EndPaint(hWnd, &ps);
      break;
   case WM_DESTROY:
      PostQuitMessage(0);
   default:
      return DefWindowProc(hWnd, msg, wParam, lParam);
   }
   return 0;
}


int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE, LPSTR, int nCmdShow)
{
   MSG msg{};
   WNDCLASS wcl = { 0 };
   HWND hWnd;
   wcl.hInstance = hThisInst;

   wcl.lpszClassName = L"My_Class";
   wcl.lpfnWndProc = WindowFunc;
   wcl.hIcon = LoadIcon(NULL, IDI_INFORMATION);
   wcl.hCursor = LoadCursor(NULL, IDC_ARROW); 
   wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
   wcl.style = CS_HREDRAW | CS_VREDRAW;

   RegisterClass(&wcl);
   hWnd = CreateWindow(L"My_Class", 
      L"Resource management in computing systems",
      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN |
      WS_CLIPSIBLINGS, 125, 60, 600, 120,
      HWND_DESKTOP, NULL, hThisInst, NULL);
 
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   while (GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   return msg.wParam;
}