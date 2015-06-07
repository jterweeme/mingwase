#include <windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
HINSTANCE g_hinst;
wchar_t g_appname[] = L"Onzin";

class WinClass
{
	WNDCLASSEX wc;
public:
	WinClass();
	void doRegister() { RegisterClassEx(&wc); }
};

class Window
{
	HWND _hwnd;
public:
	Window();
	void show() { ::ShowWindow(_hwnd, SW_SHOW); }
	void update() { ::UpdateWindow(_hwnd); }
};

class MingWase
{
public:
	int run();
};

Window::Window()
{
	_hwnd = CreateWindowEx(0, g_appname, L"Windows App", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 544, 375, HWND_DESKTOP, NULL, g_hinst, NULL);
}

WinClass::WinClass()
{
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = g_hinst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = g_appname;
	wc.hbrBackground = (HBRUSH)(COLOR_3DSHADOW + 1);
	wc.lpszMenuName = L"MAIN";
}

int MingWase::run()
{
	WinClass wc;
	wc.doRegister();
	MessageBox(NULL, L"Bericht", L"Bericht", 0);
	Window w;
	w.show();
	w.update();
	MSG msg;
	int status;

	while ((status = ::GetMessage(&msg, 0, 0, 0)) != 0)
		::DispatchMessage(&msg);

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return 0;
}

int WINAPI WinMain(HINSTANCE h, HINSTANCE p, char *c, int n)
{
	MingWase mw;
    return mw.run();
}


