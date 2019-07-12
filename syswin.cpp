// 2017/07/07 ray3
// 2019/06/25 ray4

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>       // sleep_for
#include <functional>
using namespace std;

#include <windows.h>

#include "syswin.h"

//#include "sysgdi.h"

#define	USE_RIALTIME_PAINT 0

static chrono::system_clock::time_point time_a;
static chrono::system_clock::time_point time_b;

//extern SysGdi gdi;



static 	function<void(HWND)> g_funcOnShowwindow;
static 	function<void(HWND)> g_funcOnSize;
static 	function<void(HWND)> g_funcOnPaint;
static 	function<void(HWND)> g_funcOnDestroy;

//------------------------------------------------------------------------------
void SysWin::SetOnPaint( function<void(HWND)> func )
//------------------------------------------------------------------------------
{
	g_funcOnPaint = func;
}
//------------------------------------------------------------------------------
void SysWin::SetOnSize( function<void(HWND)> func )
//------------------------------------------------------------------------------
{
	g_funcOnSize = func;
}

//------------------------------------------------------------------------------
void SysWin::SetOnDestroy( function<void(HWND)> func )
//------------------------------------------------------------------------------
{
	g_funcOnDestroy = func;
}

//------------------------------------------------------------------------------
void SysWin::SetOnShowwindow( function<void(HWND)> func )
//------------------------------------------------------------------------------
{
	g_funcOnShowwindow = func;
}

///------------------------------------------------------------------------------
LRESULT CALLBACK SysWin::WinProc
//------------------------------------------------------------------------------
(
	  HWND		hWnd
	, UINT		uMsg
	, WPARAM	wParam	//メッセージの付加情報
	, LPARAM	lParam	//メッセージの付加情報
)
{
//		■CreateWindowEx() シーケンス
//		-- 0x24		>	WM_GETMINMAXINFO
//		-- 0x81			WM_NCCREATE
//		-- 0x83			WM_NCCALCSIZE
//		-- 0x1		>	WM_CREATE
//	
//		■ShowWindow() シーケンス
//		-- 0x18		>	WM_SHOWWINDOW
//		-- 0x46			WM_WINDOWPOSCHANGING
//		-- 0x46			WM_WINDOWPOSCHANGING
//		-- 0x1c			WM_ACTIVATEAPP
//		-- 0x86			WM_NCACTIVATE
//		-- 0x7f			WM_GETICON
//		-- 0x7f			WM_GETICON
//		-- 0x7f			WM_GETICON
//		-- 0x6		>	WM_ACTIVATE
//		-- 0x282		WM_IME_NOTIFY
//		-- 0xc053		???
//		-- 0x281		WM_IME_SETCONTEXT
//		-- 0x282		WM_IME_NOTIFY
//		-- 0x7			WM_SETFOCUS
//		-- 0x85			WM_NCPAINT
//		-- 0x14		>	WM_ERASEBKGND
//		-- 0x47			WM_WINDOWPOSCHANGED
//		-- 0x5		>	WM_SIZE
//		-- 0x3		>	WM_MOVE
//	
//		■SendMessage(WM_DESTROY)シーケンス
//		-- 0x2		>	WM_DESTROY
//		-- 0x102		WM_CHAR
//	
//		■[x]クリックシーケンス
//		:
//		-- 0x14		>	WM_ERASEBKGND
//		-- 0x20			WM_SETCURSOR
//		-- 0xa1			WM_NCLBUTTONDOWN
//		-- 0x7f			WM_GETICON
//		-- 0x7f			WM_GETICON
//		-- 0x215		WM_CAPTURECHANGED
//		-- 0x112		WM_SYSCOMMAND
//		-- 0x10		>	WM_CLOSE
//		-- 0x90			??
//		-- 0x46			WM_WINDOWPOSCHANGING
//		-- 0x47			WM_WINDOWPOSCHANGED
//		-- 0x86			WM_NCACTIVATE
//		-- 0x6		>	WM_ACTIVATE
//		-- 0x1c			WM_ACTIVATEAPP
//		-- 0x8			WM_KILLFOCUS
//		-- 0x281		WM_IME_SETCONTEXT
//		-- 0x282		WM_IME_NOTIFY
//		-- 0x2		>	WM_DESTROY
//		-- 0x82			WM_NCDESTROY

//cout << hex << "-- 0x" << uMsg << endl;
	switch( uMsg ) 
	{
		case WM_CREATE:	// CreateWindowと同時に発行される
cout << "WM_CREATE " << endl;
			break;

		case WM_SHOWWINDOW:	//  ShowWindowと同時に発行される ShowWindow()  -> WM_SHOWWINDOW -> WM_ACTIVATE ->  WM_ERASEBKGND -> WM_SIZE -> WM_PAINT
cout << "WM_SHOWWINDOW " << endl;
			g_funcOnShowwindow( hWnd );
			return 0;

		case WM_ACTIVATE:
			return 0;

		case WM_ERASEBKGND:	//	WM_PAINTイベントの途中、及びWM_SHOWWINDOWのあとに発行される。 DefWindowProc()に任せると白いフラッシュが入ってしまうので、0を返す
//cout << "WM_ERASEBKGND " << endl;
			return 0;
			
		case WM_SIZE:	// 画面サイズが決定された時に発行される
cout << "WM_SIZE " << endl;
			g_funcOnSize( hWnd);
			return 0;

		case WM_PAINT:	// OSからの描画要求。再描画区域情報（ウィンドウが重なっている際などの）が得られるタイミング。
//cout << "WM_PAINT " << endl;
			g_funcOnPaint( hWnd );

			return 0;

		case WM_KEYDOWN:
			if ( wParam == VK_ESCAPE ) SendMessage(hWnd , WM_DESTROY , 0 , 0);	
			return 0;

		case WM_DESTROY:	//[x]を押すなどしたとき
			g_funcOnDestroy( hWnd );
cout << "WM_DESTROY " << endl;
			PostQuitMessage( 0 );
			return 0;
	}
//			return 1;

	// デフォルト処理呼び出し。
	return DefWindowProc( hWnd, uMsg, wParam, lParam );// [x] > WM_SYSCOMMAND > WM_CLOSE > WM_DESTROY
}
//------------------------------------------------------------------------------
SysWin::~SysWin()
//------------------------------------------------------------------------------
{
//	gdi.ReleasePixelBits();

}

