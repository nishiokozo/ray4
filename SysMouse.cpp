#include <iostream>
using namespace std;

#include "SysMouse.h"

#include <windows.h>
#include <functional>
#include "SysWin.h"

//-----------------------------------------------------------------------------
SysMouse& SysMouse::GetInstance()
//-----------------------------------------------------------------------------
{
	static SysMouse	mouse;
	return mouse;
}

//-----------------------------------------------------------------------------
SysMouse::~SysMouse()
//-----------------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------------
SysMouse::SysMouse()
//-----------------------------------------------------------------------------
{

	POINT pos;

	GetCursorPos( &pos );

	SysWin& win = SysWin::GetInstance();

	this->sx = pos.x - win.GetPosX();
	this->sy = pos.y - win.GetPosY();

	this->mx = 0;
	this->my = 0;

	this->wheel = 0;

}

/*
//-----------------------------------------------------------------------------
void SysMouse::OnSize( int width, int height )
//-----------------------------------------------------------------------------
{
	m.width = width;
	m.height = height;
}

//-----------------------------------------------------------------------------
void SysMouse::OnMove( int pos_x, int pos_y )
//-----------------------------------------------------------------------------
{
	m.pos_x = pos_x;
	m.pos_y = pos_y;
}
*/

//-----------------------------------------------------------------------------
void SysMouse::Update()
//-----------------------------------------------------------------------------
{
	POINT pos;

	SysWin& win = SysWin::GetInstance();
	GetCursorPos( &pos );
	{
		pos.x -= win.GetPosX();
		pos.y -= win.GetPosY();
	}

	int	l = GetAsyncKeyState(VK_LBUTTON);
	int r = GetAsyncKeyState(VK_RBUTTON);
	int m = GetAsyncKeyState(VK_MBUTTON);
 	int	b = GetAsyncKeyState( VK_XBUTTON1 );
 	int	f = GetAsyncKeyState( VK_XBUTTON2 );

	int	on_l = (l&0x8000)!=0;
	int	on_r = (r&0x8000)!=0;
	int	on_m = (m&0x8000)!=0;
	int	on_f = (f&0x8000)!=0;
	int	on_b = (b&0x8000)!=0;

	this->L.hi = !this->L.on &&  on_l;
	this->L.lo =  this->L.on && !on_l;
	this->L.on = on_l;

	this->R.hi = !this->R.on &&  on_r;
	this->R.lo =  this->R.on && !on_r;
	this->R.on = on_r;

	this->M.hi = !this->M.on &&  on_m;
	this->M.lo =  this->M.on && !on_m;
	this->M.on = on_m;

	this->F.hi = !this->F.on &&  on_f;
	this->F.lo =  this->F.on && !on_f;
	this->F.on = on_f;

	this->B.hi = !this->B.on &&  on_b;
	this->B.lo =  this->B.on && !on_b;
	this->B.on = on_b;

	this->mx = pos.x - this->sx;
	this->my = pos.y - this->sy;

	this->sx = pos.x;
	this->sy = pos.y;

	this->wheel = win.GetWheel();

//cout << wheel << endl;

}

