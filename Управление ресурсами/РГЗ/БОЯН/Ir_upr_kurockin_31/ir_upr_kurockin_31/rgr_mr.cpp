#include <windows.h>
#include <string>
#include <sstream>
using namespace std;
HWND hwnd, MSLabel;
int fstatus = 0, fstatus_old = -1;
void thread() // A function of access to dynamic library
{
    HINSTANCE hinstLib = LoadLibrary(TEXT("winlib.dll")); //A handle to instance of dynamic library
    if(hinstLib != NULL) //If we receive a handle
    {
        int htt, ifc; //Variables of status of connection and htt-support
        stringstream lbl; //An occurrence of stream class to operate on strings
        lbl << "PM-01, Kurochkin A.V." << endl << "    variant 9" << endl << endl;//Add a signature
        typedef int (*ifconnected_)(); //Import
        ifconnected_ ifconnected = (ifconnected_)GetProcAddress(hinstLib,"ifconnected");
        if(ifconnected != NULL)
        {
			ifc = ifconnected();//Receive a data to apply a picture
            if(ifconnected)//Solution
                lbl << "LEVEL III:" << endl << "    Network connection" << endl << "    is detected!" << endl;
            else
                lbl << "LEVEL III:" << endl << "    Network connection" << endl << "    is not detected!" << endl;
        }
        else // If something wrong with dynamic library
            MessageBox(hwnd, "int ifconnected() not found in winlib.dll", "Error", MB_OK | MB_ICONERROR);
        typedef int (*htt_support_)();//HTT section
        htt_support_ htt_support = (htt_support_)GetProcAddress(hinstLib, "htt_support");
        if(htt_support != NULL)
        {
            htt = htt_support();//Receive a data to apply a picture
			//lbl << ifc << " " << htt << endl;
            lbl << "LEVEL IV:" << endl;
            if(htt)//Solution
                lbl << "    Hyper Threading" << endl << "    is supported!";
            else
                lbl << "    Hyper Threading" << endl << "    is not supported!";
            if(ifc && htt) fstatus = 3; // Number of situation and picture
            if(!ifc && htt) fstatus = 1;
            if(ifc && !htt) fstatus = 2;
            if(!ifc && !htt) fstatus = 0;
            SetWindowText(MSLabel, (lbl.str()).c_str());
        }
        else// If something wrong with dynamic library
            MessageBox(hwnd, "int htt_support() not found in winlib.dll", "Error", MB_OK | MB_ICONERROR);
        FreeLibrary(hinstLib);// Free
    }
    else //If dynamic library did not found
        MessageBox(hwnd, "winlib.dll did not found! Put it into the program folder and click \"Run\"!", "Error", MB_OK | MB_ICONERROR);
}
LONG WINAPI WndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam) // Message processor
{
    int wmId, wmEvent;
    static HDC memBit;
    static HBITMAP hBitmap;
    static BITMAP bm;
    switch (Msg)
    {
    case WM_CTLCOLORSTATIC:// Text and background
    {
        DWORD CtrlID = GetDlgCtrlID((HWND)lParam);
        if (CtrlID == 1024)
        {
            SetTextColor((HDC)wParam, RGB(30, 200, 30));
            SetBkColor((HDC)wParam, RGB(0, 0, 0));
            return (INT_PTR)GetStockObject(BLACK_BRUSH);
        }
    }
    break;
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == 1025)
        {
            HANDLE hThread;
            DWORD IDThread;
            hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, NULL, 0, &IDThread); //Function in thread
            CloseHandle(hThread);
        }
        if (LOWORD(wParam) == 1026)
            PostQuitMessage(0);
    }
    break;
    case WM_PAINT:{ //Output a bitmap
		PAINTSTRUCT ps;
		HDC hdc; //contrxt
		if(fstatus != fstatus_old)
		{
			switch(fstatus)//Choice of bitmaps
			{
			case 3:
				hBitmap = (HBITMAP)LoadImage(NULL, TEXT("images/11.bmp"), IMAGE_BITMAP,  0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				break;
			case 2:
                hBitmap = (HBITMAP)LoadImage(NULL, TEXT("images/10.bmp"), IMAGE_BITMAP,  0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
                break;
            case 1:
                hBitmap = (HBITMAP)LoadImage(NULL, TEXT("images/01.bmp"), IMAGE_BITMAP,  0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
                break;
            case 0:
                hBitmap = (HBITMAP)LoadImage(NULL, TEXT("images/00.bmp"), IMAGE_BITMAP,  0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
                break;
            };
            if (hBitmap == NULL)// Load error
            {
                stringstream msg;
                msg << "Image load error! An image did not found!";
                MessageBox(hwnd, (msg.str()).c_str(), "Error", MB_OK | MB_ICONERROR);
                DestroyWindow(hwnd);
                return 1;
            }
            GetObject(hBitmap, sizeof(bm), &bm);
            hdc = GetDC(hwnd);
            memBit = CreateCompatibleDC(hdc);
            SelectObject(memBit, hBitmap);
            ReleaseDC(hwnd, hdc);
        }
        hdc = BeginPaint(hwnd, &ps);
        BitBlt(hdc, 20, 130, bm.bmWidth, bm.bmHeight, memBit, 0, 0, SRCCOPY);//вывод изображения
        EndPaint(hwnd, &ps);
        if(fstatus != fstatus_old)
        {
            InvalidateRect(hwnd, 0, 0);
            UpdateWindow(hwnd);
            fstatus_old = fstatus;
        }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, Msg, wParam, lParam);
    }
    return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wnd;
    RECT rt;
    HANDLE hThread;
    DWORD IDThread;
    HFONT font_std = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    HFONT font_mono = (HFONT)GetStockObject(OEM_FIXED_FONT);
    memset(&wnd, 0, sizeof(WNDCLASS));
    wnd.style = CS_HREDRAW | CS_VREDRAW;
    wnd.lpfnWndProc = WndProc;
    wnd.hInstance = hInstance;
    wnd.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wnd.lpszClassName = "main_class";
    wnd.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(100));
    RegisterClass(&wnd);
    int window_width = 200;
    int window_height = 300;
    int button_width = 90;
    int button_height = 30;
    int border = 3;
    HDC hDCScreen = GetDC(NULL);
    hwnd = CreateWindow("main_class", "MR RGR", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        (GetDeviceCaps(hDCScreen, HORZRES) - window_width) / 2, (GetDeviceCaps(hDCScreen, VERTRES) - window_height) / 2,
        window_width, window_height, NULL, NULL, hInstance, NULL);
    GetClientRect(hwnd, &rt);
    window_width = rt.right;
    window_height = rt.bottom;
    MSLabel = CreateWindow("static", "Click \"Run\" to get values", WS_CHILD | WS_VISIBLE | WS_BORDER,
        border, border, window_width - 2 * border, (window_height - button_height - 4 * border) / 2,
        hwnd, (HMENU)1024, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1024, WM_SETFONT, (WPARAM)font_mono, TRUE);
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, NULL, 0, &IDThread);
    CloseHandle(hThread);//Buttons below
    CreateWindow("button", "Run", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, window_width - 2 * (border + button_width), window_height - border - button_height, button_width, button_height, hwnd, (HMENU)1025, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1025, WM_SETFONT, (WPARAM)font_std, TRUE);
    CreateWindow("button", "Exit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, window_width - border - button_width, window_height - border - button_height, button_width, button_height, hwnd, (HMENU)1026, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1026, WM_SETFONT, (WPARAM)font_std, TRUE);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))//Loop
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
