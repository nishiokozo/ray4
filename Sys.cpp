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
Sys::Sys( const char* name, int pos_x, int pos_y, int width, int height  )
//------------------------------------------------------------------------------
{


	SysWin&	win = SysWin::GetInstance();



	//	ウィンドウ生成関数
	{
		auto func = [&]()
		{
			gra.OnCreate();
		};
		win.SetOnCreate( func );
	}

	// ウィンドウサイズ変更関数
	{
		auto func = [&]( int width, int height )
		{
			gra.OnSize( width, height );
		};
		win.SetOnSize( func );
	}

	// ウィンドウペイント関数
	{
		auto func = [&]()
		{
			gra.OnPaint();
		};
		win.SetOnPaint( func );
	}

	// ウィンドウ破棄関数
	{
		auto func = [&]()
		{
			gra.OnDestroy();
		};
		win.SetOnDestroy( func );
	}

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
	keys.Update();
	mouse.Update();
	gra.Update();
	
	return b;
}