//------------------------------------------------------------------------------
SysWin::SysWin()
//------------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------------
void SysWin::OpenWindow( const char* name, int pos_x, int pos_y, int width, int height  )
//------------------------------------------------------------------------------
{
	// アプリケーションインスタンス
	win.hInstance		= GetModuleHandle( NULL );


	// クラス名称
	win.cpClassName	= "MainWindowClass";

	// メニュー
	win.cpMenu			= MAKEINTRESOURCE( NULL );

	// ウインドウ名称
	win.cpWindowName = name;

	// ウインドウクラスパラメータセット
	win.tWndClass.cbSize		= sizeof( WNDCLASSEX );
	win.tWndClass.style			= CS_HREDRAW | CS_VREDRAW;
	win.tWndClass.lpfnWndProc	= WinProc;
	win.tWndClass.cbClsExtra	= 0;	// GetClassLong で取得可能なメモリ
	win.tWndClass.cbWndExtra	= 0;	// GetWindowLong で取得可能なメモリ
	win.tWndClass.hInstance		= win.hInstance;
	win.tWndClass.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	win.tWndClass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	win.tWndClass.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
	win.tWndClass.lpszMenuName	= win.cpMenu;
	win.tWndClass.lpszClassName = win.cpClassName;
	win.tWndClass.hIconSm		= NULL;

	// ウインドウクラス生成
	if ( 0 == RegisterClassEx( &win.tWndClass ) ) 
	{
	}
	
	m.x			= pos_x;
	m.y			= pos_y;
	m.width		= width;
	m.height	= height;


cout << "1 ----------------- " << endl;

	// ウインドウを生成する
	{
		RECT rc;
		SetRect(&rc, 0, 0, width, height );
		int valWin = WS_OVERLAPPEDWINDOW;
		AdjustWindowRectEx(&rc, valWin, FALSE, 0);

		win.hWnd = CreateWindowEx(
			 0
			, win.tWndClass.lpszClassName
			, name 
			, valWin
			, pos_x + rc.left	
			, pos_y + rc.top	
			, rc.right-rc.left 	
			, rc.bottom-rc.top	
			, NULL			  	// handle to parent or owner window
			, NULL			 	// handle to menu, or child-window identifier
			, win.hInstance	 	// handle to application instance
			, 0					// pointer to window-creation data
		);

	}
cout << "2 ----------------- " << endl;
	ShowWindow( win.hWnd, SW_SHOW );
cout << "3 ----------------- " << endl;

}

//------------------------------------------------------------------------------
bool SysWin::Update()
//------------------------------------------------------------------------------
{
//cout << "-- " << endl;
#if USE_RIALTIME_PAINT
#else
		InvalidateRect(win.hWnd , NULL , TRUE);	//	WM_PAINTを発行し再描画矩形情報を渡す
#endif

	while(1)
	{
		while ( PeekMessage( &win.tMsg, NULL, 0, 0, PM_REMOVE) )
		{
			DispatchMessage( &win.tMsg );
			TranslateMessage( &win.tMsg );
			if ( win.tMsg.message == WM_QUIT ) return false; 
		}
#if USE_RIALTIME_PAINT
		HDC hDc = GetDC( win.hWnd );
		gdi.paint0( gdi.hdcBackbuffer);
		RECT rc;
		GetClientRect( win.hWnd, &rc );
	    BitBlt(hDc, 0, 0, rc.right, rc.bottom, gdi.hdcBackbuffer, 0, 0, SRCCOPY);
		ReleaseDC( win.hWnd, hDc );
#endif

		return true;
	}
}
