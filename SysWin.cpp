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
	function<void( int width, int height)> funcOnSize;
	function<void( int width, int height)> funcOnMove;
	function<void()> funcOnPaint;
	function<void()> funcOnDestroy;

	WNDCLASSEX		wndclass;
	HINSTANCE		hInstance;
	const CHAR*		nameClass;
	const CHAR*		nameWindow;
	CHAR*			strMenu;
	MSG				msg;

	RECT rect;

	int pos_x;
	int	pos_y;
	int width;
	int	height;
	
	int	wheelAccum;	//	蓄積用
	int	wheelResult;	//	結果出力用

} g;

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
			RegisterTouchWindow( hWnd, TWF_WANTPALM );//WM_TOUCH有効、迅速反応、1回タッチで1マウスクリック相当に是正される。必須。
			return 0;

//		case WM_TOUCH:
//			cout << "WM_TOUCH " << endl;
//			return 0;

		case WM_ERASEBKGND:	//	WM_PAINTイベントの途中、及びWM_SHOWWINDOWのあとに発行される。 DefWindowProc()に任せると白いフラッシュが入ってしまうので、0を返す
			//cout << "WM_ERASEBKGND " << endl;
			return 0;
			
		case WM_SIZE:	// 画面サイズが決定された時に発行される（初期表示含む）
//			cout << "WM_SIZE " << endl;
			GetClientRect( hWnd, &g.rect );
//cout << "1onsize " << g.rect.right << " " << g.rect.bottom << endl;

			g.funcOnSize( g.rect.right, g.rect.bottom );
			return 0;

		case WM_MOVE:	// 画面位置が決定された時に発行される（初期表示含む）
//			cout << "WM_MOVE " << endl;
			{
				RECT	r;
				SetRect(&r, 0, 0, 0, 0 );
				AdjustWindowRectEx(&r, WS_OVERLAPPEDWINDOW, FALSE, 0);
//	cout << r.left << " "  << r.top << " " << endl;

				GetWindowRect( hWnd, &g.rect );
//	cout << g.rect.left << " "  << g.rect.top << " " << endl;
//	cout << g.rect.left - r.left << " "  << g.rect.top - r.top << " " << endl;

				int x = g.rect.left - r.left;	//	描画区域の左上原点座標
				int y = g.rect.top - r.top;		//	描画区域の左上原点座標

				g.pos_x = x;
				g.pos_y = y;

				g.funcOnMove( x, y );
			}
			return 0;
		
		case WM_MOUSEWHEEL:
			{
				int fwKeys = GET_KEYSTATE_WPARAM(wParam);
				int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
//				if ( fwKeys & MK_SHIFT ) cout << "shift" ;
//				if ( fwKeys & MK_CONTROL ) cout << "ctrl" ;
				//fwkeys:4 = shift
				//fwkeys:8 = ctrl
//				cout <<" zDelta" <<  zDelta << endl;
				g.wheelAccum += zDelta;

			}
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
void SysWin::SetOnPaint( function<void()> func )
//------------------------------------------------------------------------------
{
	g.funcOnPaint = func;
}

//------------------------------------------------------------------------------
void SysWin::SetOnSize( function<void( int width, int height )> func )
//------------------------------------------------------------------------------
{
	g.funcOnSize = func;
}

//------------------------------------------------------------------------------
void SysWin::SetOnMove( function<void( int pos_x, int pos_y )> func )
//------------------------------------------------------------------------------
{
	g.funcOnMove = func;
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

//------------------------------------------------------------------------------
int SysWin::GetWheel()
//------------------------------------------------------------------------------
{
	return g.wheelResult;
}
//------------------------------------------------------------------------------
int SysWin::GetPosX()
//------------------------------------------------------------------------------
{
	return g.pos_x;
}
//------------------------------------------------------------------------------
int SysWin::GetPosY()
//------------------------------------------------------------------------------
{
	return g.pos_y;
}
//------------------------------------------------------------------------------
int SysWin::GetWidth()
//------------------------------------------------------------------------------
{
	return g.width;
}
//------------------------------------------------------------------------------
int SysWin::GetHeigit()
//------------------------------------------------------------------------------
{
	return g.height;
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
	g.nameClass	= "MainWindowClass";

	// メニュー
	g.strMenu			= MAKEINTRESOURCE( NULL );

	// ウインドウ名称
	g.nameWindow = name;

	// ウインドウクラスパラメータセット
	g.wndclass.cbSize			= sizeof( WNDCLASSEX );
	g.wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	g.wndclass.lpfnWndProc		= WinProc;
	g.wndclass.cbClsExtra		= 0;	// GetClassLong で取得可能なメモリ
	g.wndclass.cbWndExtra		= 0;	// GetWindowLong で取得可能なメモリ
	g.wndclass.hInstance		= g.hInstance;
	g.wndclass.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	g.wndclass.hCursor			= LoadCursor( NULL, IDC_ARROW );
	g.wndclass.hbrBackground	= (HBRUSH)( COLOR_WINDOW + 1 );
	g.wndclass.lpszMenuName		= g.strMenu;
	g.wndclass.lpszClassName	= g.nameClass;
	g.wndclass.hIconSm			= NULL;

	// ウインドウクラス生成
	if ( 0 == RegisterClassEx( &g.wndclass ) ) 
	{
	}

	// 仮ウインドウを生成する
	{
		int valWin = WS_OVERLAPPEDWINDOW;
		win.hWnd = CreateWindowEx(
			 0
			, g.wndclass.lpszClassName
			, name 
			, valWin
			, 0,0,256,256
			, NULL			  	// handle to parent or owner window
			, NULL			 	// handle to menu, or child-window identifier
			, g.hInstance	 	// handle to application instance
			, 0					// pointer to window-creation data
		);

	}

	g.pos_x		= pos_x;
	g.pos_y		= pos_y;
	g.width		= width;
	g.height	= height;

	// ウィンドウ名を変える
	SetWindowText(win.hWnd , windowname );


	// ウィンドウ位置サイズを変える
	{
		RECT rect;
		SetRect(&rect, 0, 0, width, height );
		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

		SetWindowPos(
			  win.hWnd 
			, HWND_TOPMOST	// 最前面ウィンドウ
			, pos_x + rect.left	
			, pos_y + rect.top	
			, rect.right-rect.left 	
			, rect.bottom-rect.top	
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
		while ( PeekMessage( &g.msg, NULL, 0, 0, PM_REMOVE) )
		{
			DispatchMessage( &g.msg );
			TranslateMessage( &g.msg );
			if ( g.msg.message == WM_QUIT ) return false; 
		}

		g.wheelResult = g.wheelAccum;
		g.wheelAccum = 0;

		return true;
	}


}
