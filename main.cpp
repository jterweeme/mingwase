#include <windows.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
wchar_t szClassName[] = L"WindowsApp";
HINSTANCE g_hinst;

class WinClass
{
	WNDCLASSEX wcl;
public:
	WinClass();
	void doRegister() { RegisterClassEx(&wcl); }
};

class Window
{
	HWND hwnd;
public:
	Window();
	void show(int n) { ShowWindow(hwnd, n); }
};

class Main
{
	int _msgPump();
public:
	int run(int n);
};

Window::Window()
{
	hwnd = CreateWindowEx(0, szClassName, L"Windows App",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 544, 375, HWND_DESKTOP, NULL, g_hinst, NULL);
}

WinClass::WinClass()
{
	wcl.hInstance = g_hinst;
	wcl.lpszClassName = szClassName;
	wcl.lpfnWndProc = WindowProc;
	wcl.style = CS_DBLCLKS;
	wcl.cbSize = sizeof (WNDCLASSEX);
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.lpszMenuName = NULL;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
}

int Main::_msgPump()
{
	MSG messages;

	while (GetMessage(&messages, NULL, 0, 0))
		DispatchMessage(&messages);

	return messages.wParam;
}

int Main::run(int n)
{
	WinClass wc;
	wc.doRegister();
	Window w;
	w.show(n);
	return _msgPump();
}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE, LPSTR, int n)
{
	g_hinst = hThisInstance;
	Main main;
	return main.run(n);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

