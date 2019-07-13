#include <iostream>
using namespace std;

#include <windows.h>
#include "SysMouse.h"

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

	this->sx = (float)pos.x;
	this->sy = (float)pos.y;

	this->mx = 0;
	this->my = 0;
}


//-----------------------------------------------------------------------------
void SysMouse::Update()
//-----------------------------------------------------------------------------
{
	POINT pos;

	GetCursorPos( &pos );

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

	this->mx = (float)pos.x - this->sx;
	this->my = (float)pos.y - this->sy;

	this->sx = (float)pos.x;
	this->sy = (float)pos.y;
}

