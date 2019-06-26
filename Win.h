class	Win
{
private:// for GDI
//	void GDI_draw();
//	void GDI_create(int bpp );

public:

	unsigned char* getAddrPixels();

	int m_width;
	int	m_height;

	Win(int w, int h, const char* name );
	virtual ~Win();

	bool exec();

};
