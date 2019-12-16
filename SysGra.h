#include <vector>
#include <string>
#include "geom.h"

using namespace std;

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

	void	Update();

	void	Print( vect2 v, string str, rgb col=rgb(1,1,1) );
	void	Print( float x, float y, string str, rgb col=rgb(1,1,1) );

	void	Tri( vect2 v0, vect2 v1, vect2 v2, rgb col);
	void	Tri( vect3 v0, vect3 v1, vect3 v2, rgb col);
	void	Box( vect2 v0, vect2 v1, rgb col, float wide=1.0f);
	void	Box( vect3 v0, vect3 v1, rgb col, float wide=1.0f);
	void	Fill( vect2 v0, vect2 v1, rgb col);
	void	Line( vect2 v0, vect2 v1, rgb col, float wide=1.0f);
	void	Line( vect3 v0, vect3 v1, rgb col, float wide=1.0f);
	void	Pset( vect2 v0, rgb col, float wide=1.0f);
	void	Pset( vect3 v0, rgb col, float wide=1.0f);
	void	Circle( vect2 v, float r, rgb col, float wide=1.0f );
	void	Circle( vect3 v, float r, rgb col, float wide=1.0f );
	void	Clr( rgb col );

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
