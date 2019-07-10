#define APIENTRY
#include <stdio.h>
#include <math.h>

#include <windows.h>
#include <stdio.h>
#include <string.h>
//#include "l.h"
#include "sysmouse.h"
//#include "font.h"



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
// 	int	n = GetSystemMetrics( SM_CMOUSEBUTTONS );
// 	int	w = GetSystemMetrics( SM_XVIRTUALSCREEN );

	int	on_l = (l&0x8000)!=0;
	int	on_r = (r&0x8000)!=0;

	this->hi.l = !this->on.l &&  on_l;
	this->hi.r = !this->on.r &&  on_r;

	this->lo.l =  this->on.l && !on_l;
	this->lo.r =  this->on.r && !on_r;

	this->mx = (float)pos.x - this->sx;
	this->my = (float)pos.y - this->sy;
//	this->vm.x =   (float)pos.x - this->sx;
//	this->vm.y = -((float)pos.y - this->sy);
	this->on.l = on_l;
	this->on.r = on_r;

	this->sx = (float)pos.x;
	this->sy = (float)pos.y;

//	this->dx = (this->sx - this->cx) * this->screen_w;
//	this->dy = (this->sy - this->cy) * this->screen_h;
/*
	this->vw.x =  (((float)pos.x - framework_getX())/ framework_getW())*2.0f-1.0f;
	this->vw.y =  (((float)pos.y - framework_getY())/ framework_getH())*2.0f-1.0f;


	this->vf.x = 0; 
	this->vf.y = 0; 
	if ( this->vw.x >  0.1f ) this->vf.x = (this->vw.x-0.1f)/0.9f;
	if ( this->vw.x < -0.1f ) this->vf.x = (this->vw.x+0.1f)/0.9f;
	if ( this->vw.y >  0.1f ) this->vf.y = (this->vw.y-0.1f)/0.9f;
	if ( this->vw.y < -0.1f ) this->vf.y = (this->vw.y+0.1f)/0.9f;

	if ( this->vf.x >  1.0f ) this->vf.x =  1.0f;
	if ( this->vf.x < -1.0f ) this->vf.x = -1.0f;
	if ( this->vf.y >  1.0f ) this->vf.y =  1.0f;
	if ( this->vf.y < -1.0f ) this->vf.y = -1.0f;
	

	if ( this->vw.x < -1 ) this->vw.x = -1;
	if ( this->vw.y < -1 ) this->vw.y = -1;
	if ( this->vw.x >  1 ) this->vw.x =  1;
	if ( this->vw.y >  1 ) this->vw.y =  1;

	this->vd.x =  (this->sx - this->cx) * this->screen_w;
	this->vd.y = -(this->sy - this->cy) * this->screen_h;
*/ 
//	font_printf( "mouse %f %f %d %d %d %d \n", this->mx, this->my, this->on.l, this->on.r, n, w );

//	return	false;
}

/*
//-----------------------------------------------------------------------------
int	 mosue_resize(int w, int h)
//-----------------------------------------------------------------------------
{
	this->screen_w = 1.0 / (float)w;
	this->screen_h = 1.0 / (float)h;


	return	true;
}
//-----------------------------------------------------------------------------
int mouse_mouse(int button, int state, int x, int y)
//-----------------------------------------------------------------------------
{
return true;
	this->button = button;
	this->state = state;
	if ( button != 0xffff )
	{
#if 0
		switch (button) {
		case GLUT_LEFT_BUTTON:
			switch (state) {
			case GLUT_DOWN:

				// ドラッグ開始点を記録
				this->cx = this->sx;
				this->cy = this->sy;

//						this->on.l = true;
//						this->flgDrag = true;

				break;
			case GLUT_UP:

//						this->on.l = false;
//						this->flgDrag = false;

				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
#endif
	}
	else
	{
		
		this->sx = x;
		this->sy = y;

	}
		
	return	true;
}

*/
