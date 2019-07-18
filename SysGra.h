#include "geom.h"
struct SysGra
{
	struct	PrimBezier
	{
		vect2	v0;
		vect2	v1;
		vect2	v2;
		vect2	v3;
//		double	x0;
//		double	y0;
//		double	x1;
//		double	y1;
//		double	x2;
//		double	y2;
///		double	x3;
//		double	y3;
		int		col;
	};
	struct	PrimTri
	{
		vect2	v0;
		vect2	v1;
		vect2	v2;
//		double	x0;
//		double	y0;
//		double	x1;
//		double	y1;
//		double	x2;
//		double	y2;
		int		col;
	};
	struct	PrimLine
	{
		vect2	v0;
		vect2	v1;
//		double	x0;
//		double	y0;
//		double	x1;
//		double	y1;
		int		col;
	};
	struct	PrimPset
	{
		vect2	v;
//		double	x;
//		double	y;
		int		col;
	};
	struct	PrimCircle
	{
		vect2	v0;
		vect2	v1;
//		double	x0;
//		double	y0;
//		double	x1;
//		double	y1;
		int		col;
	};
	struct	PrimClr
	{
		bool	bActive;
		int		col;
	};
	struct
	{
//		BITMAPINFO	bmpInfo;
//		int			bpp;
//		BYTE*		bPixelBits = nullptr;
//		int			width;
//		int			height;

		vector<PrimBezier>	tblBezier;
		vector<PrimTri>		tblTri;
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

//	void	Bezier( double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, int col);
//	void	Tri( double x0, double y0, double x1, double y1, double x2, double y2, int col);
//	void	Line( double x0, double y0, double x1, double y1, int col);
//	void	Pset( double x0, double y0, int col);
//	void	Circle( double x, double y, double r, int col );
//	void	Clr( int col );
//	int 	Rgb(double r, double g , double b );

	void	Bezier( vect2 v0, vect2 v1, vect2 v2, vect2 v3, int col);
	void	Tri( vect2 v0, vect2 v1, vect2 v2, int col);
	void	Line( vect2 v0, vect2 v1, int col);
	void	Pset( vect2 v0, int col);
	void	Circle( vect2 v, double r, int col );
	void	Clr( int col );
	int 	Rgb(double r, double g , double b );

	SysGra();
	~SysGra();

};
