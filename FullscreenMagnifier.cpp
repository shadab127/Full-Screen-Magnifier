#include"magnification.h"
#include<windows.h>

//Global variables
float magnificationFactor	 = 1.0;//initial magnification factor
int timerInterval			 = 16;//set to 16 for approx 60Hz refresh rate

//macros
#define Zoom_Out				0
#define Zoom_In					1
#define About					2


HWND hWndParent;
HMENU hMenu;

LRESULT CALLBACK    WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void				AddControls(HWND);
void CALLBACK       UpdateMagWindow(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
void				SetInputTransform(_In_ HWND hwndDlg, _In_ BOOL fSetInputTransform);


//entry point of application
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	WNDCLASSW wc = { 0 };
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = L"myWindowClass";
	wc.lpfnWndProc = WindowProcedure;
	if (!RegisterClassW(&wc))
	{
		return -1;
	}
	//initialise magnifier
	MagInitialize();
	CreateWindow(L"myWindowClass", L"Magnifier", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 300, 205,
		NULL, NULL, NULL, NULL);
	MSG msg = { 0 };

	// Create a timer to update the control.
	UINT_PTR timerId = SetTimer(NULL, 0, timerInterval, UpdateMagWindow);

	RegisterHotKey(
		NULL,
		0,
		MOD_ALT | MOD_NOREPEAT,
		0x6B);
	RegisterHotKey(
		NULL,
		1,
		MOD_ALT | MOD_NOREPEAT,
		0x6D);
	RegisterHotKey(
		NULL,
		2,
		MOD_ALT | MOD_NOREPEAT,
		0x43);
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		
		if (msg.message == WM_HOTKEY)
		{
			if (msg.wParam == 0)
			{
				if (magnificationFactor < 10.0)
				{
					magnificationFactor += 1.0;
				}
			}
			if (msg.wParam == 1)
			{
				if (magnificationFactor > 1.0)
				{
					magnificationFactor -= 1.0;
				}
			}
			if (msg.wParam == 2)
			{
				return -1;
			}

		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	KillTimer(NULL, timerId);
	// Any current magnification and color effects will be turned off as a result of calling MagUninitialize().
	MagUninitialize();
	return 0;
}

//window procedure funtion for receiving message from window
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	hWndParent = hWnd;
	/*
	if (wp & HIBYTE(GetKeyState(VK_MENU)))
	{
		if (wp & HIBYTE(GetKeyState(VK_MENU)) && wp & HIBYTE(GetKeyState(VK_ADD)))
		{
			//click+Shift+Ctrl

			if (magnificationFactor < 10.0)
			{
				magnificationFactor += 1.0;
			}
		}
		else if (wp & HIBYTE(GetKeyState(VK_MENU)) && HIBYTE(GetKeyState(VK_SUBTRACT)) & wp)
		{
			//alt+shift

			if (magnificationFactor > 1.0)
			{
				magnificationFactor -= 1.0;
			}
		}
	}
	*/

	switch (msg)
	{
	
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (wp)
		{
		//If zoom in button pressed increase magnificationfactor
		case Zoom_In:
		{

			if (magnificationFactor < 10.0)
			{
				magnificationFactor += 1.0;
			}
		}
		break;

		//If zoom out button pressed decrease magnificationfactor
		case Zoom_Out:
		{
			if (magnificationFactor > 1.0)
			{
				magnificationFactor -= 1.0;
			}
		}
		break;
		case About:
		{
			MessageBox(hWnd, L"© 2019\nDeveloper : Shadab Khan", L"About",MB_OK);
			break;
		}
		}
		break;
	case WM_CREATE:
		AddControls(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
		break;
	}
}

//function for creating buttons
void AddControls(HWND hWnd)
{
	CreateWindowW(L"button", L"Zoom_In", WS_VISIBLE | WS_CHILD | WS_BORDER, 5, 5, 100, 50, hWnd, (HMENU)Zoom_In, NULL, NULL);
	CreateWindowW(L"button", L"Zoom_Out", WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 5, 100, 50, hWnd, (HMENU)Zoom_Out, NULL, NULL);
	CreateWindowW(L"static", L"Press 'Alt' + '+' for zoom in", WS_VISIBLE | WS_CHILD, 5, 60, 250, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"static", L"Press 'Alt' + '-' for zoom out", WS_VISIBLE | WS_CHILD, 5, 85, 250, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"static", L"Press 'Alt'+'C' for quit", WS_VISIBLE | WS_CHILD, 5, 110, 250, 20, hWnd, NULL, NULL, NULL);
	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, About, L"About");
	SetMenu(hWnd, hMenu);
}


