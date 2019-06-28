// 2017/07/07 ray3
// 2019/06/25 ray4

#include <iostream>
#include <vector>
using namespace std;
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

struct //	GDI
{
	struct
	{
		BITMAPINFO	bmpInfo;
		int			bpp;
		BYTE*		bPixelBits = nullptr;
		int			width;
		int			height;

		vector<Line>	tblLine;
	} m;


	//------------------------------------------------------------------------------
	void ReleasePixelBits()
	//------------------------------------------------------------------------------
	{
		if ( m.bPixelBits != 0 ) 
		{
			delete [] m.bPixelBits;
		}
	}
	//------------------------------------------------------------------------------
	void CreatePixelBits(int bpp, int width, int height )
	//------------------------------------------------------------------------------
	{
		m.bPixelBits	= new BYTE[ width * height * bpp/8 ];

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
#if 1
		HDC hdc = GetDC( hWnd );
		{
			// bmp
			{
				RECT rect;
				GetClientRect( hWnd, &rect );
				StretchDIBits( hdc, 0, 0, rect.right, rect.bottom, 0, 0, m.width, m.height, m.bPixelBits, &m.bmpInfo, DIB_RGB_COLORS, SRCCOPY );
			}
			
			//line
			{
				HPEN hPen, hOldPen;


				for ( unsigned int i=0 ; i < m.tblLine.size() ; i++ )
				{

					int x0 = m.tblLine[i].x0;
					int y0 = m.tblLine[i].y0;
					int x1 = m.tblLine[i].x1;
					int y1 = m.tblLine[i].y1;
					int	col = m.tblLine[i].col;

					hPen = CreatePen(PS_SOLID, 1, col );
					hOldPen = (HPEN)SelectObject(hdc, hPen);

					MoveToEx(hdc, x0, y0, NULL);
					LineTo(hdc, x1, y1);

//					MoveToEx(hdc, 50, 50, NULL);
//					LineTo(hdc, 100, 100);
//					LineTo(hdc, 50, 100);

					SelectObject(hdc, hOldPen);
					DeleteObject(hPen);
				}
				
				m.tblLine.clear();
			}
		}
		ReleaseDC( hWnd, hdc );
#endif

	}

	//------------------------------------------------------------------------------
	void Paint( HWND hWnd )
	//------------------------------------------------------------------------------
	{
#if 1
		{
			// 起動時にからであっても最低一度はBeginPaint~EndPaintをやっておかく必要がある。
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint( hWnd , &ps );
			EndPaint( hWnd , &ps);
		}
#endif
#if 0
cout << "paint " << endl;
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint( hWnd , &ps );
//		HDC hdc = GetDC( hWnd );
		{
			// bmp
			{
				RECT rect;
				GetClientRect( hWnd, &rect );
				StretchDIBits( hdc, 0, 0, rect.right, rect.bottom, 0, 0, m.width, m.height, m.bPixelBits, &m.bmpInfo, DIB_RGB_COLORS, SRCCOPY );
			}
			
			//line
			{
				HPEN hPen, hOldPen;


				for ( unsigned int i=0 ; i < m.tblLine.size() ; i++ )
				{

					int x0 = m.tblLine[i].x0;
					int y0 = m.tblLine[i].y0;
					int x1 = m.tblLine[i].x1;
					int y1 = m.tblLine[i].y1;
					int	col = m.tblLine[i].col;

					hPen = CreatePen(PS_SOLID, 1, col );
					hOldPen = (HPEN)SelectObject(hdc, hPen);

					MoveToEx(hdc, x0, y0, NULL);
					LineTo(hdc, x1, y1);

//					MoveToEx(hdc, 50, 50, NULL);
//					LineTo(hdc, 100, 100);
//					LineTo(hdc, 50, 100);

					SelectObject(hdc, hOldPen);
					DeleteObject(hPen);
				}
				
				m.tblLine.clear();
			}
		}
			EndPaint( hWnd , &ps);
///		ReleaseDC( hWnd, hdc );
	}
#endif
#if 0
cout << "paint " << endl;
		HDC hdc = GetDC( hWnd );
		{
			// bmp
			{
				RECT rect;
				GetClientRect( hWnd, &rect );
				StretchDIBits( hdc, 0, 0, rect.right, rect.bottom, 0, 0, m.width, m.height, m.bPixelBits, &m.bmpInfo, DIB_RGB_COLORS, SRCCOPY );
			}
			
			//line
			{
				HPEN hPen, hOldPen;


				for ( unsigned int i=0 ; i < m.tblLine.size() ; i++ )
				{

					int x0 = m.tblLine[i].x0;
					int y0 = m.tblLine[i].y0;
					int x1 = m.tblLine[i].x1;
					int y1 = m.tblLine[i].y1;
					int	col = m.tblLine[i].col;

					hPen = CreatePen(PS_SOLID, 1, col );
					hOldPen = (HPEN)SelectObject(hdc, hPen);

					MoveToEx(hdc, x0, y0, NULL);
					LineTo(hdc, x1, y1);

//					MoveToEx(hdc, 50, 50, NULL);
//					LineTo(hdc, 100, 100);
//					LineTo(hdc, 50, 100);

					SelectObject(hdc, hOldPen);
					DeleteObject(hPen);
				}
				
				m.tblLine.clear();
			}
		}
		ReleaseDC( hWnd, hdc );
#endif
	}

} gdi;

