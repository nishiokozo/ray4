#include <vector>
#include <string>
#include "geom.h"

struct SysGra
{
	uint64_t	time_peak = 0;

	bool bInitialized = false;

	float GetWidth();
	float GetHeight();
	float GetAspect();

	void OnSize( int width, int height );
	void OnMove( int pos_x, int pos_y );
	void OnDestroy();
	void OnCreate();
	void OnPaint();

	void	Wait();
	void	Flush();

	void	Print( vect2 v, std::string str, rgb col=rgb(1,1,1) );
	void	Print( float x, float y, std::string str, rgb col=rgb(1,1,1) );

	void	Tri2d( vect2 v0, vect2 v1, vect2 v2, rgb col=rgb(1,1,1));
	void	Tri3d( vect3 v0, vect3 v1, vect3 v2, rgb col=rgb(1,1,1));
	void	Box2d( vect2 v0, vect2 v1, rgb col=rgb(1,1,1), float wide=1.0f);
	void	Box3d( vect3 v0, vect3 v1, rgb col=rgb(1,1,1), float wide=1.0f);
	void	Fill2d( vect2 v0, vect2 v1, rgb col=rgb(1,1,1));
	void	Line2d( vect2 v0, vect2 v1, rgb col=rgb(1,1,1), float wide=1.0f);
	void	Line3d( vect3 v0, vect3 v1, rgb col=rgb(1,1,1), float wide=1.0f);
	void	Pset2d( vect2 v0, rgb col=rgb(1,1,1), float wide=1.0f);
	void	Pset3d( vect3 v0, rgb col=rgb(1,1,1), float wide=1.0f);
	void	Circle2d( vect2 v, float r, float step=12, rgb col=rgb(1,1,1), float wide=1.0f );
	void	Circle3d( vect3 v, float r, float step=24, rgb col=rgb(1,1,1), float wide=1.0f );
	void	Clr( rgb col=rgb(1,1,1) );

	void	SetZTest( bool flg );
	void	SetCulling( bool flg );

	SysGra();
	~SysGra();


	vect2 Conv( vect2 pos )
	{
		// ピクセル座標(pos)をGL座標に変換する
		return pos/vect2(GetWidth()/2,-GetHeight()/2)+vect2(-1,1);
	}

	vect2 Conv( float x, float y )
	{
		// ピクセル座標(pos)をGL座標に変換する
		return vect2(x,y)/vect2(GetWidth()/2,-GetHeight()/2)+vect2(-1,1);
	}

	vect2 Dot( float x, float y )
	{
		// ピクセル座標数(pos)をGL座標数に変換する
		return vect2(x,y)/vect2(GetWidth(),-GetHeight());
	}
	
	vect2 Pixel( vect2 v )
	{
		// ピクセル座標数(mov)に変換したGL座標数に変換する
		return	v*vect2(GetWidth(),GetHeight());
	}

    struct Impl;
    std::unique_ptr<Impl> pImpl;

};
