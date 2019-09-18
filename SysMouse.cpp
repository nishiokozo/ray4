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

	POINT point;

	GetCursorPos( &point );

	SysWin& win = SysWin::GetInstance();

#if 0
	this->pos.x = point.x - win.GetPosX();
	this->pos.y = point.y - win.GetPosY();

	this->mov.x = 0;
	this->mov.y = 0;

	this->prev = this->pos;
#endif

	this->wheel = 0;

	// GL座標系
	{
		SysWin& win = SysWin::GetInstance();

		vect2	gp( point.x, point.y );
		gp = gp/vect2(win.GetWidth()/2,-win.GetHeight()/2)+vect2(-1,1);
	
		this->prev = this->pos;
		this->mov = 0;
		this->pos = gp;
	}

}


//-----------------------------------------------------------------------------
void SysMouse::Update()
//-----------------------------------------------------------------------------
{
	POINT point;

	SysWin& win = SysWin::GetInstance();
	GetCursorPos( &point );
	{
		point.x -= win.GetPosX();
		point.y -= win.GetPosY();
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

//	this->mx = point.x - this->sx;
//	this->my = point.y - this->sy;
//
//	this->sx = point.x;
//	this->sy = point.y;

#if 0
	this->prev = this->pos;
	this->mov.x = point.x - this->pos.x;
	this->mov.y = point.y - this->pos.y;
	this->pos.x = point.x;
	this->pos.y = point.y;
#endif
	
	// GL座標系
	{
		SysWin& win = SysWin::GetInstance();

		vect2	gp( point.x, point.y );
		gp = gp/vect2(win.GetWidth()/2,-win.GetHeight()/2)+vect2(-1,1);
	
		this->prev = this->pos;
		this->mov = gp - this->pos;
		this->pos = gp;

		if ( win.GetHeight() > 0 )
		{
//			this->gmov.x *= 1.0/(win.GetWidth()/win.GetHeight());
		}
	}

	this->wheel = win.GetWheel();

//cout << wheel << endl;

}

