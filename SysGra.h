#include <vector>
#include "geom.h"
struct SysGra
{

	enum EnumType
	{
		TypeClr,
		TypeCircle,
		TypePset,
		TypeLine,
		TypeFill,
		TypeBox,
		TypeTri,
	};


	struct
	{
//		BITMAPINFO	bmpInfo;
//		int			bpp;
//		BYTE*		bPixelBits = nullptr;
//		int			width;
//		int			height;



		vector<vect2>		tblVect2;



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


	void	Tri( vect2 v0, vect2 v1, vect2 v2, int col);
	void	Box( vect2 v0, vect2 v1, int col);
	void	Fill( vect2 v0, vect2 v1, int col);
	void	Line( vect2 v0, vect2 v1, int col);
	void	Pset( vect2 v0, int col);
	void	Circle( vect2 v, double r, int col );
	void	Clr( int col );
	int 	Rgb(double r, double g , double b );

	SysGra();
	~SysGra();

};
