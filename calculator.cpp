#define UNICODE
#define _UNICODE
#include <windows.h>
#include <wchar.h>
static HBRUSH g_brushGreen = NULL;
static COLORREF G_GREEN = RGB(0, 180, 0);
static HBRUSH   g_brushWhite = NULL;

static void ShowLastError(HWND parent, const wchar_t* captionPrefix)
{
    DWORD err = GetLastError();
    wchar_t buf[512];
    swprintf(buf, 512, L"%s\nGetLastError() = %lu", captionPrefix, (unsigned long)err);
    MessageBoxW(parent, buf, L"Error", MB_OK | MB_ICONERROR);
}

static HWND g_input1 = NULL;
static HWND g_input2 = NULL;

static double ReadNumber(HWND hEdit)
{
    wchar_t wbuf[256] = {0};
    GetWindowTextW(hEdit, wbuf, 256);
    wchar_t* endptr = nullptr;
    double v = wcstod(wbuf, &endptr);
    return v;
}

/* This is where all the input to the window goes to */

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        CreateWindowW(L"STATIC", L"Please input two numbers",
            WS_CHILD | WS_VISIBLE,
            20, 10, 200, 20,
            hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        
			g_input1 = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            20, 40, 160, 24,
            hwnd, (HMENU)101, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        g_input2 = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            20, 75, 160, 24,
            hwnd, (HMENU)102, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        CreateWindowW(L"BUTTON", L"+", WS_CHILD | WS_VISIBLE,  20, 120, 30, 30, hwnd, (HMENU)201, NULL, NULL);
        CreateWindowW(L"BUTTON", L"-", WS_CHILD | WS_VISIBLE,  60, 120, 30, 30, hwnd, (HMENU)202, NULL, NULL);
        CreateWindowW(L"BUTTON", L"x", WS_CHILD | WS_VISIBLE, 100, 120, 30, 30, hwnd, (HMENU)203, NULL, NULL);
        CreateWindowW(L"BUTTON", L"/", WS_CHILD | WS_VISIBLE, 140, 120, 30, 30, hwnd, (HMENU)204, NULL, NULL);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 201: // +
        case 202: // -
        case 203: // x
        case 204: // /
        {
            double a = ReadNumber(g_input1);
            double b = ReadNumber(g_input2);
            double result = 0.0;

            if (LOWORD(wParam) == 201)      result = a + b;
            else if (LOWORD(wParam) == 202) result = a - b;
            else if (LOWORD(wParam) == 203) result = a * b;
            else if (LOWORD(wParam) == 204) {
                if (b == 0.0) {
                    MessageBoxW(hwnd, L"Cannot divide by zero", L"Error", MB_OK | MB_ICONWARNING);
                    return 0;
                }
                result = a / b;
            }

            wchar_t out[256];
            swprintf(out, 256, L"%f", result);
            MessageBoxW(hwnd, out, L"Result", MB_OK);
            return 0;
        }
        }
        break;
		
    	case WM_CTLCOLORSTATIC:
    	case WM_CTLCOLOREDIT:
		{
    		HDC hdc = (HDC)wParam;
    		SetTextColor(hdc, RGB(0, 0, 0));
    		SetBkColor(hdc, RGB(255, 255, 255));
    		if (!g_brushWhite) g_brushWhite = CreateSolidBrush(RGB(255, 255, 255));
    		return (LRESULT)g_brushWhite;
		}

    	case WM_CTLCOLORBTN:
    	{
        	HDC hdc = (HDC)wParam;
        	SetTextColor(hdc, RGB(0, 0, 0));
        	SetBkMode(hdc, TRANSPARENT);
        	if (!g_brushGreen) g_brushGreen = CreateSolidBrush(G_GREEN);
        	return (LRESULT)g_brushGreen;
    	}

		/* Upon destruction, tell the main thread to stop */
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		
		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEXW wc = {0}; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG msg; /* A temporary location for all messages */

	if (!g_brushGreen) g_brushGreen = CreateSolidBrush(G_GREEN);

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize	 = sizeof(wc);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor	 = LoadCursor(NULL, IDC_ARROW);
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground	= g_brushGreen;
	wc.lpszClassName	= L"MyCalcClass";
	wc.hIcon	= LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm	 = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Window Registration Failed!",L"Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

	hwnd = CreateWindowEx(0,L"MyCalcClass",L"My Calculator",style,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		250, /* width */
		200, /* height */
		NULL,NULL,hInstance,NULL);

	if(hwnd == NULL) {
		MessageBox(NULL, L"Window Creation Failed!",L"Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

	/*
		This is the heart of our program where all input is processed and 
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	while(GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}
	return msg.wParam;
}
