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

#include <gl/gl.h>

struct  Fontwgl
{
	const int count = 255;
	int font_base;

	HFONT	hFont;
	
	//------------------------------------------------------------------------------
	void Init( HDC& hDc )
	//------------------------------------------------------------------------------
	{
		font_base = glGenLists(255);

	#if 0	
	    int   nHeight			= 14;					// 文字セルまたは文字の高さ
	    int   nWidth			= 0;					// 平均文字幅
	    int   nEscapement		= 0;					// 文字送りの方向とX軸との角度
	    int   nOrientation		= 0;					// ベースラインとX軸との角度
	    int   nWeight			= FW_REGULAR;			// フォントの太さ
	    DWORD fdwItalic			= false;				// イタリック体指定
	    DWORD fdwUnderline		= false;				// 下線付き指定
	    DWORD fdwStrikeOut		= false;				// 打ち消し線付き指定
	    DWORD fdwCharSet		= SHIFTJIS_CHARSET;		// キャラクタセット
	    DWORD fdwOutPrecision	= OUT_DEFAULT_PRECIS;	// 出力精度
	    DWORD fdwClipPrecision	= CLIP_DEFAULT_PRECIS;	// クリッピングの精度
	    DWORD fdwQuality		= DEFAULT_QUALITY;		// 出力品質
	    DWORD fdwPitchAndFamily = 	FIXED_PITCH 		// 固定ピッチ
	    						//   VARIABLE_PITCH 	// 可変ピッチ
			    					| FF_SWISS 			// "MS Sans Serif" 
			    				//    FF_ROMAN			// "MS Serif"
			    					;
	    PCTSTR pszFaceName		= NULL;					// フォント名

		hFont = CreateFont(
			nHeight,
			nWidth,
			nEscapement,
			nOrientation,
			nWeight,
			fdwItalic,
			fdwUnderline,
			fdwStrikeOut,
			fdwCharSet,
			fdwOutPrecision,
			fdwClipPrecision,
			fdwQuality,
			fdwPitchAndFamily,
			pszFaceName
		);
		SelectObject(hDc, hFont);
	#else  
//		SelectObject (hDc, GetStockObject (SYSTEM_FONT)); 		// サンセリフ 固定
		SelectObject (hDc, GetStockObject (DEFAULT_GUI_FONT));	// サンセリフ 小

//		SelectObject (hDc, GetStockObject (SYSTEM_FIXED_FONT)); 
//		SelectObject (hDc, GetStockObject (ANSI_FIXED_FONT)); // セリフ

	#endif
		wglUseFontBitmaps( hDc, 0, count, font_base); 

	}
			//glDeleteLists( font_base, count);


	//------------------------------------------------------------------------------
	void DrawString( string str )
	//------------------------------------------------------------------------------
	{

		glPushAttrib(GL_LIST_BIT);

		glListBase( font_base );

		glCallLists( str.size(), GL_UNSIGNED_BYTE, str.c_str() );

		glPopAttrib();

	}

} font;

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

	// gl
	HGLRC hGlrc;
} g;


//------------------------------------------------------------------------------
void wgl_Enable(HWND hWnd, HDC * hDc, HGLRC * hGlrc)
//------------------------------------------------------------------------------
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;
	
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat( *hDc, &pfd );
	SetPixelFormat( *hDc, format, &pfd );
	
	// create a rendering context  
	*hGlrc = wglCreateContext( *hDc );

	// make it the calling thread's current rendering context  
	wglMakeCurrent( *hDc, *hGlrc );
	
}

//------------------------------------------------------------------------------
void wgl_Disable(HWND hWnd, HGLRC hGlrc)
//------------------------------------------------------------------------------
{
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hGlrc );
}


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

	HDC	hDc = GetDC( hWnd );	// GetDCに対してはReleaseDC
		{// gl
			wgl_Enable( hWnd, &hDc, &g.hGlrc );
		}
		{// font
			font.Init( hDc );
		}
	ReleaseDC( hWnd, hDc );

}	

//------------------------------------------------------------------------------
void  SysGra::OnDestroy() 
//------------------------------------------------------------------------------
{
	HWND hWnd = SysWin::GetInstance().win.hWnd;

	wgl_Disable( hWnd, g.hGlrc );

	DeleteDC(g.hdcBackbuffer);
	DeleteObject(g.hBitmap);

	DeleteObject(g.hfon);

	PostQuitMessage( 0 );
}

