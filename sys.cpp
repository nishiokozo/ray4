// 2017/07/07 ray3
// 2019/06/25 ray4

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>       // sleep_for
using namespace std;
#include <windows.h>

#include "sys.h"

static chrono::system_clock::time_point time_a;
static chrono::system_clock::time_point time_b;

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


	struct	PrimBezier
	{
		double	x0;
		double	y0;
		double	x1;
		double	y1;
		double	x2;
		double	y2;
		double	x3;
		double	y3;
		int		col;
	};
	struct	PrimTri
	{
		double	x0;
		double	y0;
		double	x1;
		double	y1;
		double	x2;
		double	y2;
		int		col;
	};
	struct	PrimLine
	{
		double	x0;
		double	y0;
		double	x1;
		double	y1;
		int		col;
	};
	struct	PrimPset
	{
		double	x;
		double	y;
		int		col;
	};
	struct	PrimCircle
	{
		double	x0;
		double	y0;
		double	x1;
		double	y1;
		int		col;
	};
	struct	PrimClr
	{
		bool	bActive;
		int		col;
	};
struct //	GDI
{
	struct
	{
//		BITMAPINFO	bmpInfo;
//		int			bpp;
//		BYTE*		bPixelBits = nullptr;
//		int			width;
//		int			height;

		vector<PrimBezier>	tblBezier;
		vector<PrimTri>		tblTri;
		vector<PrimLine>	tblLine;
		vector<PrimPset>	tblPset;
		vector<PrimCircle>	tblCircle;
		PrimClr				clr;

		RECT rect;


	} m;
	HDC     hdcBackbuffer;
	 BITMAP bmBitmap;


	//------------------------------------------------------------------------------
	void ReleasePixelBits()
	//------------------------------------------------------------------------------
	{
/*
		if ( m.bPixelBits != 0 ) 
		{
			delete [] m.bPixelBits;
		}
*/

	}
/*
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
*/
	
	//------------------------------------------------------------------------------
	void paint0( HDC hDc )
	//------------------------------------------------------------------------------
	{

		// clear background
		if ( m.clr.bActive )
		{
			HBRUSH hBrush  = CreateSolidBrush(m.clr.col);
			SelectObject( hDc , hBrush);

			PatBlt( hDc , 0 , 0 ,m.rect.right, m.rect.bottom , PATCOPY);

			DeleteObject( hBrush );
	
			m.clr.bActive = false;
		}
		
		// bmp
		{
//				StretchDIBits( hDc, 0, 0, m.rect.right, m.rect.bottom, 0, 0, m.width, m.height, m.bPixelBits, &m.bmpInfo, DIB_RGB_COLORS, SRCCOPY );

		}

		
		//circle
		{

			for ( unsigned int i=0 ; i < m.tblCircle.size() ; i++ )
			{

				int x0 = m.tblCircle[i].x0;
				int y0 = m.tblCircle[i].y0;
				int x1 = m.tblCircle[i].x1;
				int y1 = m.tblCircle[i].y1;
				int	col = m.tblCircle[i].col;

				HPEN hPen = CreatePen(PS_SOLID, 1, col );
				SelectObject(hDc, hPen);
				HBRUSH hBrush  = CreateSolidBrush(col);
				SelectObject(hDc, hPen);

//				SetPixel( hDc, x0, y0, col );

				Ellipse(hDc , x0, y0, x1, y1);

				DeleteObject(hBrush);
				DeleteObject(hPen);
			}
			
			m.tblCircle.clear();
		}

		//pset
		{

			for ( unsigned int i=0 ; i < m.tblPset.size() ; i++ )
			{
				HPEN hPen;

				int x0 = m.tblPset[i].x;
				int y0 = m.tblPset[i].y;
				int	col = m.tblPset[i].col;

				hPen = CreatePen(PS_SOLID, 1, col );
				SelectObject(hDc, hPen);

				SetPixel( hDc, x0, y0, col );

				DeleteObject(hPen);
			}
			
			m.tblPset.clear();
		}
		//line
		{

			for ( unsigned int i=0 ; i < m.tblLine.size() ; i++ )
			{

				int x0 = m.tblLine[i].x0;
				int y0 = m.tblLine[i].y0;
				int x1 = m.tblLine[i].x1;
				int y1 = m.tblLine[i].y1;
				int	col = m.tblLine[i].col;

				HPEN hPen = CreatePen(PS_SOLID, 1, col );
				SelectObject(hDc, hPen);

				MoveToEx(hDc, x0, y0, NULL);
				LineTo(hDc, x1, y1);

				DeleteObject(hPen);
			}
			m.tblLine.clear();
		}

		//Tri
		{
			for ( unsigned int i=0 ; i < m.tblTri.size() ; i++ )
			{	
				int x0 = m.tblTri[i].x0;
				int y0 = m.tblTri[i].y0;
				int x1 = m.tblTri[i].x1;
				int y1 = m.tblTri[i].y1;
				int x2 = m.tblTri[i].x2;
				int y2 = m.tblTri[i].y2;
				int	col = m.tblTri[i].col;

				HPEN hPen = CreatePen(PS_SOLID, 1, col );
				HBRUSH hBrush  = CreateSolidBrush(col);
				SelectObject(hDc, hPen);
				SelectObject(hDc, hBrush);

				POINT	tblPoint[3]={{x0,y0},{x1,y1},{x2,y2}};
				INT	tblNum[]={3};

				PolyPolygon( hDc, tblPoint, tblNum, 1 );

				DeleteObject(hBrush);
				DeleteObject(hPen);
			}
			m.tblTri.clear();
		}


		//Bezier
		{
			for ( unsigned int i=0 ; i < m.tblBezier.size() ; i++ )
			{	
				int x0 = m.tblBezier[i].x0;
				int y0 = m.tblBezier[i].y0;
				int x1 = m.tblBezier[i].x1;
				int y1 = m.tblBezier[i].y1;
				int x2 = m.tblBezier[i].x2;
				int y2 = m.tblBezier[i].y2;
				int x3 = m.tblBezier[i].x3;
				int y3 = m.tblBezier[i].y3;
				int	col = m.tblBezier[i].col;

				HPEN hPen = CreatePen(PS_SOLID, 1, col );
				HBRUSH hBrush  = CreateSolidBrush(col);
				SelectObject(hDc, hPen);
				SelectObject(hDc, hBrush);

				POINT	tblPoint[3]={{x1,y1},{x2,y2},{x3,y3}};

				MoveToEx(hDc , x0 , y0 , NULL);
				PolyBezierTo(hDc , tblPoint , 3);

				DeleteObject(hBrush);
				DeleteObject(hPen);
			}
			m.tblBezier.clear();

			static POINT pt[3];

		}


	}

