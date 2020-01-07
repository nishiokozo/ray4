//2019/07/12


#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>       // sleep_for


#include <windows.h>
#include <windowsx.h>
#include <functional>
#include "SysGra.h"
#include "SysWin.h"

#include <gl/gl.h>

using namespace std;

#define	DEPTHTEST_ENABLE()			glEnable( GL_DEPTH_TEST )
#define	DEPTHTEST_DISABLE()			glDisable( GL_DEPTH_TEST )
#define	DEPTHTEST_WRITE_GREATER()	glDepthFunc( GL_GEQUAL )	// ENABLEのときだけ depth <= 書き込み値
#define	DEPTHTEST_WRITE_ALWAYS()	glDepthFunc( GL_ALWAYS )	// ENABLEのときだけ 必ず書く
#define	DEPTHTEST_WRITE_NEVER()		glDepthFunc( GL_NEVER )		// ENABLEのときだけ 書き込まない

struct SysGra::Impl
{
	struct
	{
		const DWORD count = 255;
		GLuint font_base;

		HFONT	hFont;
		
		//------------------------------------------------------------------------------
		void Init( HDC& hDc )
		//------------------------------------------------------------------------------
		{
			font_base = glGenLists(255);	

		#if 1
			//	固定長フォント	
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
				    				//	| FF_SWISS 			// "MS Sans Serif" 
	//			    				  | FF_ROMAN			// "MS Serif" 全角日本語
	//			    				  | FF_MODERN			// 全角日本語 対応
				    				  | FF_SCRIPT 			// 全角日本語 対応
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
	//		SelectObject (hDc, GetStockObject (SYSTEM_FONT)); 		// サンセリフ 大
	//		SelectObject (hDc, GetStockObject (DEFAULT_GUI_FONT));	// サンセリフ 小

	//		SelectObject (hDc, GetStockObject (SYSTEM_FIXED_FONT)); // 固定長フォント ワイド
			SelectObject (hDc, GetStockObject (ANSI_FIXED_FONT)); // 固定長フォント セリフ 外国製っぽい

		#endif
			wglUseFontBitmaps( hDc, 0, count, font_base); 

		}
		//------------------------------------------------------------------------------
		void Delete( )
		//------------------------------------------------------------------------------
		{
			glDeleteLists( font_base, (GLsizei)count);
		}


		//------------------------------------------------------------------------------
		void DrawString( string str )
		//------------------------------------------------------------------------------
		{

			glPushAttrib(GL_LIST_BIT);

			glListBase( font_base );

			glCallLists( (signed)str.size(), GL_UNSIGNED_BYTE, str.c_str() );

			glPopAttrib();

		}

	} wgl_font;

	struct G
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

	chrono::system_clock::duration time_st  = chrono::seconds(0);
	chrono::system_clock::duration time_en  = chrono::seconds(0);
	chrono::system_clock::duration time_sec = chrono::seconds(0);
	chrono::system_clock::duration time_now = chrono::seconds(0);
	chrono::system_clock::duration time_max = chrono::seconds(0);

};
//------------------------------------------------------------------------------
SysGra::~SysGra()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
SysGra::SysGra() : pImpl( new SysGra::Impl )
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void wgl_Enable( HDC * hDc, HGLRC * hGlrc)
//------------------------------------------------------------------------------
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;
	
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;// | PFD_DOUBLEBUFFER;
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
float SysGra::GetWidth()
//------------------------------------------------------------------------------
{
	SysWin& win = SysWin::GetInstance();

	return (float)win.GetWidth();
}

//------------------------------------------------------------------------------
float SysGra::GetHeight()
//------------------------------------------------------------------------------
{
	SysWin& win = SysWin::GetInstance();

	return (float)win.GetHeight();
}

//------------------------------------------------------------------------------
float SysGra::GetAspect()
//------------------------------------------------------------------------------
{
	SysWin& win = SysWin::GetInstance();

	return (float)win.GetWidth()/(float)win.GetHeight();
}

