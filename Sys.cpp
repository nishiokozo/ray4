#include <iostream>
#include <functional>

#include "SysKeys.h"
#include "SysMouse.h"

#include <windows.h>
#include "SysGra.h"
#include "Sys.h"
#include "SysWin.h"

using namespace std;


//------------------------------------------------------------------------------
void Sys::SetOnPaint( function<void()> func )
//------------------------------------------------------------------------------
{
	SysWin&	win = SysWin::GetInstance();

	// ウィンドウペイント関数
	win.SetOnPaint( func );

}

//------------------------------------------------------------------------------
void Sys::SetOnSize( function<void( int width, int height )> func )
//------------------------------------------------------------------------------
{
	SysWin&	win = SysWin::GetInstance();

	// ウィンドウサイズ変更関数
	win.SetOnSize( func );

}

//------------------------------------------------------------------------------
void Sys::SetOnDestroy( function<void()> func )
//------------------------------------------------------------------------------
{
	SysWin&	win = SysWin::GetInstance();

	// ウィンドウ破棄関数
	win.SetOnDestroy( func );
}

//------------------------------------------------------------------------------
void Sys::SetOnCreate( function<void()> func )
//------------------------------------------------------------------------------
{
	SysWin&	win = SysWin::GetInstance();
	win.SetOnCreate( func );
}

//------------------------------------------------------------------------------
void Sys::OpenWindow( const char* name, int pos_x, int pos_y, int width, int height  )
//------------------------------------------------------------------------------
{
	SysWin&	win = SysWin::GetInstance();
	win.OpenWindow("Ray4 " __DATE__, pos_x,pos_y,width, height );
}

//------------------------------------------------------------------------------
void  Sys::SetWincursor( bool flg ) 
//------------------------------------------------------------------------------
{	// カーソル非表示
	ShowCursor( flg );
}

//------------------------------------------------------------------------------
bool	Sys::Update()
//------------------------------------------------------------------------------
{
	SysWin&	win= SysWin::GetInstance();
	bool b = win.Update();
	return b;
}