	//------------------------------------------------------------------------------
	void onCreate( HWND hWnd ) 
	//------------------------------------------------------------------------------
	{
	    HDC hDc = GetDC(hWnd);                      	// ウインドウのDCを取得
//	    GetClientRect(GetDesktopWindow(), &rc);  	// デスクトップのサイズを取得
				RECT rc;
				GetClientRect( hWnd, &rc );

		HBITMAP hBitmap = CreateCompatibleBitmap( hDc, rc.right, rc.bottom );
	    hdcBackbuffer = CreateCompatibleDC( NULL );		// カレントスクリーン互換
	    SelectObject( hdcBackbuffer, hBitmap );		// MDCにビットマップを割り付け

		GetObject(hBitmap , sizeof (BITMAP) , &bmBitmap);		

		ReleaseDC( hWnd, hDc );

	}	

	//------------------------------------------------------------------------------
	void onPaint2(HWND hWnd) 
	//------------------------------------------------------------------------------
	{
#if 0
	 		{
				PAINTSTRUCT ps;
				HDC hDc = BeginPaint( hWnd , &ps );	//再描画区域が指定されてある。WM_PAINTメッセージを処理する
				EndPaint( hWnd , &ps);
			}
#else
			{
 				// 何故か白フラッシュが入る
			    paint0( hdcBackbuffer);
			    PAINTSTRUCT ps;
			    HDC hDc = BeginPaint(hWnd, &ps);
 #if 0
				RECT rc;
				GetClientRect( hWnd, &rc );
			    BitBlt(hDc, 0, 0, rc.right, rc.bottom, hdcBackbuffer, 0, 0, SRCCOPY);

 #else
				StretchBlt(
					hDc ,
					0,0,
					bmBitmap.bmWidth / 1 , 
					bmBitmap.bmHeight / 1 ,
					hdcBackbuffer , 
					0 , 
					0 ,
					bmBitmap.bmWidth , 
					bmBitmap.bmHeight ,
					SRCCOPY
				);
 #endif
			    EndPaint(hWnd, &ps);
			}
#endif
	}

} gdi;

/*
//------------------------------------------------------------------------------
unsigned char* Sys::GetAddrPixels()
//------------------------------------------------------------------------------
{
	return( (unsigned char*)gdi.m.bPixelBits);
}

//------------------------------------------------------------------------------
int Sys::GetBytePixels()
//------------------------------------------------------------------------------
{
	return gdi.m.bpp/8;
}
*/