//------------------------------------------------------------------------------
void  SysGra::OnCreate() 
//------------------------------------------------------------------------------
{
	HWND hWnd = SysWin::GetInstance().win.hWnd;
//	pImpl->g.hfon = (HFONT) GetStockObject(OEM_FIXED_FONT); // 固定幅フォント標準
//	pImpl->g.hfon = (HFONT) GetStockObject(ANSI_FIXED_FONT); // 固定幅フォント中明朝
	pImpl->g.hfon = (HFONT) GetStockObject(DEFAULT_GUI_FONT); // 可変長小

	{
		HDC	hDc = GetDC( hWnd );	// GetDCに対してはReleaseDC

			if ( pImpl->g.gl_bInitialized == false )
			{// gl
				wgl_Enable( &hDc, &pImpl->g.hGlrc );
				pImpl->wgl_font.Init( hDc );
				pImpl->g.gl_bInitialized = true;
			}
		ReleaseDC( hWnd, hDc );
	}

}	

//------------------------------------------------------------------------------
void  SysGra::OnDestroy() 
//------------------------------------------------------------------------------
{

	if( pImpl->g.gl_bInitialized )
	{
		pImpl->wgl_font.Delete();
		wgl_Disable( pImpl->g.hGlrc );
	}

	DeleteDC(pImpl->g.hdcBackbuffer);
	DeleteObject(pImpl->g.hBitmap);

	DeleteObject(pImpl->g.hfon);

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
			if ( pImpl->g.flgActive == true )
			{
				DeleteDC(pImpl->g.hdcBackbuffer);
				DeleteObject(pImpl->g.hBitmap);
			}

			pImpl->g.hBitmap = CreateCompatibleBitmap( hDc, width, height );

		    pImpl->g.hdcBackbuffer = CreateCompatibleDC( NULL );	// Create...DCに対してはDeleteDC
		    SelectObject( pImpl->g.hdcBackbuffer, pImpl->g.hBitmap );
			pImpl->g.flgActive=true;
		}
		ReleaseDC( hWnd, hDc );
	}

	glViewport(0,0,width, height);
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
	    BeginPaint(hWnd, &ps);
	    EndPaint(hWnd, &ps);
	}

#if 0
	// glFlushに切り替えたので呼び出さない
	{
		HDC	hDc = GetDC( hWnd );	// GetDCに対してはReleaseDC
		SwapBuffers( hDc );
		ReleaseDC( hWnd, hDc );
	}
#endif

}


