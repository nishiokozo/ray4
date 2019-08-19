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


//	 BITMAP bmBitmap;

	int GetWidth();
	int GetHeight();

	void ReleasePixelBits();
	void CreatePixelBits(int bpp, int width, int height );
	void OnSize( int width, int height );
	void OnMove( int pos_x, int pos_y );
	void OnDestroy();
	void OnCreate();
	void OnPaint();

	void Update() ;


	void	Cls();

	void	Print2d( vect2 v, string str );
	void	Tri2d( vect2 v0, vect2 v1, vect2 v2, vect3 col);
	void	Box2d( vect2 v0, vect2 v1, vect3 col);
	void	Fill2d( vect2 v0, vect2 v1, vect3 col);
	void	Line2d( vect2 v0, vect2 v1, vect3 col);
	void	Pset2d( vect2 v0, vect3 col, float w=1.0f);
	void	Circle2d( vect2 v, float r, vect3 col );

	void	Print( vect2 v, string str );
	void	Tri( vect2 v0, vect2 v1, vect2 v2, vect3 col);
	void	Box( vect2 v0, vect2 v1, vect3 col);
	void	Fill( vect2 v0, vect2 v1, vect3 col);
	void	Line( vect2 v0, vect2 v1, vect3 col);
	void	Pset( vect2 v0, vect3 col, float w=1.0f);
	void	Circle( vect2 v, float r, vect3 col );

	void	Clr( vect3 col );

	SysGra();
	~SysGra();

};