//------------------------------------------------------------------------------
static	LRESULT CALLBACK WinProc
//------------------------------------------------------------------------------
(
	  HWND		hWnd
	, UINT		uMsg
	, WPARAM	wParam	//メッセージの付加情報
	, LPARAM	lParam	//メッセージの付加情報
)
{
	switch( uMsg ) 
	{
		case WM_CREATE:	// CreateWindowと同時に発行される
			gdi.onCreate( hWnd );
			break;
		case WM_SIZE:	// 画面サイズが決定された時に発行される
			GetClientRect( hWnd, &gdi.m.rect );
			return 0;

		case WM_PAINT:	// OSからの描画要求。再描画区域情報（ウィンドウが重なっている際などの）が得られるタイミング。
	 		gdi.onPaint2( hWnd );
			return 0;

		case WM_KEYDOWN:
			if ( wParam == VK_ESCAPE ) SendMessage(hWnd , WM_DESTROY , 0 , 0);	
			return 0;

		case WM_DESTROY:	//[x]を押すなどしたとき
cout << "WM_DESTROY " << endl;
			PostQuitMessage( 0 );
			return 0;
			break;
	}

	// デフォルト処理呼び出し。
	return DefWindowProc( hWnd, uMsg, wParam, lParam );// [x] > WM_SYSCOMMAND > WM_CLOSE > WM_DESTROY
}

//------------------------------------------------------------------------------
int	Sys::Rgb( double r, double g , double b )
//------------------------------------------------------------------------------
{
	r = min( 1.0, r );
	g = min( 1.0, g );
	b = min( 1.0, b );
	int	ir = ((int)(r*255))&0xff;
	int	ig = ((int)(g*255))&0xff;
	int	ib = ((int)(b*255))&0xff;
	int	col = RGB(ir,ig,ib);
	return col ;
}

//------------------------------------------------------------------------------
void Sys::Clr( int col)
//------------------------------------------------------------------------------
{
	gdi.m.clr.bActive = true;
	gdi.m.clr.col = col;
}
//------------------------------------------------------------------------------
void Sys::Circle( double x, double y, double r, int col )
//------------------------------------------------------------------------------
{
	PrimCircle a = {x-r,y-r,x+r,y+r,col};
	
	gdi.m.tblCircle.push_back( a );
}
//------------------------------------------------------------------------------
void Sys::Pset( double x, double y, int col )
//------------------------------------------------------------------------------
{
	PrimPset a = {x,y,col};
	
	gdi.m.tblPset.push_back( a );
}
//------------------------------------------------------------------------------
void Sys::Line( double x0, double y0, double x1, double y1,int col)
//------------------------------------------------------------------------------
{
	PrimLine a = {x0,y0,x1,y1,col};
	
	gdi.m.tblLine.push_back( a );
}
//------------------------------------------------------------------------------
void Sys::Tri( double x0, double y0, double x1, double y1, double x2, double y2, int col)
//------------------------------------------------------------------------------
{
	PrimTri a = {x0,y0,x1,y1,x2,y2,col};
	
	gdi.m.tblTri.push_back( a );
}
//------------------------------------------------------------------------------
void Sys::Bezier( double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, int col)
//------------------------------------------------------------------------------
{
	PrimBezier a = {x0,y0,x1,y1,x2,y2,x3,y3,col};
	
	gdi.m.tblBezier.push_back( a );
}

//------------------------------------------------------------------------------
Sys::~Sys()
//------------------------------------------------------------------------------
{
	gdi.ReleasePixelBits();

}
//------------------------------------------------------------------------------
Sys::Sys( const char* name, int pos_x, int pos_y, int width, int height  )
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


//	gdi.CreatePixelBits( 24, width, height );
	ShowWindow( win.hWnd, SW_SHOW );
}

//------------------------------------------------------------------------------
bool Sys::Update()
//------------------------------------------------------------------------------
{
		InvalidateRect(win.hWnd , NULL , TRUE);	//	WM_PAINTを発行し再描画矩形情報を渡す

	while(1)
	{
		while ( PeekMessage( &win.tMsg, NULL, 0, 0, PM_REMOVE) )
		{
			DispatchMessage( &win.tMsg );
			TranslateMessage( &win.tMsg );
			if ( win.tMsg.message == WM_QUIT ) return false; 
		}
#if 0
		HDC hDc = GetDC( win.hWnd );
		gdi.paint0( gdi.hdcBackbuffer);
				RECT rc;
				GetClientRect( win.hWnd, &rc );
 #if 1
	    BitBlt(hDc, 0, 0, rc.right, rc.bottom, gdi.hdcBackbuffer, 0, 0, SRCCOPY);
 #else
		RECT rect;
		GetClientRect(win.hWnd , &rect);

		StretchBlt(
			hDc ,
			0,0,
			gdi.bmBitmap.bmWidth / 1 , 
			gdi.bmBitmap.bmHeight / 1 ,
			gdi.hdcBackbuffer , 
			0 , 
			0 ,
			gdi.bmBitmap.bmWidth , 
			gdi.bmBitmap.bmHeight ,
			SRCCOPY
		);
 #endif

		ReleaseDC( win.hWnd, hDc );
#endif

		return true;
	}
}