//------------------------------------------------------------------------------
void SysGra::Wait()
//------------------------------------------------------------------------------
{
	{
		//=================================
		// 処理時間計算
		//=================================

		// 同期(60fps)
		{
			int t=300;	
			while( chrono::system_clock::now().time_since_epoch()-pImpl->time_st < chrono::microseconds(16000) )
			{
 				this_thread::sleep_for(chrono::microseconds(t));
			}
		}
		pImpl->time_en = chrono::system_clock::now().time_since_epoch(); 
		if ( pImpl->time_max < pImpl->time_en-pImpl->time_st ) pImpl->time_max = pImpl->time_en-pImpl->time_st;
		pImpl->time_st = chrono::system_clock::now().time_since_epoch();  

		// 表示
	    pImpl->time_now = chrono::system_clock::now().time_since_epoch();
		if ( pImpl->time_now-pImpl->time_sec > chrono::milliseconds(500) )	// 一秒更新
		{
			pImpl->time_sec = chrono::system_clock::now().time_since_epoch();
			time_peak = chrono::duration_cast<chrono::microseconds>(pImpl->time_max).count();
			pImpl->time_max = chrono::seconds(0);
		//	cout<<pImpl->time_peak/1000.0<<"ms"<<endl;
		}
	}

//	glFlush();	//SwapBufferだと定期的に処理落ちが避けられないのでglFlush
}
//------------------------------------------------------------------------------
void SysGra::Flush()
//------------------------------------------------------------------------------
{
	glFlush();	//SwapBufferだと定期的に処理落ちが避けられないのでglFlush

	glFinish();	// GLコマンド完了待ち
}
//------------------------------------------------------------------------------
void SysGra::Clr( rgb col )
//------------------------------------------------------------------------------
{
	//	カラークリア値
	glClearColor( col.r, col.g, col.b, 0.0f );

	//	深度クリア値
	glClearDepth(0.0);			// (デフォルト;1.0）

	//	深度テスト
	DEPTHTEST_ENABLE();
	DEPTHTEST_WRITE_GREATER(); 

	//	裏面カリング
	glEnable(GL_CULL_FACE);		// 時計回りが裏、反時計回りが表
	glCullFace(GL_BACK);		// (デフォルト:GL_BACK）

	//	ペイント
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

}
//------------------------------------------------------------------------------
void SysGra::Circle( vect2 v, float r, rgb col, float wide )
//------------------------------------------------------------------------------
{
	DEPTHTEST_WRITE_ALWAYS();

	float aspect = GetAspect();

    glColor3f( col.r, col.g, col.b );
    glBegin(GL_LINE_LOOP);

	float step = 12.0;
//	vect2 v0 = v + vect2(r/aspect,0);
	for ( float th = 0 ; th < 2*pi ; th+=2.0*pi/step )
	{
		vect2 v1 = vect2( r*cos(th)/aspect, r*sin(th) )+v;

	    glVertex2f(v1.x, v1.y);
	}
    glEnd();

	DEPTHTEST_WRITE_GREATER(); 
}

//------------------------------------------------------------------------------
void SysGra::Circle( vect3 v, float r, rgb col, float wide )
//------------------------------------------------------------------------------
{
	float aspect = GetAspect();

    glColor3f( col.r, col.g, col.b );
  	glLineWidth( wide );

    glBegin(GL_LINE_LOOP);

	float step = 24.0;
//	vect3 v0 = v + vect3(r/aspect,0,0);
	for ( float th = 0 ; th < 2*pi ; th+=2.0*pi/step )
	{
		vect3 v1 = vect3( r*cos(th)/aspect, r*sin(th), 0 )+v;

	    glVertex3f(v1.x, v1.y, v1.z);
	}
    glEnd();
}

