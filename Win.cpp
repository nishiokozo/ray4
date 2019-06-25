// 2017/07/07 ray3
// 2019/06/25 ray4

#include <stdio.h>
#include <windows.h>

#include "Win.h"


struct	WinBuf
{
	WNDCLASSEX		tWndClass;
	HINSTANCE		hInstance;
	const CHAR*		cpClassName;
	const CHAR*		cpWindowName;
	CHAR*			cpMenu;
	MSG				tMsg;
	HWND			m_hWnd;
	//--
	BITMAPINFO			GDI_bmpInfo;
	BITMAPFILEHEADER	GDI_bmpFileHeader;
	int					GDI_bpp;
	BYTE*				GDI_bPixelBits;
}g;


//------------------------------------------------------------------------------
void Win::GDI_draw()
//------------------------------------------------------------------------------
{

	HDC hdc = GetDC(g.m_hWnd);
	RECT rect;
	GetClientRect(g.m_hWnd, &rect);
	StretchDIBits(hdc, 0, 0, rect.right, rect.bottom, 0, 0, m_width, m_height, g.GDI_bPixelBits, &g.GDI_bmpInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(g.m_hWnd,hdc);

}
//------------------------------------------------------------------------------
void Win::GDI_create(int bpp )
//------------------------------------------------------------------------------
{
	g.GDI_bPixelBits = (BYTE*) malloc( m_width*m_height*bpp/8 );
	g.GDI_bpp=bpp;
	BITMAPINFO bi;
	BITMAPINFOHEADER &bih=g.GDI_bmpInfo.bmiHeader;
	bih.biSize			= sizeof(bih);
	bih.biWidth			= m_width;
	bih.biHeight		= m_height;
	bih.biPlanes		= 1;
	bih.biBitCount		= g.GDI_bpp;
	bih.biCompression	= BI_RGB;//BI_BITFIELDS;
	bih.biSizeImage		= 0;
	bih.biXPelsPerMeter	= 0;
	bih.biYPelsPerMeter	= 0;
	bih.biClrUsed		= 0;
	bih.biClrImportant	= 0;

}
//------------------------------------------------------------------------------
unsigned char* Win::getAddrPixels()
//------------------------------------------------------------------------------
{
	return( (unsigned char*)g.GDI_bPixelBits);
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

			/* ウインドウを表示する */
			ShowWindow( hWnd, SW_SHOW );
			break;

		case WM_PAINT:
			{
				static LARGE_INTEGER nFreq, time1, time2;
				QueryPerformanceFrequency(&nFreq);
				{

					PAINTSTRUCT ps;
					HDC hdc = BeginPaint(hWnd , &ps);

//					if ( g_callbackPaint ) g_callbackPaint( hdc );

					EndPaint(hWnd , &ps);
				}


				QueryPerformanceCounter(&time2);

				double time = ((double)(time2.QuadPart - time1.QuadPart) / (double)nFreq.QuadPart);
				QueryPerformanceCounter(&time1);

//		printf( "paint %f %d %d \n", time*1000, (int)nFreq.QuadPart, (int)(time2.QuadPart - time1.QuadPart) );
				UpdateWindow(hWnd);

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

	/* デフォルト処理呼び出し */
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//------------------------------------------------------------------------------
bool Win::exec()
//------------------------------------------------------------------------------
{
	while(1)
	{
		while ( PeekMessage( &g.tMsg, NULL, 0, 0, PM_REMOVE) )
		{
			DispatchMessage(&g.tMsg);
			TranslateMessage(&g.tMsg);
			if (g.tMsg.message == WM_QUIT) break;
		}
		if (g.tMsg.message == WM_QUIT) break;

		GDI_draw();;

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

	/* アプリケーションインスタンス */
	g.hInstance		= GetModuleHandle( NULL );


	/* クラス名称 */
	g.cpClassName	= "MainWindowClass";

	/* メニュー */
	g.cpMenu			= MAKEINTRESOURCE( NULL );

	/* ウインドウ名称 */
	g.cpWindowName = name;

	/* ウインドウクラスパラメータセット */
	g.tWndClass.cbSize		= sizeof( WNDCLASSEX );
	g.tWndClass.style			= CS_HREDRAW | CS_VREDRAW;
	g.tWndClass.lpfnWndProc	= WinProc;
	g.tWndClass.cbClsExtra	= 0;	/* GetClassLong で取得可能なメモリ */
	g.tWndClass.cbWndExtra	= 0;	/* GetWindowLong で取得可能なメモリ */
	g.tWndClass.hInstance		= g.hInstance;
	g.tWndClass.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	g.tWndClass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	g.tWndClass.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
	g.tWndClass.lpszMenuName	= g.cpMenu;
	g.tWndClass.lpszClassName = g.cpClassName;
	g.tWndClass.hIconSm		= NULL;

	/* ウインドウクラス生成 */
	if ( 0 == RegisterClassEx( &g.tWndClass ) ) 
	{
	}

	/* ウインドウを生成する */
	{
		RECT rc;
		SetRect(&rc, 0, 0, m_width, m_height );
		int valWin = WS_OVERLAPPEDWINDOW;
		AdjustWindowRectEx(&rc, valWin, FALSE, 0);

		g.m_hWnd = CreateWindowEx(
			  0
			, g.tWndClass.lpszClassName//m_classname//wc.lpszClassName
			, name 
			, valWin
			, 800+rc.left   	        // horizontal position of window
			, 100+rc.top				// vertical position of window
			, rc.right-rc.left 		// window width
			, rc.bottom-rc.top			// window height
			, NULL                    // handle to parent or owner window
			, NULL                    // handle to menu, or child-window identifier
			, g.hInstance               // handle to application instance
			, 0				       // pointer to window-creation data
		);

	}		
	
	GDI_create(24);
}
