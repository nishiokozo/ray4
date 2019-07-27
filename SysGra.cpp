//2019/07/12


#include <iostream>
#include <vector>
#include <chrono>
#include <thread>       // sleep_for
using namespace std;

#include "SysGra.h"

#include <windows.h>
#include <windowsx.h>
#include <functional>
#include "SysWin.h"

static struct G
{
	bool flgActive;
//	RECT rect;
	HDC  hdcBackbuffer;
	HBITMAP hBitmap;

	int pos_x;
	int pos_y;
	int	width;
	int height;

	G()
	{
		flgActive = false;
	}

} g;


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
int SysGra::GetWidth()
//------------------------------------------------------------------------------
{
	SysWin& win = SysWin::GetInstance();

	return win.GetWidth();
}

//------------------------------------------------------------------------------
int SysGra::GetHeight()
//------------------------------------------------------------------------------
{
	SysWin& win = SysWin::GetInstance();

	return win.GetHeigit();
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

//------------------------------------------------------------------------------
void  SysGra::OnCreate() 
//------------------------------------------------------------------------------
{
	HWND hWnd = SysWin::GetInstance().win.hWnd;
}	

//------------------------------------------------------------------------------
void  SysGra::OnSize( int width, int height ) 
//------------------------------------------------------------------------------
{
	g.width = width;
	g.height = height;

//cout << "onsize " << g.width << " " << g.height << endl;

	HWND hWnd = SysWin::GetInstance().win.hWnd;
    HDC hDc = GetDC(hWnd);
    {
		if ( g.flgActive == true )
		{
			DeleteDC(g.hdcBackbuffer);
			DeleteObject(g.hBitmap);
		}


		g.hBitmap = CreateCompatibleBitmap( hDc, width, height );

	    g.hdcBackbuffer = CreateCompatibleDC( NULL );
	    SelectObject( g.hdcBackbuffer, g.hBitmap );
		g.flgActive=true;
	}
	ReleaseDC( hWnd, hDc );
}

//------------------------------------------------------------------------------
void  SysGra::OnMove( int pos_x, int pos_y ) 
//------------------------------------------------------------------------------
{
	g.pos_x = pos_x;
	g.pos_y = pos_y;

	{
		HWND hWnd = SysWin::GetInstance().win.hWnd;
		RECT rect;
		SetRect(&rect, 0, 0, g.width, g.height );
		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);
//cout << "a" << endl;
//cout << pos_x << " " << pos_y << endl;
//cout << g.width << " " << g.height << endl;
//
//cout << "b" << endl;
//cout << pos_x + rect.left << " " << pos_y + rect.top << endl;
//cout << rect.right-rect.left << " " << rect.bottom-rect.top << endl;
	}


}

