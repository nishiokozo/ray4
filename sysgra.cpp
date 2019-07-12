//2019/07/12


#include <iostream>
#include <vector>
#include <chrono>
#include <thread>       // sleep_for
using namespace std;
#include <windows.h>

#include "SysGra.h"

#define	USE_RIALTIME_PAINT 0




//------------------------------------------------------------------------------
SysGra::~SysGra()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
SysGra::SysGra()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
void  SysGra::ReleasePixelBits()
//------------------------------------------------------------------------------
{

//		if ( m.bPixelBits != 0 ) 
	{
//			delete [] m.bPixelBits;
	}


}

//------------------------------------------------------------------------------
void  SysGra::CreatePixelBits(int bpp, int width, int height )
//------------------------------------------------------------------------------
{
/*
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
*/
}


/*
//------------------------------------------------------------------------------
void  SysGra::paint0( HDC hDc )
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
*/

/*

//------------------------------------------------------------------------------
void  SysGra::OnSize( HWND hWnd ) 
//------------------------------------------------------------------------------
{
	GetClientRect( hWnd, &m.rect );
}

//------------------------------------------------------------------------------
void  SysGra::OnDestroy( HWND hWnd ) 
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void  SysGra::OnShowwindow( HWND hWnd ) 
//------------------------------------------------------------------------------
{
    HDC hDc = GetDC(hWnd);
		RECT rc;
		GetClientRect( hWnd, &rc );
		HBITMAP hBitmap = CreateCompatibleBitmap( hDc, rc.right, rc.bottom );
	    hdcBackbuffer = CreateCompatibleDC( NULL );
	    SelectObject( hdcBackbuffer, hBitmap );
	ReleaseDC( hWnd, hDc );

}	

//------------------------------------------------------------------------------
void  SysGra::OnPaint(HWND hWnd) 
//------------------------------------------------------------------------------
{
#if USE_RIALTIME_PAINT
	{
		PAINTSTRUCT ps;
		HDC hDc = BeginPaint( hWnd , &ps );	//再描画区域が指定されてある。WM_PAINTメッセージを処理する
		EndPaint( hWnd , &ps);
	}
#else
	{
	    paint0( hdcBackbuffer);
	    PAINTSTRUCT ps;
	    HDC hDc = BeginPaint(hWnd, &ps);
		RECT rc;
		GetClientRect( hWnd, &rc );
	    BitBlt(hDc, 0, 0, rc.right, rc.bottom, hdcBackbuffer, 0, 0, SRCCOPY);
	    EndPaint(hWnd, &ps);
	}
#endif
}
*/

/*
//------------------------------------------------------------------------------
unsigned char* Sys::GetAddrPixels()
//------------------------------------------------------------------------------
{
	return( (unsigned char*)(*this).m.bPixelBits);
}

//------------------------------------------------------------------------------
int Sys::GetBytePixels()
//------------------------------------------------------------------------------
{
	return (*this).m.bpp/8;
}
*/

//------------------------------------------------------------------------------
void	SysGra::Draw()
//------------------------------------------------------------------------------
{
}



//------------------------------------------------------------------------------
int	SysGra::Rgb( double r, double g , double b )
//------------------------------------------------------------------------------
{
	r = min( 1.0, r );
	g = min( 1.0, g );
	b = min( 1.0, b );
	int	ir = ((int)(r*255))&0xff;
	int	ig = ((int)(g*255))&0xff;
	int	ib = ((int)(b*255))&0xff;
//	int	col = RGB(ir,ig,ib);
	int	col = RGB(ir,ig,ib);
	return col ;
}

//------------------------------------------------------------------------------
void SysGra::Clr( int col)
//------------------------------------------------------------------------------
{
	(*this).m.clr.bActive = true;
	(*this).m.clr.col = col;
}
//------------------------------------------------------------------------------
void SysGra::Circle( double x, double y, double r, int col )
//------------------------------------------------------------------------------
{
	PrimCircle a = {x-r,y-r,x+r,y+r,col};
	
	(*this).m.tblCircle.push_back( a );
}
//------------------------------------------------------------------------------
void SysGra::Pset( double x, double y, int col )
//------------------------------------------------------------------------------
{
	PrimPset a = {x,y,col};
	
	(*this).m.tblPset.push_back( a );
}
//------------------------------------------------------------------------------
void SysGra::Line( double x0, double y0, double x1, double y1,int col)
//------------------------------------------------------------------------------
{
	PrimLine a = {x0,y0,x1,y1,col};
	
	(*this).m.tblLine.push_back( a );
}
//------------------------------------------------------------------------------
void SysGra::Tri( double x0, double y0, double x1, double y1, double x2, double y2, int col)
//------------------------------------------------------------------------------
{
	PrimTri a = {x0,y0,x1,y1,x2,y2,col};
	
	(*this).m.tblTri.push_back( a );
}
//------------------------------------------------------------------------------
void SysGra::Bezier( double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, int col)
//------------------------------------------------------------------------------
{
	PrimBezier a = {x0,y0,x1,y1,x2,y2,x3,y3,col};
	
	(*this).m.tblBezier.push_back( a );
}

