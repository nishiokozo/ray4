#include <iostream>
#include <functional>
using namespace std;

#include	"plat.h"

#include <windows.h>

#include	"syswin.h"
#include	"sysgdi.h"

	SysWin	win;
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

		auto func = [&]( HWND hWnd )
		{
			gdi.OnShowwindow( hWnd );
		};
		win.SetOnShowwindow( func );
	}
	{
		auto func = [&]( HWND hWnd )
		{
			gdi.OnSize( hWnd );
		};
		win.SetOnSize( func );
	}
	{
		auto func = [&]( HWND hWnd )
		{
			gdi.OnPaint( hWnd );
		};
		win.SetOnPaint( func );
	}
	{
		auto func = [&]( HWND hWnd )
		{
			gdi.OnDestroy( hWnd );
		};
		win.SetOnDestroy( func );
	}

	win.OpenWindow("Ray4 " __DATE__, 300,300,512, 512 );

}
//------------------------------------------------------------------------------
bool	Plat::Update()
//------------------------------------------------------------------------------
{
	return win.Update();
}

