#pragma once

#include <Windows.h>

struct Resolution
{
	UINT width;
	UINT height;
	Resolution(UINT w, UINT h) { this->width = w, this->height = h; }
};

class WindowManager
{
public:
	WindowManager(UINT w, UINT h);

	bool Initialize();

	const HWND&		  GetWindow() const;
	const Resolution& GetResolution() const;

private:
	Resolution mResolution;
	HWND	   mWindow;
};