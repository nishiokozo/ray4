#include <vector>
#include "geom.h"
struct SysGra
{
/*
	struct	PrimBezier
	{
		vect2	v0;
		vect2	v1;
		vect2	v2;
		vect2	v3;
		int		col;
	};
*/
	struct	PrimTri
	{
		vect2	v0;
		vect2	v1;
		vect2	v2;
		vect2	col;
	};
	struct	PrimBox
	{
		vect2	v0;
		vect2	v1;
		vect2	col;
	};
	struct	PrimFill
	{
		vect2	v0;
		vect2	v1;
		vect2	col;
	};
	struct	PrimLine
	{
		vect2	v0;
		vect2	v1;
		vect2	col;
	};
	struct	PrimPset
	{
		vect2	v;
		vect2	col;
	};
	struct	PrimCircle
	{
		vect2	v0;
		vect2	v1;
		vect2	col;
	};
	struct	PrimClr
	{
		bool	bActive;
		vect2	col;
	};
	struct
	{
//		BITMAPINFO	bmpInfo;
//		int			bpp;
//		BYTE*		bPixelBits = nullptr;
//		int			width;
//		int			height;

//		vector<PrimBezier>	tblBezier;
		vector<PrimTri>		tblTri;
		vector<PrimBox>		tblBox;
		vector<PrimFill>	tblFill;
		vector<PrimLine>	tblLine;
		vector<PrimPset>	tblPset;
		vector<PrimCircle>	tblCircle;
		PrimClr				clr;



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

//	void	Clr( int col );

//	void	Bezier( vect2 v0, vect2 v1, vect2 v2, vect2 v3, int col);
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
