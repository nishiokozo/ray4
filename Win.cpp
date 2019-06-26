// 2017/07/07 ray3
// 2019/06/25 ray4

#include <stdio.h>
#include <windows.h>

#include "Win.h"


struct	//	WIN
{
	WNDCLASSEX		tWndClass;
	HINSTANCE		hInstance;
	const CHAR*		cpClassName;
	const CHAR*		cpWindowName;
	CHAR*			cpMenu;
	MSG				tMsg;
	HWND			hWnd;
} win;

struct	//	GDI
{
	struct
	{
		BITMAPINFO	bmpInfo;
		int			bpp;
		BYTE*		bPixelBits;
		int			width;
		int			height;
	}l;

	//------------------------------------------------------------------------------
	void create(int bpp, int width, int height )
	//------------------------------------------------------------------------------
	{
		l.bPixelBits	= (BYTE*) malloc( width * height * bpp/8 );
		l.bpp			= bpp;
		l.width			= width;
		l.height		= height;
		
		BITMAPINFO bi;
		BITMAPINFOHEADER &bih	= l.bmpInfo.bmiHeader;
		bih.biSize			= sizeof(bih);
		bih.biWidth			= width;
		bih.biHeight		= height;
		bih.biPlanes		= 1;
		bih.biBitCount		= bpp;
		bih.biCompression	= BI_RGB;//BI_BITFIELDS;
		bih.biSizeImage		= 0;
		bih.biXPelsPerMeter	= 0;
		bih.biYPelsPerMeter	= 0;
		bih.biClrUsed		= 0;
		bih.biClrImportant	= 0;

	}

	//------------------------------------------------------------------------------
	void draw( HWND hWnd )
	//------------------------------------------------------------------------------
	{

		HDC hdc = GetDC( hWnd );
		RECT rect;
		GetClientRect( hWnd, &rect );
		StretchDIBits( hdc, 0, 0, rect.right, rect.bottom, 0, 0, l.width, l.height, l.bPixelBits, &l.bmpInfo, DIB_RGB_COLORS, SRCCOPY );
		ReleaseDC( hWnd, hdc );

	}

} gdi;

//------------------------------------------------------------------------------
unsigned char* Win::getAddrPixels()
//------------------------------------------------------------------------------
{
	return( (unsigned char*)gdi.l.bPixelBits);
}

//------------------------------------------------------------------------------
static	LRESULT CALLBACK WinProc
//------------------------------------------------------------------------------
(
	  HWND	hWnd	// handle to window
	, UINT	uMsg	// message identifier
	, WPARAM wParam // first message parameter
	, LPARAM lParam // second message parameter
)
{


	switch( uMsg ) 
	{
		case WM_CREATE:

			// ウインドウを表示する
			ShowWindow( hWnd, SW_SHOW );
			break;

		case WM_PAINT:
			{
				{
					PAINTSTRUCT ps;
					HDC hdc = BeginPaint( hWnd , &ps );
					//if ( g_callbackPaint ) g_callbackPaint( hdc );
					EndPaint( hWnd , &ps);
				}
				UpdateWindow( hWnd );

				return 0;
			}

		case WM_KEYDOWN:
			{
				if ( wParam == 27 ) // ESC
				{
					PostQuitMessage( 0 );
				}
				break;
			}

		case WM_DESTROY:

			PostQuitMessage( 0 );
			break;
	}

	// デフォルト処理呼び出し
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//------------------------------------------------------------------------------
bool Win::exec()
//------------------------------------------------------------------------------
{
	while(1)
	{
		while ( PeekMessage( &win.tMsg, NULL, 0, 0, PM_REMOVE) )
		{
			DispatchMessage( &win.tMsg );
			TranslateMessage( &win.tMsg );
			if ( win.tMsg.message == WM_QUIT ) break;
		}
		if ( win.tMsg.message == WM_QUIT ) break;

		gdi.draw( win.hWnd );

		return true;
	}
	return	false;
}

//------------------------------------------------------------------------------
Win::~Win()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
Win::Win( int w, int h, const char* name  )
//------------------------------------------------------------------------------
{
	m_width=w;
	m_height=h;

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

	// ウインドウを生成する
	{
		RECT rc;
		SetRect(&rc, 0, 0, m_width, m_height );
		int valWin = WS_OVERLAPPEDWINDOW;
		AdjustWindowRectEx(&rc, valWin, FALSE, 0);

		win.hWnd = CreateWindowEx(
			  0
			, win.tWndClass.lpszClassName//m_classname//wc.lpszClassName
			, name 
			, valWin
			, 800+rc.left	  	// horizontal position of window
			, 100+rc.top		// vertical position of window
			, rc.right-rc.left 	// window width
			, rc.bottom-rc.top	// window height
			, NULL			  	// handle to parent or owner window
			, NULL			 	// handle to menu, or child-window identifier
			, win.hInstance	 	// handle to application instance
			, 0					// pointer to window-creation data
		);

	}
	
	gdi.create( 24, m_width, m_height );
}
