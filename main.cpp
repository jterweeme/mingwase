#include <windows.h>
#include <CommCtrl.h>
#include "resource.h"

class Toolbar
{
    HWND _tbar;
public:
    void init(HWND parent, HINSTANCE hinst);
};

class MDIClient
{
    HWND _mdiClient;
public:
    void create(HWND parent, HINSTANCE hinst);
    HWND hwnd() const { return _mdiClient; }
};

class WinClass
{
	WNDCLASSEX _wc;
public:
    WinClass() { }
    void init(HINSTANCE hinst, LPCWSTR className, WNDPROC cb, LPCWSTR menu, HBRUSH bg);
    void init(HINSTANCE hinst, LPCWSTR cname, WNDPROC winproc) { init(hinst, cname, winproc, 0, 0); }
    void init(HINSTANCE hinst, LPCWSTR className, WNDPROC cb, int menu, int bg) { init(hinst, className, cb, MAKEINTRESOURCE(menu), (HBRUSH)bg); }
    void init(WNDCLASSEX wc) { _wc = wc; }
	void doRegister() { RegisterClassEx(&_wc); }
    WNDCLASSEX wc() const { return _wc; }
    LPCWSTR className() const { return _wc.lpszClassName; }
};

class Window
{
	HWND _hwnd;
public:
	Window(HINSTANCE hinst, LPCWSTR className);
	void show(int n) { ShowWindow(_hwnd, n); }
};

class Main
{
    Main() { }
    HINSTANCE _hinst;
    Toolbar _tbar;
    HWND _sbar;
    LRESULT _wProc(HWND, UINT, WPARAM, LPARAM);
    WinClass _wc;
    WinClass _mc;
    MDIClient _mdic;
    int command(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    int create(HWND hwnd);
public:
    int msgPump();
    static Main *getInstance() { static Main instance; return &instance; }
    void hinst(HINSTANCE hinst) { _hinst = hinst; }
	static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK mdiProc(HWND, UINT, WPARAM, LPARAM);
    void init(HINSTANCE hinst, int n);
};

Window::Window(HINSTANCE hinst, LPCWSTR className)
{
	_hwnd = CreateWindowEx(0, className, L"Windows App", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 544, 375, 0, 0, hinst, 0);
}

void WinClass::init(HINSTANCE hinst, LPCWSTR className, WNDPROC cb, LPCWSTR menu, HBRUSH bg)
{
	_wc.hInstance = hinst;
	_wc.lpszClassName = className;
	_wc.lpfnWndProc = cb;
	_wc.style = CS_HREDRAW | CS_VREDRAW;
	_wc.cbSize = sizeof(WNDCLASSEX);
	_wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	_wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	_wc.lpszMenuName = menu;
	_wc.cbClsExtra = 0;
	_wc.cbWndExtra = 0;
	_wc.hbrBackground = bg;
}

void MDIClient::create(HWND parent, HINSTANCE hinst)
{
    CLIENTCREATESTRUCT ccs;

    _mdiClient = CreateWindowEx(WS_EX_CLIENTEDGE, L"mdiclient", NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, (HMENU)981, hinst, (LPVOID)&ccs);

    ShowWindow(_mdiClient, SW_SHOW);
}

int Main::msgPump()
{
	MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateMDISysAccel(_mdic.hwnd(), &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	return msg.wParam;
}

void Main::init(HINSTANCE hinst, int n)
{
    _wc.init(hinst, L"untitled", Main::wndProc, IDR_MENU1, COLOR_3DSHADOW + 1);
    _mc.init(hinst, L"child", Main::mdiProc, 0, COLOR_3DFACE + 1);
	_wc.doRegister();
    _mc.doRegister();
	Window w(hinst, _wc.className());
	w.show(n);
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int n)
{
    Main *main = Main::getInstance();
    main->hinst(hinst);
	main->init(hinst, n);
    return main->msgPump();
}

int Main::command(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	MDICREATESTRUCT mcs;
    HWND hChild;

    switch (LOWORD(wp))
    {
    case ID_FILE_NEW:
        mcs.szTitle = L"[Untitled]";
        mcs.szClass = _mc.className();
        mcs.hOwner = _hinst;
        mcs.x = mcs.cx = CW_USEDEFAULT;
        mcs.y = mcs.cy = CW_USEDEFAULT;
        mcs.style = MDIS_ALLCHILDSTYLES;
        hChild = (HWND)SendMessage(_mdic.hwnd(), WM_MDICREATE, 0, (LONG)&mcs);

        if (!hChild)
        {
            throw L"MDI Child creation failed.";
        }
		
		break;
    case ID_WINDOW_START:
        system("c:/tools/hxden/hxd.exe");
        break;
	case ID_FILE_EXIT:
		PostQuitMessage(0);
		break;
	}
	return 0;
}

void Toolbar::init(HWND parent, HINSTANCE hinst)
{
    TBADDBITMAP tbab;
    TBBUTTON tbb[9];
    _tbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, parent, (HMENU)IDR_TOOLBAR1, hinst, NULL);

    if (!_tbar)
        throw L"Toolbar creation failed.";

    SendMessage(_tbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    tbab.hInst = HINST_COMMCTRL;
    tbab.nID = IDB_STD_SMALL_COLOR;
    SendMessage(_tbar, TB_ADDBITMAP, 0, (LPARAM)&tbab);
    ZeroMemory(tbb, sizeof(tbb));
    tbb[0].iBitmap = STD_FILENEW;
    tbb[0].fsState = TBSTATE_ENABLED;
    tbb[0].fsStyle = TBSTYLE_BUTTON;
    tbb[0].idCommand = ID_FILE_NEW;
    SendMessage(_tbar, TB_ADDBUTTONS, 1, (LPARAM)&tbb);
}

int Main::create(HWND parent)
{
    _tbar.init(parent, _hinst);
    _sbar = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, parent, (HMENU)900, _hinst, NULL);
    _mdic.create(parent, _hinst);
    return 0;
}

LRESULT CALLBACK Main::mdiProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	return DefMDIChildProc(hwnd, msg, wp, lp);
}

LRESULT Main::_wProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        create(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        command(hwnd, msg, wp, lp);
        break;
    case WM_SIZE:
        SendMessage(_sbar, WM_SIZE, 0, 0);
        break;
    }

    return DefFrameProc(hwnd, _mdic.hwnd(), msg, wp, lp);
}

LRESULT CALLBACK Main::wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    LRESULT retval = 0;
    Main *main;
    main = Main::getInstance();
    try
    {
        retval = main->_wProc(hwnd, msg, wp, lp);
    }
    catch (LPCWSTR e)
    {
        ::MessageBox(hwnd, e, L"Uh oh", 0);
    }
    catch (...)
    {
    }

    return retval;
}

