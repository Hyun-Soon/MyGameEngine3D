#include "WindowManager.h"

WindowManager::WindowManager(UINT w, UINT h)
	: mResolution(w, h)
{
}

// LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	// switch (msg)
//	//{
//	//	case WM_SIZE:
//	//		break;
//	//	case WM_LBUTTONDOWN:
//	//		std::cout << "LBUTTONDOWN" << std::endl;
//	//		break;
//	// }
//	return DefWindowProc(hwnd, msg, wParam, lParam); // Default Window Procedure
// }

// bool WindowManager::Initialize()
//{
//	WNDCLASSEX wc = { sizeof(WNDCLASSEX), // size of structure
//		CS_CLASSDC,						  // style of window
//		WndProc,						  // points to window procedure
//		0L,
//		0L,
//		GetModuleHandle(NULL), // because used main function, not WinMain
//		NULL,
//		NULL,
//		NULL,
//		NULL,
//		L"MyWhatTheGolf", // name of window class
//		NULL };
//
//	if (!RegisterClassEx(&wc))
//	{
//		OutputDebugString(L"RegisterClassEx() failed.\n");
//		return 1;
//	}
//
//	RECT wr = { 0, 0, mResolution.width, mResolution.height };
//	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);
//
//	mWindow = CreateWindow(wc.lpszClassName,
//		L"MyWhatTheGolf",
//		WS_OVERLAPPEDWINDOW,
//		0,
//		0,
//		wr.right - wr.left,
//		wr.bottom - wr.top,
//		NULL,
//		NULL,
//		wc.hInstance,
//		NULL);
//
//	if (!mWindow)
//	{
//		OutputDebugString(L"CreateWindow() failed.\n");
//		return false;
//	}
//	mResolution.width = wr.right - wr.left;
//	mResolution.height = wr.bottom - wr.top;
//
//	ShowWindow(mWindow, SW_SHOWDEFAULT);
//	// UpdateWindow(mainWindow);
//	SetForegroundWindow(mWindow);
//
//	return true;
// }

const HWND& WindowManager::GetWindow() const
{
	return this->mWindow;
}

const Resolution& WindowManager::GetResolution() const
{
	return this->mResolution;
}
