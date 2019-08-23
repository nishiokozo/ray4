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

struct  wgl_Font
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
	//------------------------------------------------------------------------------
	void Delete( )
	//------------------------------------------------------------------------------
	{
		glDeleteLists( font_base, count);
	}


	//------------------------------------------------------------------------------
	void DrawString( string str )
	//------------------------------------------------------------------------------
	{

		glPushAttrib(GL_LIST_BIT);

		glListBase( font_base );

		glCallLists( str.size(), GL_UNSIGNED_BYTE, str.c_str() );

		glPopAttrib();

	}

} wgl_font;

static struct G
{
	bool flgActive;
	HDC  hdcBackbuffer;
	HBITMAP hBitmap;
	HFONT hfon, hfonPrev;
	vector<string>tblString;
	int	width;
	int height;
	G()
	{
		flgActive = false;
	}

	// gl
	bool gl_bInitialized = false;
	HGLRC hGlrc;
} g;


//------------------------------------------------------------------------------
void wgl_Enable( HDC * hDc, HGLRC * hGlrc)
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
void wgl_Disable( HGLRC hGlrc)
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

	return win.GetHeight();
}

//------------------------------------------------------------------------------
float SysGra::GetAspect()
//------------------------------------------------------------------------------
{
	SysWin& win = SysWin::GetInstance();

	return (float)win.GetWidth()/win.GetHeight();
}

//------------------------------------------------------------------------------
void  SysGra::OnCreate() 
//------------------------------------------------------------------------------
{
	HWND hWnd = SysWin::GetInstance().win.hWnd;
//	g.hfon = (HFONT) GetStockObject(OEM_FIXED_FONT); // 固定幅フォント標準
//	g.hfon = (HFONT) GetStockObject(ANSI_FIXED_FONT); // 固定幅フォント中明朝
	g.hfon = (HFONT) GetStockObject(DEFAULT_GUI_FONT); // 可変長小

	{
		HDC	hDc = GetDC( hWnd );	// GetDCに対してはReleaseDC

			if ( g.gl_bInitialized == false )
			{// gl
				wgl_Enable( &hDc, &g.hGlrc );
				wgl_font.Init( hDc );
				g.gl_bInitialized = true;
			}
		ReleaseDC( hWnd, hDc );
	}

}	

//------------------------------------------------------------------------------
void  SysGra::OnDestroy() 
//------------------------------------------------------------------------------
{

	if( g.gl_bInitialized )
	{
		wgl_font.Delete();
		wgl_Disable( g.hGlrc );
	}

	DeleteDC(g.hdcBackbuffer);
	DeleteObject(g.hBitmap);

	DeleteObject(g.hfon);

	PostQuitMessage( 0 );
}

