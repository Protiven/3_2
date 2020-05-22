#include <windows.h>


int function_key;
TCHAR info[256];



DWORD WINAPI ThreadFunc(void*) // дочерний процесс
{ 
   typedef int(*ImportFunction)(TCHAR*);
   ImportFunction DLLInfo;
   HINSTANCE hinstLib = LoadLibrary(TEXT("info.dll")); // подгрузка библиотеки
   DLLInfo = (ImportFunction)GetProcAddress(hinstLib, "Information");
   DLLInfo(info);
   FreeLibrary(hinstLib);

   int k = GetSystemMetrics(SM_NETWORK);
   //k = (k >> (sizeof(k) * 8 - 1)) & 1;
   k = k & 1;

   return 0;
}

LRESULT CALLBACK WindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   PAINTSTRUCT ps;
   HDC hDC;

   switch (msg)
   {
   case WM_CREATE:
      /*создание потомка, запуск процедуры в рамках созданного
      потомка*/
      HANDLE hThread;
      DWORD IDThread;
      hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
      WaitForSingleObject(hThread, INFINITE);
      CloseHandle(hThread);
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

   wcl.lpszClassName = L"MyClass";
   wcl.lpfnWndProc = WindowFunc;
   wcl.hIcon = LoadIcon(NULL, IDI_INFORMATION);
   wcl.hCursor = LoadCursor(NULL, IDC_ARROW); 
   wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
   wcl.style = CS_HREDRAW | CS_VREDRAW;

   RegisterClass(&wcl);
   hWnd = CreateWindow(L"MyClass", // Создается окно с названием "Resource management in computing systems"
      L"Resource management in computing systems",
      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN |
      WS_CLIPSIBLINGS, 100, 50, 700, 120,
      HWND_DESKTOP, NULL, hThisInst, NULL);
  
   /*
   // запуск процесса
   
   HANDLE hThread;
   DWORD IDThread;
   
   hThread = CreateThread(NULL, 0, ThreadFunc, &function_key, 0, &IDThread); // Запуск процесса
   
   WaitForSingleObject(hThread, INFINITE);
   CloseHandle(hThread);
   */

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   while (GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   return msg.wParam;
}