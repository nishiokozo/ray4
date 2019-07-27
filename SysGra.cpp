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
/*
if(0)
		if ( m.clr.bActive )
		{
			HBRUSH hBrush  = CreateSolidBrush(m.clr.col);
			SelectObject( hDc , hBrush);

//			PatBlt( hDc , 0 , 0 ,g.rect.right, g.rect.bottom , PATCOPY);
			PatBlt( hDc , 0 , 0 ,g.width, g.height , PATCOPY);

			DeleteObject( hBrush );

//			m.clr.bActive = false;
		}
*/

		// bmp
		{
	//				StretchDIBits( hDc, 0, 0, g.rect.right, g.rect.bottom, 0, 0, m.width, m.height, m.bPixelBits, &m.bmpInfo, DIB_RGB_COLORS, SRCCOPY );

		}

		
		for ( unsigned int i=0 ; i < m.tblVect2.size() ; )
		{
			vect2 attr = m.tblVect2[i++];
			EnumType	type	= (EnumType)attr.x;
			int			col		= attr.y;

			HPEN hPen = CreatePen(PS_SOLID, 1, col);
			HPEN hOldPen = SelectPen(hDc, hPen);
			HBRUSH hBrush = CreateSolidBrush( col );
			HBRUSH hOldBrush = SelectBrush(hDc, hBrush);

			switch ( type )
			{
				case TypeClr:
					PatBlt( hDc , 0 , 0 ,g.width, g.height , PATCOPY);
					break;

				case TypeCircle:
					{
						vect2 v0 = m.tblVect2[i++];
						vect2 v1 = m.tblVect2[i++];
						Ellipse(hDc , v0.x, v0.y, v1.x, v1.y);
					}
					break;

				case TypePset:
					{
						vect2 v0 = m.tblVect2[i++];
						SetPixel( hDc, v0.x, v0.y, col );
					}
					break;

				case TypeFill:
					{
						vect2 v0 = m.tblVect2[i++];
						vect2 v1 = m.tblVect2[i++];
						Rectangle(hDc , v0.x, v0.y, v1.x, v1.y); 
					}
					break;
				
				case TypeLine: 
					{
						vect2 v0 = m.tblVect2[i++];
						vect2 v1 = m.tblVect2[i++];
						MoveToEx(hDc, v0.x, v0.y, NULL); 
						LineTo(hDc, v1.x, v1.y); 
					}
					break;

				case TypeBox:
					{
						vect2 v0 = m.tblVect2[i++];
						vect2 v1 = m.tblVect2[i++];
						MoveToEx(hDc, v0.x, v0.y, NULL);
						LineTo(hDc, v1.x, v0.y);
						LineTo(hDc, v1.x, v1.y);
						LineTo(hDc, v0.x, v1.y);
						LineTo(hDc, v0.x, v0.y);
					}
					break;

				case TypeTri:
					{	
						vect2 v0 = m.tblVect2[i++];
						vect2 v1 = m.tblVect2[i++];
						vect2 v2 = m.tblVect2[i++];
						POINT	tblPoint[3]={{(int)v0.x,(int)v0.y} , {(int)v1.x,(int)v1.y} , {(int)v2.x,(int)v2.y} };
						INT	tblNum[]={3};
						PolyPolygon( hDc, tblPoint, tblNum, 1 );;
					}
					break;

			}

			SelectBrush(hDc, hOldBrush);
			DeleteObject(hBrush);

			SelectPen(hDc, hOldPen);
			DeleteObject(hPen);
		}


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
//		m.tblTri.clear();
//		m.tblBox.clear();
//		m.tblFill.clear();
//		m.tblLine.clear();
//		m.tblPset.clear();
//		m.tblCircle.clear();
//		m.clr.bActive = false;

		m.tblVect2.clear();
}
//------------------------------------------------------------------------------
void SysGra::Clr( int col)
//------------------------------------------------------------------------------
{
//	(*this).m.clr.bActive = true;
//	(*this).m.clr.col = col;

	m.tblVect2.emplace_back( TypeClr, col );
}
//------------------------------------------------------------------------------
void SysGra::Circle( vect2 v, double r, int col )
//------------------------------------------------------------------------------
{
//	PrimCircle a = {col,vect2(v.x-r, v.y-r), vect2(v.x+r, v.y+r)};
//	(*this).m.tblCircle.push_back( a );
	m.tblVect2.emplace_back( TypeCircle, col );
	m.tblVect2.emplace_back( v.x-r, v.y-r );
	m.tblVect2.emplace_back( v.x+r, v.y+r );

}
//------------------------------------------------------------------------------
void SysGra::Pset( vect2 v0, int col )
//------------------------------------------------------------------------------
{
//	PrimPset a = {col,v};
//	(*this).m.tblPset.push_back( a );

	m.tblVect2.emplace_back( TypePset, col );
	m.tblVect2.emplace_back( v0 );
}
//------------------------------------------------------------------------------
void SysGra::Box( vect2 v0, vect2 v1,int col)
//------------------------------------------------------------------------------
{
//	PrimBox a = {col,v0,v1};
//	(*this).m.tblBox.push_back( a );
	m.tblVect2.emplace_back( TypeBox, col );
	m.tblVect2.emplace_back( v0 );
	m.tblVect2.emplace_back( v1 );
}
//------------------------------------------------------------------------------
void SysGra::Fill( vect2 v0, vect2 v1,int col)
//------------------------------------------------------------------------------
{
//	PrimFill a = {col,v0,v1};
//	(*this).m.tblFill.push_back( a );

	m.tblVect2.emplace_back( TypeFill, col );
	m.tblVect2.emplace_back( v0 );
	m.tblVect2.emplace_back( v1 );
}
//------------------------------------------------------------------------------
void SysGra::Line( vect2 v0, vect2 v1,int col)
//------------------------------------------------------------------------------
{
//	PrimLine a = {col,v0,v1};
//	(*this).m.tblLine.push_back( a );

	m.tblVect2.emplace_back( TypeLine, col );
	m.tblVect2.emplace_back( v0 );
	m.tblVect2.emplace_back( v1 );

}
//------------------------------------------------------------------------------
void SysGra::Tri( vect2 v0, vect2 v1, vect2 v2, int col)
//------------------------------------------------------------------------------
{
//	PrimTri a = {col,v0,v1,v2};
//		(*this).m.tblTri.push_back( a );
	m.tblVect2.emplace_back( TypeTri, col );
	m.tblVect2.emplace_back( v0 );
	m.tblVect2.emplace_back( v1 );
	m.tblVect2.emplace_back( v2 );

}
/*
//------------------------------------------------------------------------------
void SysGra::Bezier( vect2 v0, vect2 v1, vect2 v2, vect2 v3, int col)
//------------------------------------------------------------------------------
{
	PrimBezier a = {col,v0,v1,v2,v3,col};
	
	(*this).m.tblBezier.push_back( a );
}
*/