//------------------------------------------------------------------------------
void SysGra::Pset( vect2 v0, rgb col, float wide )
//------------------------------------------------------------------------------
{
	DEPTHTEST_WRITE_ALWAYS();

	glPointSize( wide );
    glBegin(GL_POINTS);
    glColor3f( col.r, col.g, col.b );
    glVertex2f(v0.x, v0.y);
    glEnd();

	DEPTHTEST_WRITE_GREATER(); 
}
//------------------------------------------------------------------------------
void SysGra::Pset( vect3 v0, rgb col, float wide )
//------------------------------------------------------------------------------
{
	glPointSize( wide );
    glBegin(GL_POINTS);
    glColor3f( col.r, col.g, col.b );
    glVertex3f(v0.x, v0.y, v0.z);
    glEnd();
}
//------------------------------------------------------------------------------
void SysGra::Box( vect2 v0, vect2 v1,rgb col, float wide )
//------------------------------------------------------------------------------
{
	DEPTHTEST_WRITE_ALWAYS();

  	glLineWidth( wide );

    glBegin(GL_LINE_LOOP);
    glColor3f( col.r, col.g, col.b );
    glVertex2f(v1.x, v0.y);
    glVertex2f(v1.x, v1.y);
    glVertex2f(v0.x, v1.y);
    glVertex2f(v0.x, v0.y);
    glEnd();

	DEPTHTEST_WRITE_GREATER(); 
}
//------------------------------------------------------------------------------
void SysGra::Box( vect3 v0, vect3 v1,rgb col, float wide )
//------------------------------------------------------------------------------
{
  	glLineWidth( wide );

    glBegin(GL_LINE_LOOP);
    glColor3f( col.r, col.g, col.b );
    glVertex3f(v1.x, v0.y, 0);
    glVertex3f(v1.x, v1.y, 0);
    glVertex3f(v0.x, v1.y, 0);
    glVertex3f(v0.x, v0.y, 0);
    glEnd();

}
//------------------------------------------------------------------------------
void SysGra::Fill( vect2 v0, vect2 v1,rgb col )
//------------------------------------------------------------------------------
{
    glBegin(GL_QUADS);
    glColor3f( col.r, col.g, col.b );
    glVertex2f(v1.x, v1.y);
    glVertex2f(v1.x, v0.y);
    glVertex2f(v0.x, v0.y);
    glVertex2f(v0.x, v1.y);
    glEnd();

}
//------------------------------------------------------------------------------
void SysGra::Line( vect2 v0, vect2 v1,rgb col, float wide )
//------------------------------------------------------------------------------
{
	DEPTHTEST_WRITE_ALWAYS();

  	glLineWidth( wide );
  
    glBegin(GL_LINES);
    glColor3f( col.r, col.g, col.b );
    glVertex2f(v0.x, v0.y);
    glVertex2f(v1.x, v1.y);
    glEnd();

	DEPTHTEST_WRITE_GREATER(); 
}
//------------------------------------------------------------------------------
void SysGra::Line( vect3 v0, vect3 v1,rgb col, float wide )
//------------------------------------------------------------------------------
{
  	glLineWidth( wide );
  
    glBegin(GL_LINES);
    glColor3f( col.r, col.g, col.b );
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glEnd();
}
//------------------------------------------------------------------------------
void SysGra::Tri( vect2 v0, vect2 v1, vect2 v2, rgb col )
//------------------------------------------------------------------------------
{
	DEPTHTEST_WRITE_ALWAYS();

    glBegin( GL_TRIANGLES );
    glColor3f( col.r, col.g, col.b );
    glVertex2f(v0.x, v0.y);
    glVertex2f(v1.x, v1.y);
    glVertex2f(v2.x, v2.y);
    glEnd();

	DEPTHTEST_WRITE_GREATER(); 
}
//------------------------------------------------------------------------------
void SysGra::Tri( vect3 v0, vect3 v1, vect3 v2, rgb col )
 //------------------------------------------------------------------------------
{
    glBegin( GL_TRIANGLES );
    glColor3f( col.r, col.g, col.b );
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glEnd();
}
//------------------------------------------------------------------------------
void SysGra::Print( vect2 v0, string str, rgb col  )
//------------------------------------------------------------------------------
{
	DEPTHTEST_WRITE_ALWAYS();

	glBegin(GL_POINTS);
	glColor3f( col.r, col.g, col.b );
	glEnd();

	glRasterPos2f(v0.x, v0.y);
	pImpl->wgl_font.DrawString( str );

	DEPTHTEST_WRITE_GREATER(); 
}

//------------------------------------------------------------------------------
void SysGra::Print( float x, float y, string str, rgb col  )
//------------------------------------------------------------------------------
{
	vect2 v = vect2(x*16,(y+1)*16)/vect2(GetWidth()/2,-GetHeight()/2)+vect2(-1,1);

	Print( v, str, col );
}

//------------------------------------------------------------------------------
void SysGra::SetZTest( bool flg )
//------------------------------------------------------------------------------
{
	if ( flg )
	{
		DEPTHTEST_ENABLE();
		//DEPTHTEST_WRITE_GREATER(); 
	}
	else
	{
		DEPTHTEST_DISABLE();
		//DEPTHTEST_WRITE_NEVER(); 
	}
	
}
//------------------------------------------------------------------------------
void SysGra::SetCulling( bool flg )
//------------------------------------------------------------------------------
{
	if ( flg )
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
	
}
