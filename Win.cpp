// ver 1.1 2017/07/07 

#include <stdio.h>
#include <windows.h>

#include "Win.h"


//void	(*g_callbackPaint)(HDC)=0;
//void	(*g_callbackUpdate)(Win&)=0;


//------------------------------------------------------------------------------
void Win::GDI_draw()
//------------------------------------------------------------------------------
{

	HDC hdc = GetDC(m_hWnd);
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	StretchDIBits(hdc, 0, 0, rect.right, rect.bottom, 0, 0, m_width, m_height, GDI_bPixelBits, &GDI_bmpInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(m_hWnd,hdc);

}
//------------------------------------------------------------------------------
void Win::GDI_create(int bpp )
//------------------------------------------------------------------------------
{
	GDI_bPixelBits = (BYTE*) malloc( m_width*m_height*bpp/8 );
	GDI_bpp=bpp;
	BITMAPINFO bi;
	BITMAPINFOHEADER &bih=GDI_bmpInfo.bmiHeader;
	bih.biSize			= sizeof(bih);
	bih.biWidth			= m_width;
	bih.biHeight		= m_height;
	bih.biPlanes		= 1;
	bih.biBitCount		= GDI_bpp;
	bih.biCompression	= BI_RGB;//BI_BITFIELDS;
	bih.biSizeImage		= 0;
	bih.biXPelsPerMeter	= 0;
	bih.biYPelsPerMeter	= 0;
	bih.biClrUsed		= 0;
	bih.biClrImportant	= 0;

}

//------------------------------------------------------------------------------
static	LRESULT CALLBACK WinProc
//------------------------------------------------------------------------------
(
	HWND	m_hWnd	// handle to window
	, UINT	uMsg	// message identifier
	, WPARAM wParam // first message parameter
	, LPARAM lParam // second message parameter
)
{


	switch( uMsg ) 
	{
		case WM_CREATE:

			/* ウインドウを表示する */
			ShowWindow( m_hWnd, SW_SHOW );
			break;

		case WM_PAINT:
			{
				static LARGE_INTEGER nFreq, time1, time2;
				QueryPerformanceFrequency(&nFreq);
				{

					PAINTSTRUCT ps;
					HDC hdc = BeginPaint(m_hWnd , &ps);

//					if ( g_callbackPaint ) g_callbackPaint( hdc );

					EndPaint(m_hWnd , &ps);
				}


				QueryPerformanceCounter(&time2);

				double time = ((double)(time2.QuadPart - time1.QuadPart) / (double)nFreq.QuadPart);
				QueryPerformanceCounter(&time1);

//		printf( "paint %f %d %d \n", time*1000, (int)nFreq.QuadPart, (int)(time2.QuadPart - time1.QuadPart) );
				UpdateWindow(m_hWnd);

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
	return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
}


/*
//------------------------------------------------------------------------------
void	Win::entryCallbackPaint( void(*func)(HDC) )
//------------------------------------------------------------------------------
{
	g_callbackPaint = func;
}
//------------------------------------------------------------------------------
void	 Win::entryCallbackUpdate( void(*func)(Win&) )
//------------------------------------------------------------------------------
{
	g_callbackUpdate = func;
}
*/

//------------------------------------------------------------------------------
bool Win::runMessage()
//------------------------------------------------------------------------------
{
	while(1)
	{
		while ( PeekMessage( &tMsg, NULL, 0, 0, PM_REMOVE) )
		{
			DispatchMessage(&tMsg);
			TranslateMessage(&tMsg);
			if (tMsg.message == WM_QUIT) break;
		}
		if (tMsg.message == WM_QUIT) break;
//		if ( g_callbackUpdate ) g_callbackUpdate(*this);

		switch( m_render )
		{
		case RENDER_NONE:break;
		case RENDER_GDI24:GDI_draw();break;
		case RENDER_GDI32:GDI_draw();break;
		}
		return true;
	}
//	return( tMsg.wParam );
	return	false;
}
/*
//------------------------------------------------------------------------------
int Win::run()
//------------------------------------------------------------------------------
{
	while(1)
	{
		while ( PeekMessage( &tMsg, NULL, 0, 0, PM_REMOVE) )
		{
			DispatchMessage(&tMsg);
			TranslateMessage(&tMsg);
			if (tMsg.message == WM_QUIT) break;
		}
		if (tMsg.message == WM_QUIT) break;
		if ( g_callbackUpdate ) g_callbackUpdate(*this);

		switch( m_render )
		{
		case RENDER_NONE:break;
		case RENDER_GDI24:GDI_draw();break;
		case RENDER_GDI32:GDI_draw();break;
		}
	}
	return( tMsg.wParam );
}
*/
//------------------------------------------------------------------------------
Win::~Win()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
Win::Win( int w, int h, const char* name, RENDER render  )
//------------------------------------------------------------------------------
{
	m_render=render;
	m_width=w;
	m_height=h;

	/* アプリケーションインスタンス */
	hInstance		= GetModuleHandle( NULL );


	/* クラス名称 */
	cpClassName	= "MainWindowClass";

	/* メニュー */
	cpMenu			= MAKEINTRESOURCE( NULL );

	/* ウインドウ名称 */
	cpWindowName = name;

	/* ウインドウクラスパラメータセット */
	tWndClass.cbSize		= sizeof( WNDCLASSEX );
	tWndClass.style			= CS_HREDRAW | CS_VREDRAW;
	tWndClass.lpfnWndProc	= WinProc;
	tWndClass.cbClsExtra	= 0;	/* GetClassLong で取得可能なメモリ */
	tWndClass.cbWndExtra	= 0;	/* GetWindowLong で取得可能なメモリ */
	tWndClass.hInstance		= hInstance;
	tWndClass.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	tWndClass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	tWndClass.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
	tWndClass.lpszMenuName	= cpMenu;
	tWndClass.lpszClassName = cpClassName;
	tWndClass.hIconSm		= NULL;

	/* ウインドウクラス生成 */
	if ( 0 == RegisterClassEx( &tWndClass ) ) 
	{
	}

	/* ウインドウを生成する */
	{
		RECT rc;
		SetRect(&rc, 0, 0, m_width, m_height );
//		int valWin = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);//
		int valWin = WS_OVERLAPPEDWINDOW;
		AdjustWindowRectEx(&rc, valWin, FALSE, 0);

		m_hWnd = CreateWindowEx(
			  0
			, tWndClass.lpszClassName//m_classname//wc.lpszClassName
			, name 
			, valWin
			, 800+rc.left   	        // horizontal position of window
			, 100+rc.top				// vertical position of window
			, rc.right-rc.left 		// window width
			, rc.bottom-rc.top			// window height
			, NULL                    // handle to parent or owner window
			, NULL                    // handle to menu, or child-window identifier
			, hInstance               // handle to application instance
			, 0				       // pointer to window-creation data
		);

	}		
	
	switch( m_render )
	{
	case RENDER_NONE:		break;
	case RENDER_GDI24:	GDI_create(24);	break;
	case RENDER_GDI32:	GDI_create(32);	break;
	}
//	return	true;
}
