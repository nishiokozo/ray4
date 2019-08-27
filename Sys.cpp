#include <iostream>
#include <functional>
using namespace std;

#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"
#include "Sys.h"

#include <windows.h>
#include "SysWin.h"


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
