#include <iostream>
#include <functional>
using namespace std;

#include	"syskeys.h"
#include	"sysmouse.h"

#include <windows.h>

#include	"sysgra.h"
#include	"syswin.h"

#include	"plat.h"

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
			win.gdi.OnShowwindow( hWnd );
		};
		win.SetOnShowwindow( func );
	}
	{
		auto func = [&]( HWND hWnd )
		{
			win.gdi.OnSize( hWnd );
		};
		win.SetOnSize( func );
	}
	{
		auto func = [&]( HWND hWnd )
		{
			win.gdi.OnPaint( hWnd );
		};
		win.SetOnPaint( func );
	}
	{
		auto func = [&]( HWND hWnd )
		{
			win.gdi.OnDestroy( hWnd );
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