//Function that magnify full screen such that cursor remain at the center of screen
void CALLBACK UpdateMagWindow(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR /*idEvent*/, DWORD /*dwTime*/)
{
	POINT mouseLocation;
	GetCursorPos(&mouseLocation);

	//width of screen in pixels
	int width = (int)((float)GetSystemMetrics(SM_CXSCREEN) / magnificationFactor);

	//height of screen in pixels
	int height = (int)((float)GetSystemMetrics(SM_CYSCREEN) / magnificationFactor);
	int xWnd = mouseLocation.x - width / 2;
	int yWnd = mouseLocation.y - height / 2;

	if (xWnd < 0)
	{
		xWnd = 0;
	}
	if (yWnd < 0)
	{
		yWnd = 0;
	}
	if (mouseLocation.x + width / 2 > (int)GetSystemMetrics(SM_CXSCREEN))
	{
		xWnd = (int)GetSystemMetrics(SM_CXSCREEN) - width;
	}
	if (mouseLocation.y + height / 2 > (int)GetSystemMetrics(SM_CYSCREEN))
	{
		yWnd = (int)GetSystemMetrics(SM_CYSCREEN) - height;
	}
	//set magnification factor and offset
	MagSetFullscreenTransform(magnificationFactor, xWnd, yWnd);
	SetInputTransform(hWndParent, true);
}

//
// FUNCTION: SetInputTransform()
//
// PURPOSE: Apply an input transform to allow touch and pen input to account 
//          for the current fullscreen or lens magnification.
//
void SetInputTransform(_In_ HWND hwndDlg, _In_ BOOL fSetInputTransform)
{
	bool fContinue = true;

	RECT rcSource = { 0 };
	RECT rcDest = { 0 };

	// MagSetInputTransform() is used to adjust pen and touch input to account for the current magnification.
	// The "Source" and "Destination" rectangles supplied to MagSetInputTransform() are from the perspective
	// of the currently magnified visuals. The source rectangle is the portion of the screen that is 
	// currently being magnified, and the destination rectangle is the area on the screen which shows the 
	// magnified results.

	// If we're setting an input transform, base the transform on the current fullscreen magnification.
	if (fSetInputTransform)
	{
		// Assume here the touch and pen input is going to the primary monitor.
		rcDest.right = GetSystemMetrics(SM_CXSCREEN);
		rcDest.bottom = GetSystemMetrics(SM_CYSCREEN);

		float magnificationFactor;
		int xOffset;
		int yOffset;

		// Get the currently active magnification.
		if (MagGetFullscreenTransform(&magnificationFactor, &xOffset, &yOffset))
		{
			// Determine the area of the screen being magnified.
			rcSource.left = xOffset;
			rcSource.top = yOffset;
			rcSource.right = rcSource.left + (int)(rcDest.right / magnificationFactor);
			rcSource.bottom = rcSource.top + (int)(rcDest.bottom / magnificationFactor);
		}
		else
		{
			// An unexpected error occurred trying to get the current magnification.
			fContinue = false;

			DWORD dwErr = GetLastError();

			WCHAR szError[256];
		}
	}

	if (fContinue)
	{
		// Now set the input transform as required.
		if (!MagSetInputTransform(fSetInputTransform, &rcSource, &rcDest))
		{
			DWORD dwErr = GetLastError();

			// If the last error is E_ACCESSDENIED, then this may mean that the process is not running with
			// UIAccess privileges. UIAccess is required in order for MagSetInputTransform() to success.

		}
	}
}
