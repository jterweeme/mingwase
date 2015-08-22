#include <windows.h>
#include <CommCtrl.h>
#include "resource.h"

class Handle
{
protected:
    HWND _hwnd;
public:
    HWND hwnd() const { return _hwnd; }
};

class Toolbar : public Handle
{
public:
    void init(HWND parent, HINSTANCE hinst);
};

class MDIClient : public Handle
{
public:
    void create(HWND parent, HINSTANCE hinst);
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
    void create(HINSTANCE hinst, LPCWSTR cname);
	void show(int n) { ShowWindow(_hwnd, n); }
    void update() { UpdateWindow(_hwnd); }
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
    Window _mw;
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

void Window::create(HINSTANCE hinst, LPCWSTR cname)
{
    _hwnd = CreateWindowEx(0, cname, L"Windows App", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 544, 375, 0, 0, hinst, 0);
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

    _hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"mdiclient", NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, (HMENU)981, hinst, (LPVOID)&ccs);

    ShowWindow(_hwnd, SW_SHOW);
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
    _mw.create(hinst, _wc.className());
	_mw.show(n);
    _mw.update();
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
    _hwnd = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, parent, (HMENU)IDR_TOOLBAR1, hinst, NULL);

    if (!_hwnd)
        throw L"Toolbar creation failed.";

    SendMessage(_hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    tbab.hInst = HINST_COMMCTRL;
    tbab.nID = IDB_STD_SMALL_COLOR;
    SendMessage(_hwnd, TB_ADDBITMAP, 0, (LPARAM)&tbab);
    ZeroMemory(tbb, sizeof(tbb));
    tbb[0].iBitmap = STD_FILENEW;
    tbb[0].fsState = TBSTATE_ENABLED;
    tbb[0].fsStyle = TBSTYLE_BUTTON;
    tbb[0].idCommand = ID_FILE_NEW;
    tbb[1].iBitmap = STD_FILEOPEN;
    tbb[1].fsState = TBSTATE_ENABLED;
    tbb[1].fsStyle = TBSTYLE_BUTTON;
    tbb[1].idCommand = ID_FILE_OPEN;
    tbb[2].iBitmap = STD_FILESAVE;
    tbb[2].fsStyle = TBSTYLE_BUTTON;
    tbb[2].idCommand = ID_FILE_SAVE;
    tbb[3].fsStyle = TBSTYLE_SEP;
    tbb[4].iBitmap = STD_CUT;
    tbb[4].fsStyle = TBSTYLE_BUTTON;
    tbb[4].idCommand = ID_EDIT_CUT;
    tbb[5].iBitmap = STD_COPY;
    tbb[5].fsStyle = TBSTYLE_BUTTON;
    tbb[5].idCommand = ID_EDIT_COPY;
    tbb[6].iBitmap = STD_PASTE;
    tbb[6].fsStyle = TBSTYLE_BUTTON;
    tbb[6].idCommand = ID_EDIT_PASTE;
    tbb[7].fsStyle = TBSTYLE_SEP;
    tbb[8].iBitmap = STD_UNDO;
    tbb[8].fsStyle = TBSTYLE_BUTTON;
    //tbb[8].idCommand = ID_EDIT_UNDO;
    SendMessage(_hwnd, TB_ADDBUTTONS, 9, (LPARAM)&tbb);
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
    RECT rectClient, rectStatus, rectTool;
    UINT uToolHeight, uStatusHeight, uClientAlreaHeight;
    GetClientRect(hwnd, &rectClient);

    switch (msg)
    {
    case WM_CREATE:
        create(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_COMMAND:
        command(hwnd, msg, wp, lp);
        break;
    case WM_SIZE:
        SendMessage(_tbar.hwnd(), TB_AUTOSIZE, 0, 0);
        SendMessage(_sbar, WM_SIZE, 0, 0);
        //MoveWindow(_mdic.hwnd(), 0, uToolHeight, rectClient.right, uClientAlreaHeight - uStatusHeight - uToolHeight, TRUE);
        MoveWindow(_mdic.hwnd(), 0, 30, rectClient.right - 2, rectClient.bottom - 50, TRUE);
        break;
    default:
        return DefFrameProc(hwnd, _mdic.hwnd(), msg, wp, lp);
    }

    return 0;
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

