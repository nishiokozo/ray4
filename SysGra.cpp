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
	HDC  hdcBackbuffer;
	HBITMAP hBitmap;
	HFONT hfon, hfonPrev;
	vector<string>tblString;
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
//	g.hfon = (HFONT) GetStockObject(OEM_FIXED_FONT); // 固定幅フォント標準
//	g.hfon = (HFONT) GetStockObject(ANSI_FIXED_FONT); // 固定幅フォント中明朝
	g.hfon = (HFONT) GetStockObject(DEFAULT_GUI_FONT); // 可変長小
}	

//------------------------------------------------------------------------------
void  SysGra::OnSize( int width, int height ) 
//------------------------------------------------------------------------------
{
	g.width = width;
	g.height = height;

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
	}


}

//------------------------------------------------------------------------------
void  SysGra::OnPaint()
//------------------------------------------------------------------------------
{
	HWND hWnd = SysWin::GetInstance().win.hWnd;

	{
		HDC hDc = g.hdcBackbuffer;


		// bmp
		{
	//				StretchDIBits( hDc, 0, 0, g.rect.right, g.rect.bottom, 0, 0, m.width, m.height, m.bPixelBits, &m.bmpInfo, DIB_RGB_COLORS, SRCCOPY );

		}

		
		for ( unsigned int i=0 ; i < m.tblVect2.size() ; )
		{
			vect2 attr = m.tblVect2[i++];
			EnumType	type	= (EnumType)attr.x;
			int			col		= (int)attr.y;

			HPEN hPen = CreatePen(PS_SOLID, 1, col);
			HPEN hOldPen = SelectPen(hDc, hPen);
			HBRUSH hBrush = CreateSolidBrush( col );
			HBRUSH hOldBrush = SelectBrush(hDc, hBrush);


		
			switch ( type )
			{
				case TypeClr:
					PatBlt( hDc , 0 , 0 ,g.width, g.height , PATCOPY);
					break;

/*
				case TypeCircleOld:
					{
						vect2 v0 = m.tblVect2[i++];
						vect2 v1 = m.tblVect2[i++];
						Ellipse(hDc , v0.x, v0.y, v1.x, v1.y);
					}
					break;
*/
				case TypeCircle:
					{
						vect2 v = m.tblVect2[i++];
						vect2 param = m.tblVect2[i++];
						float r = param.x;
						
						int s=0;
						vect2 v0 = v + vect2(r,0);
						MoveToEx(hDc, (int)v0.x, (int)v0.y, NULL); 
						for ( int i = 0 ; i < 360 ; i+=45 )
						{
							float th = rad(i);
							vect2 v1 = vect2( r*cos(th), r*sin(th) )+v;
							LineTo(hDc, (int)v1.x, (int)v1.y); 
							s++;
						}
						LineTo(hDc, (int)v0.x, (int)v0.y); 
					}
					break;

				case TypePset:
					{
						vect2 v0 = m.tblVect2[i++];
						SetPixel( hDc, (int)v0.x, (int)v0.y, col );
					}
					break;

				case TypeFill:
					{
						vect2 v0 = m.tblVect2[i++];
						vect2 v1 = m.tblVect2[i++];
						Rectangle(hDc , (int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y); 
					}
					break;
				
				case TypeLine: 
					{
						vect2 v0 = m.tblVect2[i++];
						vect2 v1 = m.tblVect2[i++];
						MoveToEx(hDc, (int)v0.x, (int)v0.y, NULL); 
						LineTo(hDc, (int)v1.x, (int)v1.y); 
					}
					break;

				case TypeBox:
					{
						vect2 v0 = m.tblVect2[i++];
						vect2 v1 = m.tblVect2[i++];
						MoveToEx(hDc, (int)v0.x, (int)v0.y, NULL);
						LineTo(hDc, (int)v1.x, (int)v0.y);
						LineTo(hDc, (int)v1.x, (int)v1.y);
						LineTo(hDc, (int)v0.x, (int)v1.y);
						LineTo(hDc, (int)v0.x, (int)v0.y);
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
		

		{
	        SetTextColor(hDc, RGB(0xff, 0xff, 0xff));  // 文字色を設定
	        SetBkColor(hDc, RGB(0xf, 0xf, 0xf));    // 背景色を設定
	        SetBkMode( hDc, TRANSPARENT ); // 背景を塗りつぶさない
	        g.hfonPrev = (HFONT) SelectObject(hDc, g.hfon);  // フォントを選択
//			int y = 0;
			for ( Message& m : m.tblMessage )
	        {
	        	const char* str = m.str.c_str();
	        	int x = (int)m.pos.x;
	        	int y = (int)m.pos.y;
		        TextOut(hDc, x, y, str, lstrlen(str));
//				y+= 16;
			}
	        SelectObject(hDc, g.hfonPrev);      
		}


	}



	{
	    PAINTSTRUCT ps;
	    HDC hDc = BeginPaint(hWnd, &ps);
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

	DeleteObject(g.hfon);

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
int	SysGra::Rgb( float r, float g , float b )
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

	m.tblVect2.clear();
	m.tblMessage.clear();
}
//------------------------------------------------------------------------------
void SysGra::Clr( int col)
//------------------------------------------------------------------------------
{
	m.tblVect2.emplace_back( TypeClr, col );
}
/*
//------------------------------------------------------------------------------
void SysGra::Circle( vect2 v, float r, int col )
//------------------------------------------------------------------------------
{
	m.tblVect2.emplace_back( TypeCircleOld, col );
	m.tblVect2.emplace_back( v.x-r, v.y-r );
	m.tblVect2.emplace_back( v.x+r, v.y+r );

}
*/
//------------------------------------------------------------------------------
void SysGra::Circle( vect2 v0, float r, int col )
//------------------------------------------------------------------------------
{
	m.tblVect2.emplace_back( TypeCircle, col );
	m.tblVect2.emplace_back( v0 );
	m.tblVect2.emplace_back( r, 8.0  ); // 半径 , 分割数

}
//------------------------------------------------------------------------------
void SysGra::Pset( vect2 v0, int col )
//------------------------------------------------------------------------------
{
	m.tblVect2.emplace_back( TypePset, col );
	m.tblVect2.emplace_back( v0 );
}
//------------------------------------------------------------------------------
void SysGra::Box( vect2 v0, vect2 v1,int col)
//------------------------------------------------------------------------------
{
	m.tblVect2.emplace_back( TypeBox, col );
	m.tblVect2.emplace_back( v0 );
	m.tblVect2.emplace_back( v1 );
}
//------------------------------------------------------------------------------
void SysGra::Fill( vect2 v0, vect2 v1,int col)
//------------------------------------------------------------------------------
{
	m.tblVect2.emplace_back( TypeFill, col );
	m.tblVect2.emplace_back( v0 );
	m.tblVect2.emplace_back( v1 );
}
//------------------------------------------------------------------------------
void SysGra::Line( vect2 v0, vect2 v1,int col)
//------------------------------------------------------------------------------
{
	m.tblVect2.emplace_back( TypeLine, col );
	m.tblVect2.emplace_back( v0 );
	m.tblVect2.emplace_back( v1 );
}
//------------------------------------------------------------------------------
void SysGra::Tri( vect2 v0, vect2 v1, vect2 v2, int col)
//------------------------------------------------------------------------------
{
	m.tblVect2.emplace_back( TypeTri, col );
	m.tblVect2.emplace_back( v0 );
	m.tblVect2.emplace_back( v1 );
	m.tblVect2.emplace_back( v2 );

}
//------------------------------------------------------------------------------
void SysGra::Print( vect2 pos, string str )
//------------------------------------------------------------------------------
{
	m.tblMessage.emplace_back( str, pos );
}