//------------------------------------------------------------------------------
void  SysGra::OnPaint()
//------------------------------------------------------------------------------
{
	HWND hWnd = SysWin::GetInstance().win.hWnd;

	{
		HDC hDc = g.hdcBackbuffer;

		// clear background
		if ( m.clr.bActive )
		{
			HBRUSH hBrush  = CreateSolidBrush(m.clr.col.l);
			SelectObject( hDc , hBrush);

//			PatBlt( hDc , 0 , 0 ,g.rect.right, g.rect.bottom , PATCOPY);
			PatBlt( hDc , 0 , 0 ,g.width, g.height , PATCOPY);

			DeleteObject( hBrush );

//			m.clr.bActive = false;
		}

		// bmp
		{
	//				StretchDIBits( hDc, 0, 0, g.rect.right, g.rect.bottom, 0, 0, m.width, m.height, m.bPixelBits, &m.bmpInfo, DIB_RGB_COLORS, SRCCOPY );

		}

		
		//circle
		{

			for ( unsigned int i=0 ; i < m.tblCircle.size() ; i++ )
			{

				int x0 = m.tblCircle[i].v0.x;
				int y0 = m.tblCircle[i].v0.y;
				int x1 = m.tblCircle[i].v1.x;
				int y1 = m.tblCircle[i].v1.y;
				int	col = m.tblCircle[i].col.l;

				HPEN hPen = CreatePen(PS_SOLID, 1, col );
				SelectObject(hDc, hPen);
				HBRUSH hBrush  = CreateSolidBrush(col);
				SelectObject(hDc, hPen);

	//				SetPixel( hDc, x0, y0, col );

				Ellipse(hDc , x0, y0, x1, y1);

				DeleteObject(hBrush);
				DeleteObject(hPen);
			}
			
//			m.tblCircle.clear();
		}

		//pset
		{

			for ( unsigned int i=0 ; i < m.tblPset.size() ; i++ )
			{
				HPEN hPen;

				int x0 = m.tblPset[i].v.x;
				int y0 = m.tblPset[i].v.y;
				int	col = m.tblPset[i].col.l;

				hPen = CreatePen(PS_SOLID, 1, col );
				SelectObject(hDc, hPen);

				SetPixel( hDc, x0, y0, col );

				DeleteObject(hPen);
			}
			
//			m.tblPset.clear();
		}

		//box
		{

			for ( unsigned int i=0 ; i < m.tblBox.size() ; i++ )
			{

				int x0 = m.tblBox[i].v0.x;
				int y0 = m.tblBox[i].v0.y;
				int x1 = m.tblBox[i].v1.x;
				int y1 = m.tblBox[i].v1.y;
				int	col = m.tblBox[i].col.l;

				HPEN hPen = CreatePen(PS_SOLID, 1, col );
				SelectObject(hDc, hPen);

				MoveToEx(hDc, x0, y0, NULL);
				LineTo(hDc, x1, y0);
				LineTo(hDc, x1, y1);
				LineTo(hDc, x0, y1);
				LineTo(hDc, x0, y0);

				DeleteObject(hPen);
			}
		}

		//fill
		{

			for ( unsigned int i=0 ; i < m.tblFill.size() ; i++ )
			{

				int x0 = m.tblFill[i].v0.x;
				int y0 = m.tblFill[i].v0.y;
				int x1 = m.tblFill[i].v1.x;
				int y1 = m.tblFill[i].v1.y;
				int	col = m.tblFill[i].col.l;
/*
				HPEN hPen = CreatePen(PS_SOLID, 1, col );
				SelectObject(hDc, hPen);
				HBRUSH hBrush  = CreateSolidBrush(col);

//				MoveToEx(hDc, x0, y0, NULL);
//				LineTo(hDc, x1, y0);
//				LineTo(hDc, x1, y1);
//				LineTo(hDc, x0, y1);
//				LineTo(hDc, x0, y0);
				Rectangle(hDc , x0, y0, x1, y1);
				DeleteObject(hBrush);

				DeleteObject(hPen);
*/
				HPEN hPen = CreatePen(PS_SOLID, 2, col);
				HPEN hOldPen = SelectPen(hDc, hPen);

				HBRUSH hBrush = CreateSolidBrush( col );
				HBRUSH hOldBrush = SelectBrush(hDc, hBrush);

				Rectangle(hDc , x0, y0, x1, y1);

				SelectBrush(hDc, hOldBrush);
				DeleteObject(hBrush);

				SelectPen(hDc, hOldPen);
				DeleteObject(hPen);
			}
		}

		//line
		{

			for ( unsigned int i=0 ; i < m.tblLine.size() ; i++ )
			{

				int x0 = m.tblLine[i].v0.x;
				int y0 = m.tblLine[i].v0.y;
				int x1 = m.tblLine[i].v1.x;
				int y1 = m.tblLine[i].v1.y;
				int	col = m.tblLine[i].col.l;

				HPEN hPen = CreatePen(PS_SOLID, 1, col );
				SelectObject(hDc, hPen);

				MoveToEx(hDc, x0, y0, NULL);
				LineTo(hDc, x1, y1);

				DeleteObject(hPen);
			}
//			m.tblLine.clear();
		}

		//Tri
		{
			for ( unsigned int i=0 ; i < m.tblTri.size() ; i++ )
			{	
				int x0 = m.tblTri[i].v0.x;
				int y0 = m.tblTri[i].v0.y;
				int x1 = m.tblTri[i].v1.x;
				int y1 = m.tblTri[i].v1.y;
				int x2 = m.tblTri[i].v2.x;
				int y2 = m.tblTri[i].v2.y;
				int	col = m.tblTri[i].col.l;

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
//			m.tblTri.clear();
		}


/*		//Bezier
		{
			for ( unsigned int i=0 ; i < m.tblBezier.size() ; i++ )
			{	
				int x0 = m.tblBezier[i].v0.x;
				int y0 = m.tblBezier[i].v0.y;
				int x1 = m.tblBezier[i].v1.x;
				int y1 = m.tblBezier[i].v1.y;
				int x2 = m.tblBezier[i].v2.x;
				int y2 = m.tblBezier[i].v2.y;
				int x3 = m.tblBezier[i].v3.x;
				int y3 = m.tblBezier[i].v3.y;
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
//			m.tblBezier.clear();

			static POINT pt[3];

		}
*/

	}



	{
	    PAINTSTRUCT ps;
	    HDC hDc = BeginPaint(hWnd, &ps);
//	    BitBlt(hDc, 0, 0, g.rect.right, g.rect.bottom, g.hdcBackbuffer, 0, 0, SRCCOPY);
	    BitBlt(hDc, 0, 0, g.width, g.height, g.hdcBackbuffer, 0, 0, SRCCOPY);
	    EndPaint(hWnd, &ps);
	}

}
//------------------------------------------------------------------------------
void  SysGra::OnDestroy() 
//------------------------------------------------------------------------------
{
	HWND hWnd = SysWin::GetInstance().win.hWnd;

	DeleteDC(g.hdcBackbuffer);
	DeleteObject(g.hBitmap);

	PostQuitMessage( 0 );
}


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
int	SysGra::Rgb( double r, double g , double b )
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
void SysGra::Update()
//------------------------------------------------------------------------------
{
//		m.tblBezier.clear();
		m.tblTri.clear();
		m.tblBox.clear();
		m.tblFill.clear();
		m.tblLine.clear();
		m.tblPset.clear();
		m.tblCircle.clear();
		m.clr.bActive = false;
}
//------------------------------------------------------------------------------
void SysGra::Clr( int col)
//------------------------------------------------------------------------------
{
	(*this).m.clr.bActive = true;
	(*this).m.clr.col = vect2(0,col);
}
//------------------------------------------------------------------------------
void SysGra::Circle( vect2 v, double r, int col )
//------------------------------------------------------------------------------
{
	PrimCircle a = {vect2(v.x-r, v.y-r), vect2(v.x+r, v.y+r),vect2(0,col)};
	
	(*this).m.tblCircle.push_back( a );
}
//------------------------------------------------------------------------------
void SysGra::Pset( vect2 v, int col )
//------------------------------------------------------------------------------
{
	PrimPset a = {v,vect2(0,col)};
	
	(*this).m.tblPset.push_back( a );
}
//------------------------------------------------------------------------------
void SysGra::Box( vect2 v0, vect2 v1,int col)
//------------------------------------------------------------------------------
{
	PrimBox a = {v0,v1,vect2(0,col)};
	
	(*this).m.tblBox.push_back( a );
}
//------------------------------------------------------------------------------
void SysGra::Fill( vect2 v0, vect2 v1,int col)
//------------------------------------------------------------------------------
{
	PrimFill a = {v0,v1,vect2(0,col)};
	
	(*this).m.tblFill.push_back( a );
}
//------------------------------------------------------------------------------
void SysGra::Line( vect2 v0, vect2 v1,int col)
//------------------------------------------------------------------------------
{
	PrimLine a = {v0,v1,vect2(0,col)};
	
	(*this).m.tblLine.push_back( a );
}
//------------------------------------------------------------------------------
void SysGra::Tri( vect2 v0, vect2 v1, vect2 v2, int col)
//------------------------------------------------------------------------------
{
	PrimTri a = {v0,v1,v2,vect2(0,col)};
	
	(*this).m.tblTri.push_back( a );
}
/*
//------------------------------------------------------------------------------
void SysGra::Bezier( vect2 v0, vect2 v1, vect2 v2, vect2 v3, int col)
//------------------------------------------------------------------------------
{
	PrimBezier a = {v0,v1,v2,v3,col};
	
	(*this).m.tblBezier.push_back( a );
}
*/

