#include <vector>
#include <string>
#include "geom.h"
struct SysGra
{

//	enum EnumType
//	{
//		TypeClr,
//		TypeCircle,
//		TypePset,
//		TypeLine,
//		TypeFill,
//		TypeBox,
//		TypeTri,
//	};
//
//	struct Message
//	{
//		string	str;
//		vect2	pos;
//		Message( string _str, vect2 _pos )
//		{
//			str = _str;
//			pos = _pos;
//		}
//	} ;

	struct 
	{
//		BITMAPINFO	bmpInfo;
//		int			bpp;
//		BYTE*		bPixelBits = nullptr;
//		int			width;
//		int			height;



//		vector<vect2>		tblVect2;
//		vector<Message>		tblMessage;



	} m;

	bool bInitialized = false;

//	 BITMAP bmBitmap;

	int GetWidth();
	int GetHeight();
	float GetAspect();

	void OnSize( int width, int height );
	void OnMove( int pos_x, int pos_y );
	void OnDestroy();
	void OnCreate();
	void OnPaint();

	void Update() ;


	void	Print( vect2 v, string str );
	void	Print( float x, float y, string str );

	void	Tri( vect2 v0, vect2 v1, vect2 v2, rgb col);
	void	Tri( vect3 v0, vect3 v1, vect3 v2, rgb col);
	void	Box( vect2 v0, vect2 v1, rgb col, float wide=1.0f);
	void	Box( vect3 v0, vect3 v1, rgb col, float wide=1.0f);
	void	Fill( vect2 v0, vect2 v1, rgb col);
	void	Line( vect2 v0, vect2 v1, rgb col, float wide=1.0f);
	void	Line( vect3 v0, vect3 v1, rgb col, float wide=1.0f);
	void	Pset( vect2 v0, rgb col, float wide=1.0f);
	void	Pset( vect3 v0, rgb col, float wide=1.0f);

	void	Circle( vect2 v, float r, rgb col );
	void	Clr( rgb col );

	void	SetZTest( bool flg );

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
};
