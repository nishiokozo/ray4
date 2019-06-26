class	Win
{
public:
	struct
	{
		int width;
		int	height;
	} m;
	
	unsigned char* getAddrPixels();

	Win(int w, int h, const char* name );
	virtual ~Win();

	bool exec();

};
