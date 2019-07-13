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
	m.x			= pos_x;
	m.y			= pos_y;
	m.width		= width;
	m.height	= height;

	SysWin&	win= SysWin::GetInstance();


	//	ウィンドウ生成関数
	{
		auto func = [&]()
		{
			gra.OnShowwindow();
		};
		win.SetOnShowwindow( func );
	}

	// ウィンドウサイズ変更関数
	{
		auto func = [&]()
		{
			gra.OnSize();
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

	win.OpenWindow("Ray4 " __DATE__, 300,300,512, 512 );

}
//------------------------------------------------------------------------------
bool	Sys::Update()
//------------------------------------------------------------------------------
{
	SysWin&	win= SysWin::GetInstance();
	bool b = win.Update();
	keys.Update();
	mouse.Update();

	return b;
}
