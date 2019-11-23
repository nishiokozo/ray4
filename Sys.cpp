#include <iostream>
#include <memory>
#include <functional>

#include "SysKeys.h"
#include "SysMouse.h"

#include <windows.h>
#include "SysGra.h"
#include "Sys.h"
#include "SysWin.h"

using namespace std;

//------------------------------------------------------------------------------
void Sys::InitWinapi(
//------------------------------------------------------------------------------
	function<void()> funcOnCreate,
	function<void( int width, int height )> funcOnSize,
	function<void()> funcOnPaint,
	function<void()> funcDestroy
)
{
	SysWin&	win = SysWin::GetInstance();

	win.SetOnCreate( funcOnCreate );
	win.SetOnSize( funcOnSize );
	win.SetOnPaint( funcOnPaint );
	win.SetOnDestroy( funcDestroy );

	win.InitWinapi();
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
