struct SysGra
{
	struct	PrimBezier
	{
		double	x0;
		double	y0;
		double	x1;
		double	y1;
		double	x2;
		double	y2;
		double	x3;
		double	y3;
		int		col;
	};
	struct	PrimTri
	{
		double	x0;
		double	y0;
		double	x1;
		double	y1;
		double	x2;
		double	y2;
		int		col;
	};
	struct	PrimLine
	{
		double	x0;
		double	y0;
		double	x1;
		double	y1;
		int		col;
	};
	struct	PrimPset
	{
		double	x;
		double	y;
		int		col;
	};
	struct	PrimCircle
	{
		double	x0;
		double	y0;
		double	x1;
		double	y1;
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

	void	Bezier( double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, int col);
	void	Tri( double x0, double y0, double x1, double y1, double x2, double y2, int col);
	void	Line( double x0, double y0, double x1, double y1, int col);
	void	Pset( double x0, double y0, int col);
	void	Circle( double x, double y, double r, int col );
	void	Clr( int col );
	int 	Rgb(double r, double g , double b );

	SysGra();
	~SysGra();

};