//------------------------------------------------------------------------------
void  SysGra::OnSize( int width, int height ) 
//------------------------------------------------------------------------------
{
	g.width = width;
	g.height = height;

	HWND hWnd = SysWin::GetInstance().win.hWnd;
	HDC	hDc = GetDC( hWnd );	// GetDCに対してはReleaseDC
    {
		if ( g.flgActive == true )
		{
			DeleteDC(g.hdcBackbuffer);
			DeleteObject(g.hBitmap);
		}


		g.hBitmap = CreateCompatibleBitmap( hDc, width, height );

	    g.hdcBackbuffer = CreateCompatibleDC( NULL );	// Create...DCに対してはDeleteDC
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

///------------------------------------------------------------------------------
void  SysGra::OnPaint()
//------------------------------------------------------------------------------
{
	HWND hWnd = SysWin::GetInstance().win.hWnd;
		{// WM_PAINT呼び出しのために空でも必要。
		    PAINTSTRUCT ps;
		    HDC hDc = BeginPaint(hWnd, &ps);
//		    BitBlt(hDc, 0, 0, g.width, g.height, g.hdcBackbuffer, 0, 0, SRCCOPY);
		    EndPaint(hWnd, &ps);
		}


		if(1)
		{
			HDC	hDc = GetDC( hWnd );	// GetDCに対してはReleaseDC
			SwapBuffers( hDc );
			ReleaseDC( hWnd, hDc );
		}
	
return;

#if 0
	{ // gdi
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
	} // dgi
#endif

#if 0
	{// GL / GDI
		if(0)
		{
//			HDC hDc = g.hdcBackbuffer;


			// bmp
			{
		//				StretchDIBits( hDc, 0, 0, g.rect.right, g.rect.bottom, 0, 0, m.width, m.height, m.bPixelBits, &m.bmpInfo, DIB_RGB_COLORS, SRCCOPY );

			}

			for ( unsigned int i=0 ; i < m.tblVect2.size() ; )
			{
				vect2 attr = m.tblVect2[i++];
				EnumType	type	= (EnumType)attr.x;
				int			col		= (int)attr.y;

//				HPEN hPen = CreatePen(PS_SOLID, 1, col);
//				HPEN hOldPen = SelectPen(hDc, hPen);
//				HBRUSH hBrush = CreateSolidBrush( col );
//				HBRUSH hOldBrush = SelectBrush(hDc, hBrush);

				float cb = (float)((col>>16)&0xff)/255.0f;
				float cg = (float)((col>> 8)&0xff)/255.0f;
				float cr = (float)((col>> 0)&0xff)/255.0f;

			
				switch ( type )
				{
					case TypeClr:
//						PatBlt( hDc , 0 , 0 ,g.width, g.height , PATCOPY);

						// gl
						glClearColor( cr, cg, cb, 0.0f );
						glClear( GL_COLOR_BUFFER_BIT );
						break;

					case TypeCircle:
						{
							vect2 v = m.tblVect2[i++];
							vect2 param = m.tblVect2[i++];
							float r = param.x;
							
//							{
//								int s=0;
//								vect2 v0 = v + vect2(r,0);
//								MoveToEx(hDc, (int)v0.x, (int)v0.y, NULL); 
//								for ( int i = 0 ; i < 360 ; i+=45 )
//								{
//									float th = rad(i);
//									vect2 v1 = vect2( r*cos(th), r*sin(th) )+v;
//									LineTo(hDc, (int)v1.x, (int)v1.y); 
//									s++;
//								}
//								LineTo(hDc, (int)v0.x, (int)v0.y); 
//							}

							//gl
							{
							    glColor3f( col.r, col.g, col.b );
							    glBegin(GL_LINE_LOOP);

								int s=0;
								vect2 v0 = v + vect2(r,0);
								v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
//								MoveToEx(hDc, (int)v0.x, (int)v0.y, NULL); 
								for ( int i = 0 ; i < 360 ; i+=45 )
								{
									float th = rad(i);
									vect2 v1 = vect2( r*cos(th), r*sin(th) )+v;
//									LineTo(hDc, (int)v1.x, (int)v1.y); 

									//gl
									v1 = v1 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
								    glVertex2f(v1.x, v1.y);
									s++;
								}
							    glEnd();
//								LineTo(hDc, (int)v0.x, (int)v0.y); 
							}
						}
						break;

					case TypePset:
						{
							vect2 v0 = m.tblVect2[i++];
//							SetPixel( hDc, (int)v0.x, (int)v0.y, col );

							//gl
							v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
						    glColor3f( col.r, col.g, col.b );
						    glBegin(GL_POINTS);
						    glVertex2f(v0.x, v0.y);
						    glEnd();
						}
						break;

					case TypeFill:
						{
							vect2 v0 = m.tblVect2[i++];
							vect2 v1 = m.tblVect2[i++];
//							Rectangle(hDc , (int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y); 

							//gl
							v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
							v1 = v1 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
						    glColor3f( col.r, col.g, col.b );
						    glBegin(GL_QUADS);
						    glVertex2f(v1.x, v0.y);
						    glVertex2f(v1.x, v1.y);
						    glVertex2f(v0.x, v1.y);
						    glVertex2f(v0.x, v0.y);
						    glEnd();
						}
						break;
					
					case TypeLine: 
						{
							vect2 v0 = m.tblVect2[i++];
							vect2 v1 = m.tblVect2[i++];
//							MoveToEx(hDc, (int)v0.x, (int)v0.y, NULL); 
//							LineTo(hDc, (int)v1.x, (int)v1.y); 

							//gl
							v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
							v1 = v1 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
						    glColor3f( col.r, col.g, col.b );
						    glBegin(GL_LINES);
						    glVertex2f(v0.x, v0.y);
						    glVertex2f(v1.x, v1.y);
						    glEnd();
						}
						break;

					case TypeBox:
						{
							vect2 v0 = m.tblVect2[i++];
							vect2 v1 = m.tblVect2[i++];
//							MoveToEx(hDc, (int)v0.x, (int)v0.y, NULL);
//							LineTo(hDc, (int)v1.x, (int)v0.y);
//							LineTo(hDc, (int)v1.x, (int)v1.y);
//							LineTo(hDc, (int)v0.x, (int)v1.y);
//							LineTo(hDc, (int)v0.x, (int)v0.y);

							//gl
							v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
							v1 = v1 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
						    glColor3f( col.r, col.g, col.b );
						    glBegin(GL_LINE_LOOP);
						    glVertex2f(v1.x, v0.y);
						    glVertex2f(v1.x, v1.y);
						    glVertex2f(v0.x, v1.y);
						    glVertex2f(v0.x, v0.y);
						    glEnd();

						}
						break;

					case TypeTri:
						{	
							vect2 v0 = m.tblVect2[i++];
							vect2 v1 = m.tblVect2[i++];
							vect2 v2 = m.tblVect2[i++];
//							POINT	tblPoint[3]={{(int)v0.x,(int)v0.y} , {(int)v1.x,(int)v1.y} , {(int)v2.x,(int)v2.y} };
//							INT	tblNum[]={3};
//							PolyPolygon( hDc, tblPoint, tblNum, 1 );;

							//gl
							v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
							v1 = v1 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
							v2 = v2 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
						    glColor3f( col.r, col.g, col.b );
						    glBegin( GL_TRIANGLES );
						    glVertex2f(v0.x, v0.y);
						    glVertex2f(v1.x, v1.y);
						    glVertex2f(v2.x, v2.y);
						    glEnd();

	
						}
						break;

				}
	        
//				SelectBrush(hDc, hOldBrush);
//				DeleteObject(hBrush);
//
//				SelectPen(hDc, hOldPen);
//				DeleteObject(hPen);
			}
			

			{
//		        SetTextColor(hDc, RGB(0xff, 0xff, 0xff));  // 文字色を設定
//		        SetBkColor(hDc, RGB(0xf, 0xf, 0xf));    // 背景色を設定
//		        SetBkMode( hDc, TRANSPARENT ); // 背景を塗りつぶさない
//		        g.hfonPrev = (HFONT) SelectObject(hDc, g.hfon);  // フォントを選択

			    glBegin(GL_POINTS);
			    glColor3f( 1,1,1 );
			    glEnd();

				for ( Message& m : m.tblMessage )
		        {
					vect2 v0 = m.pos;
		        	const char* str = m.str.c_str();
//			        TextOut(hDc, (int)v0.x, (int)v0.y, str, lstrlen(str));

					//gl
					v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
					glRasterPos2f(v0.x, v0.y);
					font.DrawString( str );
				}
				
//		        SelectObject(hDc, g.hfonPrev);      
			}



		}



		{// WM_PAINT呼び出しのために空でも必要。
		    PAINTSTRUCT ps;
		    HDC hDc = BeginPaint(hWnd, &ps);
//		    BitBlt(hDc, 0, 0, g.width, g.height, g.hdcBackbuffer, 0, 0, SRCCOPY);
		    EndPaint(hWnd, &ps);
		}


		if(1)
		{
			HDC	hDc = GetDC( hWnd );	// GetDCに対してはReleaseDC
			SwapBuffers( hDc );
			ReleaseDC( hWnd, hDc );
		}
	}
#endif

#if 0
	if(0)
	{
		static int lim =60;
		for ( Message& m : m.tblMessage )
        {
        	const char* str = m.str.c_str();
			if(1)
			{
				if ( lim == 0 ) 
				{
					cout << str << endl;
				}
			}
		}
		if ( lim-- == 0 ) lim = 60*3;
	}

	m.tblVect2.clear();
	m.tblMessage.clear();
#endif

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



/*
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
*/
//------------------------------------------------------------------------------
void SysGra::Update()
//------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------
void SysGra::Clr( vect3 col)
//------------------------------------------------------------------------------
{
//	m.tblVect2.emplace_back( TypeClr, col );
				//float cb = (float)((col>>16)&0xff)/255.0f;
				//float cg = (float)((col>> 8)&0xff)/255.0f;
				//float cr = (float)((col>> 0)&0xff)/255.0f;

						glClearColor( col.r, col.g, col.b, 0.0f );
						glClear( GL_COLOR_BUFFER_BIT );

}
//------------------------------------------------------------------------------
void SysGra::Circle( vect2 v, float r, vect3 col )
//------------------------------------------------------------------------------
{
//	m.tblVect2.emplace_back( TypeCircle, col );
//	m.tblVect2.emplace_back( v0 );
//	m.tblVect2.emplace_back( r, 8.0  ); // 半径 , 分割数

				//float cb = (float)((col>>16)&0xff)/255.0f;
				//float cg = (float)((col>> 8)&0xff)/255.0f;
				//float cr = (float)((col>> 0)&0xff)/255.0f;

							{
							    glColor3f( col.r, col.g, col.b );
							    glBegin(GL_LINE_LOOP);

								int s=0;
								vect2 v0 = v + vect2(r,0);
								v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
//								MoveToEx(hDc, (int)v0.x, (int)v0.y, NULL); 
								for ( int i = 0 ; i < 360 ; i+=45 )
								{
									float th = rad(i);
									vect2 v1 = vect2( r*cos(th), r*sin(th) )+v;
//									LineTo(hDc, (int)v1.x, (int)v1.y); 

									//gl
									v1 = v1 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
								    glVertex2f(v1.x, v1.y);
									s++;
								}
							    glEnd();
//								LineTo(hDc, (int)v0.x, (int)v0.y); 
							}

}
//------------------------------------------------------------------------------
void SysGra::Pset( vect2 v0, vect3 col )
//------------------------------------------------------------------------------
{
//	m.tblVect2.emplace_back( TypePset, col );
//	m.tblVect2.emplace_back( v0 );
				//float cb = (float)((col>>16)&0xff)/255.0f;
				//float cg = (float)((col>> 8)&0xff)/255.0f;
				//float cr = (float)((col>> 0)&0xff)/255.0f;

							v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
						    glColor3f( col.r, col.g, col.b );
						    glBegin(GL_POINTS);
						    glVertex2f(v0.x, v0.y);
						    glEnd();
}
//------------------------------------------------------------------------------
void SysGra::Box( vect2 v0, vect2 v1,vect3 col)
//------------------------------------------------------------------------------
{
//	m.tblVect2.emplace_back( TypeBox, col );
//	m.tblVect2.emplace_back( v0 );
//	m.tblVect2.emplace_back( v1 );
				//float cb = (float)((col>>16)&0xff)/255.0f;
				//float cg = (float)((col>> 8)&0xff)/255.0f;
				//float cr = (float)((col>> 0)&0xff)/255.0f;

							v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
							v1 = v1 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
						    glColor3f( col.r, col.g, col.b );
						    glBegin(GL_LINE_LOOP);
						    glVertex2f(v1.x, v0.y);
						    glVertex2f(v1.x, v1.y);
						    glVertex2f(v0.x, v1.y);
						    glVertex2f(v0.x, v0.y);
						    glEnd();

}
//------------------------------------------------------------------------------
void SysGra::Fill( vect2 v0, vect2 v1,vect3 col)
//------------------------------------------------------------------------------
{
//	m.tblVect2.emplace_back( TypeFill, col );
//	m.tblVect2.emplace_back( v0 );
//	m.tblVect2.emplace_back( v1 );
				//float cb = (float)((col>>16)&0xff)/255.0f;
				//float cg = (float)((col>> 8)&0xff)/255.0f;
				//float cr = (float)((col>> 0)&0xff)/255.0f;

							v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
							v1 = v1 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
						    glColor3f( col.r, col.g, col.b );
						    glBegin(GL_QUADS);
						    glVertex2f(v1.x, v0.y);
						    glVertex2f(v1.x, v1.y);
						    glVertex2f(v0.x, v1.y);
						    glVertex2f(v0.x, v0.y);
						    glEnd();

}
//------------------------------------------------------------------------------
void SysGra::Line( vect2 v0, vect2 v1,vect3 col)
//------------------------------------------------------------------------------
{
#if 0
	m.tblVect2.emplace_back( TypeLine, col );
	m.tblVect2.emplace_back( v0 );
	m.tblVect2.emplace_back( v1 );
#else
				//float cb = (float)((col>>16)&0xff)/255.0f;
				//float cg = (float)((col>> 8)&0xff)/255.0f;
				//float cr = (float)((col>> 0)&0xff)/255.0f;
							v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
							v1 = v1 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
						    glColor3f( col.r, col.g, col.b );
						    glBegin(GL_LINES);
						    glVertex2f(v0.x, v0.y);
						    glVertex2f(v1.x, v1.y);
						    glEnd();
#endif
}
//------------------------------------------------------------------------------
void SysGra::Tri( vect2 v0, vect2 v1, vect2 v2, vect3 col)
//------------------------------------------------------------------------------
{
#if 0
	m.tblVect2.emplace_back( TypeTri, col );
	m.tblVect2.emplace_back( v0 );
	m.tblVect2.emplace_back( v1 );
	m.tblVect2.emplace_back( v2 );
#else
				//float cb = (float)((col>>16)&0xff)/255.0f;
				//float cg = (float)((col>> 8)&0xff)/255.0f;
				//float cr = (float)((col>> 0)&0xff)/255.0f;

							v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
							v1 = v1 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
							v2 = v2 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
						    glColor3f( col.r, col.g, col.b );
						    glBegin( GL_TRIANGLES );
						    glVertex2f(v0.x, v0.y);
						    glVertex2f(v1.x, v1.y);
						    glVertex2f(v2.x, v2.y);
						    glEnd();
#endif

}
//------------------------------------------------------------------------------
void SysGra::Print( vect2 v0, string str )
//------------------------------------------------------------------------------
{
//	m.tblMessage.emplace_back( str, pos );

if(1)
			{
//		        SetTextColor(hDc, RGB(0xff, 0xff, 0xff));  // 文字色を設定
//		        SetBkColor(hDc, RGB(0xf, 0xf, 0xf));    // 背景色を設定
//		        SetBkMode( hDc, TRANSPARENT ); // 背景を塗りつぶさない
//2		        g.hfonPrev = (HFONT) SelectObject(hDc, g.hfon);  // フォントを選択

			    glBegin(GL_POINTS);
			    glColor3f( 1,1,1 );
			    glEnd();

	//			for ( Message& m : m.tblMessage )
		        {
	//				vect2 v0 = m.pos;
//		        	const char* str = m.str.c_str();
//			        TextOut(hDc, (int)v0.x, (int)v0.y, str, lstrlen(str));

					//gl
					v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
					glRasterPos2f(v0.x, v0.y);
					font.DrawString( str );
				}
				
//		        SelectObject(hDc, g.hfonPrev);      
			}
}
