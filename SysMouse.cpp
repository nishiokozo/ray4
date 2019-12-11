#include <iostream>
#include <memory>

#include "SysMouse.h"

#include <windows.h>
#include <functional>
#include "SysWin.h"

using namespace std;

struct SysMouse::Impl
{
};

//-----------------------------------------------------------------------------
SysMouse::~SysMouse()
//-----------------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------------
SysMouse::SysMouse() : pImpl( new SysMouse::Impl )
//-----------------------------------------------------------------------------
{

	POINT point;

	GetCursorPos( &point );

	SysWin& win = SysWin::GetInstance();

	this->wheel = 0;

	// GL座標系
	{
		SysWin& win = SysWin::GetInstance();

		vect2	gp( (float)point.x, (float)point.y );
		gp = gp/vect2((float)win.GetWidth()/2,-(float)win.GetHeight()/2)+vect2(-1,1);
	
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

	int	l = GetAsyncKeyState( VK_LBUTTON );
	int r = GetAsyncKeyState( VK_RBUTTON );
	int m = GetAsyncKeyState( VK_MBUTTON );
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

	// GL座標系
	{
		SysWin& win = SysWin::GetInstance();

		vect2	gp( (float)point.x, (float)point.y );
		gp = gp/vect2((float)win.GetWidth()/2,-(float)win.GetHeight()/2)+vect2(-1,1);
	
		this->prev = this->pos;
		this->mov = gp - this->pos;
		this->pos = gp;

	}

	this->wheel = (float)win.GetWheel();

//cout << wheel << endl;

}

