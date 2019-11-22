struct Lab15 : LabObj
{
	bool	bActive = false;	//	使用可能

	int idxTbl = 0;

	~Lab15()
	{
//		for ( Obj* p : tblPoint ) delete p;
	}

/*
	vector<Obj*> tblPoint =
	{
		new Point3(vect3(-1.0, 0.0, 0.0 ),vect3( 0.0, 0.0, 1.0 ),vect3( 0.0, 0.0,-1.0 )),
		new Point3(vect3( 1.0, 0.0, 0.0 ),vect3( 0.0, 0.0,-1.0 ),vect3( 0.0, 0.0, 1.0 )),

	};
*/

	vector<int>	idxPoint =
	{
		0,1,0
	};

	vector<Edge*> tblDummy;		//	空のまま

};
