// 2017/07/07 ray3
// 2019/06/25 ray4

#include <iostream>
using namespace std;
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
		BYTE*		bPixelBits;// = nullptr;
		int			width;
		int			height;
	} m;


	//------------------------------------------------------------------------------
	void ReleasePixelBits()
	//------------------------------------------------------------------------------
	{
		if ( m.bPixelBits != 0 ) 
		{
//			free( m.bPixelBits );
		}
	}
	//------------------------------------------------------------------------------
	void CreatePixelBits(int bpp, int width, int height )
	//------------------------------------------------------------------------------
	{
		
		m.bPixelBits	= (BYTE*) malloc( width * height * bpp/8 );
//		m.bPixelBits	= new BYTE( width * height * bpp/8 );			//	何故かハングアップ
		m.bpp			= bpp;
		m.width			= width;
		m.height		= height;
		
		BITMAPINFO bi;
		BITMAPINFOHEADER &bih	= m.bmpInfo.bmiHeader;
		bih.biSize				= sizeof(bih);
		bih.biWidth				= width;
		bih.biHeight			= height;
		bih.biPlanes			= 1;
		bih.biBitCount			= bpp;
		bih.biCompression		= BI_RGB;//BI_BITFIELDS;
		bih.biSizeImage			= 0;
		bih.biXPelsPerMeter		= 0;
		bih.biYPelsPerMeter		= 0;
		bih.biClrUsed			= 0;
		bih.biClrImportant		= 0;

	}

	//------------------------------------------------------------------------------
	void draw( HWND hWnd )
	//------------------------------------------------------------------------------
	{

		HDC hdc = GetDC( hWnd );
		RECT rect;
		GetClientRect( hWnd, &rect );
		StretchDIBits( hdc, 0, 0, rect.right, rect.bottom, 0, 0, m.width, m.height, m.bPixelBits, &m.bmpInfo, DIB_RGB_COLORS, SRCCOPY );
		if (1)
		{
			HPEN hPen, hOldPen;

			hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
			hOldPen = (HPEN)SelectObject(hdc, hPen);

			MoveToEx(hdc, 50, 50, NULL);
			LineTo(hdc, 100, 100);
			LineTo(hdc, 50, 100);
			SelectObject(hdc, hOldPen);

			DeleteObject(hPen);
		}
		ReleaseDC( hWnd, hdc );



	}

	//------------------------------------------------------------------------------
	void Paint( HWND hWnd )
	//------------------------------------------------------------------------------
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint( hWnd , &ps );
		if (0)
		{
			HPEN hPen, hOldPen;

			hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
			hOldPen = (HPEN)SelectObject(hdc, hPen);

			MoveToEx(hdc, 50, 50, NULL);
			LineTo(hdc, 100, 100);
			LineTo(hdc, 50, 100);
			SelectObject(hdc, hOldPen);

			DeleteObject(hPen);
		}
		EndPaint( hWnd , &ps);

		UpdateWindow( hWnd );
	}

} gdi;

//------------------------------------------------------------------------------
unsigned char* Win::getAddrPixels()
//------------------------------------------------------------------------------
{
	return( (unsigned char*)gdi.m.bPixelBits);
}

//------------------------------------------------------------------------------
static	LRESULT CALLBACK WinProc
//------------------------------------------------------------------------------
(
	  HWND		hWnd	// handle to window
	, UINT		uMsg	// message identifier
	, WPARAM	wParam // first message parameter
	, LPARAM	lParam // second message parameter
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
cout <<"paint" << endl;
				gdi.Paint( hWnd );
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
	gdi.ReleasePixelBits();

}
//------------------------------------------------------------------------------
Win::Win( int width, int height, const char* name  )
//------------------------------------------------------------------------------
{
	m.width		= width;
	m.height	= height;

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
		SetRect(&rc, 0, 0, width, height );
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
	

	gdi.CreatePixelBits( 24, width, height );
}
