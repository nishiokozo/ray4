// 2017/07/07 ray3
// 2019/06/25 ray4

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>       // sleep_for
#include <functional>
using namespace std;

#include <windows.h>

#include "SysWin.h"



static struct
{
	function<void()> funcOnCreate;
	function<void()> funcOnSize;
	function<void()> funcOnPaint;
	function<void()> funcOnDestroy;

	WNDCLASSEX		tWndClass;
	HINSTANCE		hInstance;
	const CHAR*		cpClassName;
	const CHAR*		cpWindowName;
	CHAR*			cpMenu;
	MSG				tMsg;

} g;


//------------------------------------------------------------------------------
void SysWin::SetOnPaint( function<void()> func )
//------------------------------------------------------------------------------
{
	g.funcOnPaint = func;
}

//------------------------------------------------------------------------------
void SysWin::SetOnSize( function<void()> func )
//------------------------------------------------------------------------------
{
	g.funcOnSize = func;
}

//------------------------------------------------------------------------------
void SysWin::SetOnDestroy( function<void()> func )
//------------------------------------------------------------------------------
{
	g.funcOnDestroy = func;
}

//------------------------------------------------------------------------------
void SysWin::SetOnCreate( function<void()> func )
//------------------------------------------------------------------------------
{
	g.funcOnCreate = func;
}

///------------------------------------------------------------------------------
static LRESULT CALLBACK WinProc
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
			g.funcOnCreate();
			return 0;

//		case WM_SHOWWINDOW:	//  ShowWindowと同時に発行される ShowWindow()  -> WM_SHOWWINDOW -> WM_ACTIVATE ->  WM_ERASEBKGND -> WM_SIZE -> WM_PAINT
//			cout << "WM_SHOWWINDOW " << endl;
//			return 0;
//
//		case WM_ACTIVATE:
//			return 0;

		case WM_ERASEBKGND:	//	WM_PAINTイベントの途中、及びWM_SHOWWINDOWのあとに発行される。 DefWindowProc()に任せると白いフラッシュが入ってしまうので、0を返す
			//cout << "WM_ERASEBKGND " << endl;
			return 0;
			
		case WM_SIZE:	// 画面サイズが決定された時に発行される
			//cout << "WM_SIZE " << endl;
			g.funcOnSize();
			return 0;

		case WM_PAINT:	// OSからの描画要求。再描画区域情報（ウィンドウが重なっている際などの）が得られるタイミング。
			//cout << "WM_PAINT " << endl;
			g.funcOnPaint();

			return 0;

		case WM_KEYDOWN:
			if ( wParam == VK_ESCAPE ) SendMessage(hWnd , WM_DESTROY , 0 , 0);	
			return 0;

		case WM_DESTROY:	//[x]を押すなどしたとき
			g.funcOnDestroy();
			cout << "WM_DESTROY " << endl;
			return 0;
	}

	// デフォルト処理呼び出し。
	return DefWindowProc( hWnd, uMsg, wParam, lParam );// [x] > WM_SYSCOMMAND > WM_CLOSE > WM_DESTROY
}
//------------------------------------------------------------------------------
SysWin&	SysWin::GetInstance()
//------------------------------------------------------------------------------
{
	static SysWin	instance;
	return instance;
}
//------------------------------------------------------------------------------
SysWin::~SysWin()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
SysWin::SysWin()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void SysWin::OpenWindow( const char* windowname, int pos_x, int pos_y, int width, int height  )
//------------------------------------------------------------------------------
{
	const char* name = "kozo:SysWin";

	// アプリケーションインスタンス
	g.hInstance		= GetModuleHandle( NULL );


	// クラス名称
	g.cpClassName	= "MainWindowClass";

	// メニュー
	g.cpMenu			= MAKEINTRESOURCE( NULL );

	// ウインドウ名称
	g.cpWindowName = name;

	// ウインドウクラスパラメータセット
	g.tWndClass.cbSize		= sizeof( WNDCLASSEX );
	g.tWndClass.style			= CS_HREDRAW | CS_VREDRAW;
	g.tWndClass.lpfnWndProc	= WinProc;
	g.tWndClass.cbClsExtra	= 0;	// GetClassLong で取得可能なメモリ
	g.tWndClass.cbWndExtra	= 0;	// GetWindowLong で取得可能なメモリ
	g.tWndClass.hInstance		= g.hInstance;
	g.tWndClass.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	g.tWndClass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	g.tWndClass.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
	g.tWndClass.lpszMenuName	= g.cpMenu;
	g.tWndClass.lpszClassName = g.cpClassName;
	g.tWndClass.hIconSm		= NULL;

	// ウインドウクラス生成
	if ( 0 == RegisterClassEx( &g.tWndClass ) ) 
	{
	}

	// 仮ウインドウを生成する
	{
		int valWin = WS_OVERLAPPEDWINDOW;
		win.hWnd = CreateWindowEx(
			 0
			, g.tWndClass.lpszClassName
			, name 
			, valWin
			, 0,0,256,256
			, NULL			  	// handle to parent or owner window
			, NULL			 	// handle to menu, or child-window identifier
			, g.hInstance	 	// handle to application instance
			, 0					// pointer to window-creation data
		);

	}

	m.x			= pos_x;
	m.y			= pos_y;
	m.width		= width;
	m.height	= height;

	// ウィンドウ名を変える
	SetWindowText(win.hWnd , windowname );


	// ウィンドウ位置サイズを変える
	{
		RECT rc;
		SetRect(&rc, 0, 0, width, height );
		AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, FALSE, 0);

		SetWindowPos(
			  win.hWnd 
			, HWND_TOPMOST	// 最前面ウィンドウ
			, pos_x + rc.left	
			, pos_y + rc.top	
			, rc.right-rc.left 	
			, rc.bottom-rc.top	
			,0
		);
	}
	
	// ウィンドウを表示する
	ShowWindow( win.hWnd, SW_SHOW );

}

//------------------------------------------------------------------------------
bool SysWin::Update()
//------------------------------------------------------------------------------
{
	InvalidateRect(win.hWnd , NULL , TRUE);	//	WM_PAINTを発行し再描画矩形情報を渡す

	while(1)
	{
		while ( PeekMessage( &g.tMsg, NULL, 0, 0, PM_REMOVE) )
		{
			DispatchMessage( &g.tMsg );
			TranslateMessage( &g.tMsg );
			if ( g.tMsg.message == WM_QUIT ) return false; 
		}

		return true;
	}
}
