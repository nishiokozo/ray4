#include <vector>
#include <string>
#include "geom.h"
struct SysGra
{

	enum EnumType
	{
		TypeClr,
//		TypeCircleOld,
		TypeCircle,
		TypePset,
		TypeLine,
		TypeFill,
		TypeBox,
		TypeTri,
	};

	struct Message
	{
		string	str;
		vect2	pos;
		Message( string _str, vect2 _pos )
		{
			str = _str;
			pos = _pos;
		}
	} ;
	struct 
	{
//		BITMAPINFO	bmpInfo;
//		int			bpp;
//		BYTE*		bPixelBits = nullptr;
//		int			width;
//		int			height;



		vector<vect2>		tblVect2;
		vector<Message>		tblMessage;



	} m;

	bool bGl = false;

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


	void	Print( vect2 v, string str );
	void	Cls();

	void	Tri( vect2 v0, vect2 v1, vect2 v2, int col);
	void	Box( vect2 v0, vect2 v1, int col);
	void	Fill( vect2 v0, vect2 v1, int col);
	void	Line( vect2 v0, vect2 v1, int col);
	void	Pset( vect2 v0, int col);
//	void	CircleOld( vect2 v, float r, int col );
	void	Circle( vect2 v, float r, int col );
	void	Clr( int col );
	int 	Rgb(float r, float g , float b );

	SysGra();
	~SysGra();

};
