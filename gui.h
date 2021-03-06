


struct Gui
{
	struct Cp cp;	// Controll Point

	bool bShowPoint = true;

	struct One
	{
		bool	bEnable;
		float	w;		// wバッファ値
		int		idxTbl;	// オブジェクト番号
		int		idxObj;	// オブジェクト番号
		//--
		bool	bSelected_a = false;
		bool	bSelected_b = false;

		One()
		{
			clear();
		}
		
		void clear()
		{
			bEnable = false;
			w = 0.0;
			idxTbl = 0;
			idxObj = 0;
			bSelected_a = false;
			bSelected_b = false;
		}
		
	};

	enum class G_CALC
	{
		NONE,
		ADD,
		SUB,
		COPY,
		REV,
	};

	vect2 rect_st = vect2(0,0);			//	矩形選択開始位置
	G_CALC rect_mode = G_CALC::NONE;	//	矩形選択中フラグ
	One one;


	//------------------------------------------------------------------------------
	int EntryTbl( std::vector<std::shared_ptr<Obj>>tbl, std::vector<std::shared_ptr<Edge>>tbl_pEdge )
	//------------------------------------------------------------------------------
	{
		int idx =  (signed)cp.tbltbl_pObj.size();
		cp.tbltbl_pObj.emplace_back( tbl );
		cp.tbltbl_pEdge.emplace_back( tbl_pEdge );
		return idx;
	}

	//------------------------------------------------------------------------------
	void TouchFirst( SysGra& gra, Pers& pers, std::vector<std::vector<std::shared_ptr<Obj>>>& tbltbl_pObj, vect2 mpos );
	//------------------------------------------------------------------------------

	// 矩形カーソル開始 新規選択
	//------------------------------------------------------------------------------
	void SelectRectOnly( vect2 mpos );
	//------------------------------------------------------------------------------

	// 矩形カーソル開始 追加選択
	//------------------------------------------------------------------------------
	void SelectRectAdd( vect2 mpos );
	//------------------------------------------------------------------------------

	// 矩形カーソル開始 反転選択
	//------------------------------------------------------------------------------
	void SelectRectRev( vect2 mpos );
	//------------------------------------------------------------------------------

	// 矩形カーソル開始 削除選択
	//------------------------------------------------------------------------------
	void SelectRectSub( vect2 mpos );
	//------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------
	void calcRectMode( G_CALC rect_mode, bool& bPreselect, bool& bSelected );
	//------------------------------------------------------------------------------

	// 矩形カーソル終了（選択決定）
	//------------------------------------------------------------------------------
	void SelectRectEnd( std::vector<std::vector<std::shared_ptr<Obj>>>& tbltbl_pObj );
	//------------------------------------------------------------------------------

	// 矩形カーソル選択	
	//------------------------------------------------------------------------------
	void SelectRectBegin( Pers& pers, std::vector<std::vector<std::shared_ptr<Obj>>>& tbltbl_pObj , vect2 mpos );
	//------------------------------------------------------------------------------

	// 単独 新規選択
	//------------------------------------------------------------------------------
	void SelectOneOnly( std::vector<std::vector<std::shared_ptr<Obj>>>& tbltbl_pObj );
	//------------------------------------------------------------------------------

	// 単独 追加選択
	//------------------------------------------------------------------------------
	void SelectOneAdd( std::vector<std::vector<std::shared_ptr<Obj>>>& tbltbl_pObj );
	//------------------------------------------------------------------------------

	// 単独 反転選択
	//------------------------------------------------------------------------------
	void SelectOneRev( std::vector<std::vector<std::shared_ptr<Obj>>>& tbltbl_pObj );
	//------------------------------------------------------------------------------

	// 単独 削除選択
	//------------------------------------------------------------------------------
	void SelectOneSub( std::vector<std::vector<std::shared_ptr<Obj>>>& tbltbl_pObj );

	//------------------------------------------------------------------------------
	// 選択リスト表示
	//------------------------------------------------------------------------------
	void DrawController( SysGra& gra, Pers& pers, std::vector<std::vector<std::shared_ptr<Obj>>>& tbltbl_pObj, std::vector<std::vector<std::shared_ptr<Edge>>>& tbltbl_pEdge, vect2 mpos );
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	void MoveObj( SysGra& gra, Pers& pers, std::vector<std::vector<std::shared_ptr<Obj>>>& tbltbl_pObj, vect2& mpos, vect2& mprev, vect2& mmov, bool bSame, bool bByCamera, bool bByFloor, bool bByXY );
	//------------------------------------------------------------------------------



};
