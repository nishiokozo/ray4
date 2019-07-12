#include <iostream>
#include <functional>
using namespace std;

#include	"syskeys.h"
#include	"sysmouse.h"
#include	"SysGdi.h"

#include <windows.h>


#include	"plat.h"

#include	"syswin.h"

static 	SysWin&	win= SysWin::GetInstance();

//------------------------------------------------------------------------------
Plat::Plat( const char* name, int pos_x, int pos_y, int width, int height  )
//------------------------------------------------------------------------------
{
	m.x			= pos_x;
	m.y			= pos_y;
	m.width		= width;
	m.height	= height;

	// コールバック登録
	{

		auto func = [&]()
		{
			gdi.OnShowwindow();
		};
		win.SetOnShowwindow( func );
	}
	{
		auto func = [&]()
		{
			gdi.OnSize();
		};
		win.SetOnSize( func );
	}
	{
		auto func = [&]()
		{
			gdi.OnPaint();
		};
		win.SetOnPaint( func );
	}
	{
		auto func = [&]()
		{
			gdi.OnDestroy();
		};
		win.SetOnDestroy( func );
	}

	win.OpenWindow("Ray4 " __DATE__, 300,300,512, 512 );

}
//------------------------------------------------------------------------------
bool	Plat::Update()
//------------------------------------------------------------------------------
{
	bool b = win.Update();
		keys.Update();
		mouse.Update();

	return b;
}