//------------------------------------------------------------------------------
void  SysGra::OnSize( int width, int height ) 
//------------------------------------------------------------------------------
{


	HWND hWnd = SysWin::GetInstance().win.hWnd;
	{
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

	// gl
#if 1
		glViewport(0,0,width, height);
#else
	{
		HDC	hDc = GetDC( hWnd );	// GetDCに対してはReleaseDC
		if( g.gl_bInitialized )
		{
			wgl_font.Delete();
			wgl_Disable( g.hGlrc );
		}
		{
			wgl_Enable( &hDc, &g.hGlrc );
			wgl_font.Init( hDc );
		}
		ReleaseDC( hWnd, hDc );

	}
#endif
}

//------------------------------------------------------------------------------
void  SysGra::OnMove( int pos_x, int pos_y ) 
//------------------------------------------------------------------------------
{


	{
		SysWin& win = SysWin::GetInstance();
		int w = win.GetWidth();
		int h = win.GetHeight();

	
		RECT rect;
		SetRect(&rect, 0, 0, w, h );
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
	    EndPaint(hWnd, &ps);
	}


	{
		HDC	hDc = GetDC( hWnd );	// GetDCに対してはReleaseDC
		SwapBuffers( hDc );
		ReleaseDC( hWnd, hDc );
	}

}


//------------------------------------------------------------------------------
void SysGra::Update()
//------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------
void SysGra::Clr( vect3 col)
//------------------------------------------------------------------------------
{
//	カラークリア値
	glClearColor( col.r, col.g, col.b, 0.0f );

//	深度クリア値
	glClearDepth(0.0);			// (デフォルト;1.0）

//	深度テスト
	glEnable(GL_DEPTH_TEST);	// (デフォルト:GL_LESS）
	glDepthFunc(GL_GEQUAL);		// depth <= 書き込み値 

//	裏面カリング
	glEnable(GL_CULL_FACE);		// 時計回りが裏、反時計回りが表
	glCullFace(GL_BACK);		// (デフォルト:GL_BACK）

//	ペイント
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

}
//------------------------------------------------------------------------------
void SysGra::Circle( vect2 v, float r, vect3 col )
//------------------------------------------------------------------------------
{
	glDisable(GL_DEPTH_TEST);

	{
	    glColor3f( col.r, col.g, col.b );
	    glBegin(GL_LINE_LOOP);

		int s=0;
		vect2 v0 = v + vect2(r,0);
		v0 = v0 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
		for ( int i = 0 ; i < 360 ; i+=45 )
		{
			float th = rad(i);
			vect2 v1 = vect2( r*cos(th), r*sin(th) )+v;

			//gl
			v1 = v1 / vect2(768/2,-512/2) -vect2(1.0f, -1.0f);
		    glVertex2f(v1.x, v1.y);
			s++;
		}
	    glEnd();
	}

}
//------------------------------------------------------------------------------
void SysGra::Pset( vect2 v0, vect3 col, float wide )
//------------------------------------------------------------------------------
{
	glDisable(GL_DEPTH_TEST);

	glPointSize(wide);
    glBegin(GL_POINTS);
    glColor3f( col.r, col.g, col.b );
    glVertex2f(v0.x, v0.y);
    glEnd();
}
//------------------------------------------------------------------------------
void SysGra::Pset( vect3 v0, vect3 col, float wide )
//------------------------------------------------------------------------------
{
	glDisable(GL_DEPTH_TEST);

	glPointSize(wide);
    glBegin(GL_POINTS);
    glColor3f( col.r, col.g, col.b );
    glVertex3f(v0.x, v0.y, v0.z);
    glEnd();
}
//------------------------------------------------------------------------------
void SysGra::Box( vect2 v0, vect2 v1,vect3 col)
//------------------------------------------------------------------------------
{
	glDisable(GL_DEPTH_TEST);

    glBegin(GL_LINE_LOOP);
    glColor3f( col.r, col.g, col.b );
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
	glDisable(GL_DEPTH_TEST);

    glBegin(GL_QUADS);
    glColor3f( col.r, col.g, col.b );
    glVertex2f(v1.x, v1.y);
    glVertex2f(v1.x, v0.y);
    glVertex2f(v0.x, v0.y);
    glVertex2f(v0.x, v1.y);
    glEnd();

}
//------------------------------------------------------------------------------
void SysGra::Line( vect2 v0, vect2 v1,vect3 col)
//------------------------------------------------------------------------------
{
	glDisable(GL_DEPTH_TEST);

    glBegin(GL_LINES);
    glColor3f( col.r, col.g, col.b );
    glVertex2f(v0.x, v0.y);
    glVertex2f(v1.x, v1.y);
    glEnd();
}
//------------------------------------------------------------------------------
void SysGra::Tri( vect2 v0, vect2 v1, vect2 v2, vect3 col)
//------------------------------------------------------------------------------
{
	glDisable(GL_DEPTH_TEST);

    glBegin( GL_TRIANGLES );
    glColor3f( col.r, col.g, col.b );
    glVertex2f(v0.x, v0.y);
    glVertex2f(v1.x, v1.y);
    glVertex2f(v2.x, v2.y);
    glEnd();
}
//------------------------------------------------------------------------------
void SysGra::Tri( vect3 v0, vect3 v1, vect3 v2, vect3 col)
 //------------------------------------------------------------------------------
{
	glEnable(GL_DEPTH_TEST);	// (デフォルト:GL_LESS）

    glBegin( GL_TRIANGLES );
    glColor3f( col.r, col.g, col.b );
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glEnd();
}
//------------------------------------------------------------------------------
void SysGra::Print( vect2 v0, string str )
//------------------------------------------------------------------------------
{
	glDisable(GL_DEPTH_TEST);

	glBegin(GL_POINTS);
	glColor3f( 1,1,1 );
	glEnd();

	glRasterPos2f(v0.x, v0.y);
	wgl_font.DrawString( str );
}

