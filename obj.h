struct	Obj
{
	int id = 0;

	bool 	bRectSelected	= false;		//	矩形選択中、選択＆非選択
	bool 	bRectIn			= false;		//	矩形選択中、矩形選択対象
	bool 	bSelected		= false;		//	選択
	bool 	bAffectable		= false;		//	削除対象

	virtual void Move2( vect2 v ) =0;
	virtual vect2 Pos2() =0;
};