//------------------------------------------------------------------------------
unsigned char* Win::GetAddrPixels()
//------------------------------------------------------------------------------
{
	return( (unsigned char*)gdi.m.bPixelBits);
}

//------------------------------------------------------------------------------
int Win::GetBytePixels()
//------------------------------------------------------------------------------
{
	return gdi.m.bpp/8;
}

//------------------------------------------------------------------------------
static	LRESULT CALLBACK WinProc
//------------------------------------------------------------------------------
(
	  HWND		hWnd	// handle to window
	, UINT		uMsg	// message identifier
	, WPARAM	wParam	// first message parameter
	, LPARAM	lParam	// second message parameter
)
{
	switch( uMsg ) 
	{
		case WM_CREATE:	//	CreateWindowと同時に発行される
			break;

		case WM_PAINT:
			{
				gdi.Paint( hWnd );
				return 0;
			}

		case WM_KEYDOWN:
			{
				if ( wParam == 27 ) // ESC
				{
					PostQuitMessage( 0 );
				}
				return 0;
				break;
			}

		case WM_DESTROY:	//[x]を押すなどしたとき

			PostQuitMessage( 0 );
			break;
	}

	// デフォルト処理呼び出し
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//------------------------------------------------------------------------------
int	Win::rgb( double r, double g , double b )
//------------------------------------------------------------------------------
{
	int	ir = ((int)(r*255))&0xff;
	int	ig = ((int)(g*255))&0xff;
	int	ib = ((int)(b*255))&0xff;
	int	col = RGB(ir,ig,ib);
	return col ;
}

//------------------------------------------------------------------------------
void Win::line( double x0, double y0, double x1, double y1,int col)
//------------------------------------------------------------------------------
{
	Line l = {x0,y0,x1,y1,col};
	
	gdi.m.tblLine.push_back( l );
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
//UpdateWindow(win.hWnd);

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
Win::Win( const char* name, int pos_x, int pos_y, int width, int height  )
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
			, pos_x + rc.left	  	// horizontal position of window
			, pos_y + rc.top		// vertical position of window
			, rc.right-rc.left 	// window width
			, rc.bottom-rc.top	// window height
			, NULL			  	// handle to parent or owner window
			, NULL			 	// handle to menu, or child-window identifier
			, win.hInstance	 	// handle to application instance
			, 0					// pointer to window-creation data
		);

	}

	ShowWindow( win.hWnd, SW_SHOW );

	gdi.CreatePixelBits( 24, width, height );
}
