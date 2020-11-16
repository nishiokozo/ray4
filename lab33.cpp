//2019/11/22

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <cmath>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <tuple>
#include <random>

#include "geom.h"

#include "SysSound.h"
#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"

#include "func.h"
#include "pers.h"

#include "lab.h"
#include "lab33.h"

using namespace std;

struct Lab33::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

//	vector<shared_ptr<Obj>>	tbl_pObj;



	struct Parsar_MQO
	{
		struct MQO
		{
			struct Scene
			{
				struct Dirlights
				{
					vect3	dir;
					rgb		color;
				};
				
				vect3	pos;
				vect3	lookat;
				float	head;
				float	pich;
				float	bank;
				float	ortho;
				float	zoom2;
				rgb		amb;
				float	frontclip;
				float	backclip;
				
				vector<Dirlights> tbl_dirlights;
				
			};

			struct Material
			{
				int		shader;
				rgba	col;
				float	dif;
				float	amb;
				float	emi;
				float	spc	;
				float	power;
			};
			
			struct	Object
			{
				struct	Face
				{
					vector<int>		V;
					int				M;
					vector<vect2>	UV;
				};
			
				int		depth;
				int		folding;
				vect3	scale;
				vect3	rotation;
				vect3	translation;
				int		visible;
				int		locking;
				int		shading;
				float	facet;
				rgb		color;
				int		color_type;
				vector<vect3>	tbl_vertex;
				vector<Face>	tbl_face;
			};
			
			Scene				scene;
			map<string,Material> 	tbl_material;
			map<string,Object>		tbl_object;

			map<int,string> 	tbl_material_name;	// 番号から名前を引き当てる

			//------------------------------------------------------------------------------
			void	dump()
			//------------------------------------------------------------------------------
			{
				// Scene
				cout << "Scene.pos               " << scene.pos.x << "," << scene.pos.y << "," << scene.pos.z << endl;
				cout << "Scene.lookat            " << scene.lookat.x << "," << scene.lookat.y << "," << scene.lookat.z << endl;
				cout << "Scene.head              " << scene.head << endl;
				cout << "Scene.pich              " << scene.pich << endl;
				cout << "Scene.bank              " << scene.bank << endl;
				cout << "Scene.ortho             " << scene.ortho << endl;
				cout << "Scene.zoom2             " << scene.zoom2 << endl;
				cout << "Scene.amb               " << scene.amb.x << "," << scene.amb.y << "," << scene.amb.z << endl;
				cout << "Scene.frontclip         " << scene.frontclip << endl;
				cout << "Scene.backclip          " << scene.backclip << endl;
				for ( unsigned int i = 0 ; i < scene.tbl_dirlights.size() ; i++ )
				{
					cout << "Scene.dirlight[" << to_string(i) << "]" << ".dir   " <<scene.tbl_dirlights[i].dir.x << "," <<scene.tbl_dirlights[i].dir.y << "," <<scene.tbl_dirlights[i].dir.z << endl;
					cout << "Scene.dirlight[" << to_string(i) << "]" << ".color " <<scene.tbl_dirlights[i].color.x << "," <<scene.tbl_dirlights[i].color.y << "," <<scene.tbl_dirlights[i].color.z << endl;
				}

				// Material
				for ( auto a : tbl_material )
				{
					cout << "Material[\"" << a.first << "\"].shader    " << to_string(a.second.shader) << endl;
					cout << "Material[\"" << a.first << "\"].rgba      " << to_string(a.second.col.r) << "," << to_string(a.second.col.g) << "," << to_string(a.second.col.b) << "," << to_string(a.second.col.a) << endl;
					cout << "Material[\"" << a.first << "\"].dif       " << to_string(a.second.dif) << endl;
					cout << "Material[\"" << a.first << "\"].amb       " << to_string(a.second.amb) << endl;
					cout << "Material[\"" << a.first << "\"].emi       " << to_string(a.second.emi) << endl;
					cout << "Material[\"" << a.first << "\"].spc       " << to_string(a.second.spc) << endl;
					cout << "Material[\"" << a.first << "\"].power     " << to_string(a.second.power) << endl;
				}

				// Object
				for ( auto a : tbl_object )
				{
					cout << "Object[\"" << a.first << "\"].depth       " << a.second.depth << endl;
					cout << "Object[\"" << a.first << "\"].folding     " << a.second.folding << endl;
					cout << "Object[\"" << a.first << "\"].scale       " << to_string(a.second.scale.x) << "," << to_string(a.second.scale.y) << "," << to_string(a.second.scale.z) << endl;
					cout << "Object[\"" << a.first << "\"].rotation    " << to_string(a.second.rotation.x) << "," << to_string(a.second.rotation.y) << "," << to_string(a.second.rotation.z) << endl;
					cout << "Object[\"" << a.first << "\"].translation " << to_string(a.second.translation.x) << "," << to_string(a.second.translation.y) << "," << to_string(a.second.translation.z) << endl;
					cout << "Object[\"" << a.first << "\"].visible     " << a.second.visible << endl;
					cout << "Object[\"" << a.first << "\"].locking     " << a.second.locking << endl;
					cout << "Object[\"" << a.first << "\"].shading     " << a.second.shading << endl;
					cout << "Object[\"" << a.first << "\"].facet       " << a.second.facet << endl;
					cout << "Object[\"" << a.first << "\"].color       " << to_string(a.second.color.r) << "," << to_string(a.second.color.g) << "," << to_string(a.second.color.b) << endl;
					cout << "Object[\"" << a.first << "\"].color_type  " << a.second.color_type << endl;

					for ( unsigned int j = 0 ; j < a.second.tbl_vertex.size() ; j++ )
					{
						cout << "Object[\"" << a.first << "\"].vertex["<<to_string(j)<<"]   " << a.second.tbl_vertex[j].x << "," << a.second.tbl_vertex[j].y << "," << a.second.tbl_vertex[j].z << endl;
					}
					for ( unsigned int j = 0 ; j < a.second.tbl_face.size() ; j++ )
					{
						// V
						cout << "Object[\"" << a.first << "\"].face["<<to_string(j)<<"].V   ";
						for ( unsigned int k = 0 ; k < a.second.tbl_face[j].V.size() ; k++ )
						{
							cout << a.second.tbl_face[j].V[k] <<" ";
						}
						cout << endl;

						// M
						cout << "Object[\"" << a.first << "\"].face["<<to_string(j)<<"].M   ";
						cout << a.second.tbl_face[j].M << " "; 
						cout << endl;

						// UV
						cout << "Object[\"" << a.first << "\"].face["<<to_string(j)<<"].UV  ";
						for ( unsigned int k = 0 ; k < a.second.tbl_face[j].UV.size() ; k++ )
						{
							cout << a.second.tbl_face[j].UV[k].x << ",";
							cout << a.second.tbl_face[j].UV[k].y << ",";
						}
						cout << endl;

					}
				}

			}
		};

		struct Token_MQO
		{
			enum Type
			{
				TYPE_NONE,	// 0:
				TYPE_ALPHA,	// 1:	isalpha	abcd..
				TYPE_NUM,	// 2:	1234..
				TYPE_SPACE,	// 3:	0x20, \t, ..
				TYPE_CALC,	// 4:	ispunct	+,*,
				TYPE_STR,	// 5:	"..."
			//	TYPE_CTRL,	// 6:	/n /r	...
			};
			private:string text;
			private:int		idx;
			private:bool	bComplete;

			public:string	word;
			public:Type		type;

			//------------------------------------------------------------------------------
			Token_MQO( string filename )
			//------------------------------------------------------------------------------
			{
				fstream fi( filename, ios::in );
				string buf;

				while ( getline( fi, buf ) )
				{
					text += buf+" ";
				}

				idx	= 0;
				type = TYPE_NONE;
				word = "";
				bComplete = false;
			}
			
			//------------------------------------------------------------------------------
			bool GetWord()
			//------------------------------------------------------------------------------
			{
				if ( bComplete )
				{
					bComplete = false;
					type = TYPE_NONE;
					word = "";
				}
				
				while( idx < (signed)text.length() )
				{
					char c = text[idx++]; 

					auto func = []( char c )
					{
						Type	t = TYPE_NONE;
							 if ( c=='"' )				t = TYPE_STR;
						else if ( c==' ' || c=='\t')	t = TYPE_SPACE;
						else if ( c>='a' && c<='z')		t = TYPE_ALPHA;
						else if ( c>='A' && c<='Z')		t = TYPE_ALPHA;
						else if ( c=='_')				t = TYPE_ALPHA;
						else if ( c>='0' && c<='9')		t = TYPE_NUM;
						else if ( c>='.')				t = TYPE_NUM;
						else if ( c>='-')				t = TYPE_NUM;
						else if ( c=='{' || c=='}') 	t = TYPE_CALC;
						else if ( c=='(' || c==')') 	t = TYPE_CALC;
						else t = TYPE_ALPHA;
						return t;
					};

				
					Type t = func(c);

					// トークンの始まり
					if ( type == TYPE_NONE )
					{
						if ( t==TYPE_SPACE ) {}
						else
						if ( t==TYPE_STR ) 
						{
							type=t;
						}
						else
						{
							type = t;
							word = c;
						}
					}
					else
					// 文字列トークン
					if ( type == TYPE_STR )
					{
						if ( t==TYPE_STR ) 
						{
							// 文字列が閉じれば、トークン完成
							bComplete = true;
							break;
						}
						else
						{
							word += c;
						}
					}
					else
					// 通常トークン
					{
						if ( type == TYPE_ALPHA && t == TYPE_NUM ) t = TYPE_ALPHA;	//英＋数字は英として扱う

						if ( t==type )
						{
							word += c;
						}
						else
						{
							// 違うタイプの文字が来たら、トークン完成
							bComplete = true;
							idx--;
							break;
						}
					}
				}
			
				return ( idx < (signed)text.length() );
			}
		};

		MQO mqo;	// 保存先

		//------------------------------------------------------------------------------
		void LoadFile( string filename )	// 定型フォーマットを順に読み込む方式の、簡易実装
		//------------------------------------------------------------------------------
		{
			Token_MQO token( filename );	// 読み込み元

			int cntObject =0;

			while( token.GetWord() )
			{
				if ( token.word=="Thumbnail" )
				{
					// このタグは無視
				}

				if ( token.word=="Format" )
				{
					// バージョンチェック	Ver 1.1のみ
					token.GetWord();	// "Text
					token.GetWord();	// "Ver"
					token.GetWord();	if ( token.word != "1.1" ) { cout<< ".mqo Ver "<<token.word<< " is not support" << endl; exit(1);};
				}
				
				if ( token.word=="Scene" )
				{
					token.GetWord();	// "{"

					token.GetWord();	// "pos"
					token.GetWord();	mqo.scene.pos.x = stof(token.word);
					token.GetWord();	mqo.scene.pos.y = stof(token.word);
					token.GetWord();	mqo.scene.pos.z = stof(token.word);

					token.GetWord();	// "lookat"
					token.GetWord();	mqo.scene.lookat.x = stof(token.word);
					token.GetWord();	mqo.scene.lookat.y = stof(token.word);
					token.GetWord();	mqo.scene.lookat.z = stof(token.word);

					token.GetWord();	// "head"
					token.GetWord();	mqo.scene.head = stof(token.word);

					token.GetWord();	// "pich"
					token.GetWord();	mqo.scene.pich = stof(token.word);

					token.GetWord();	// "bank"
					token.GetWord();	mqo.scene.bank = stof(token.word);

					token.GetWord();	// "ortho"
					token.GetWord();	mqo.scene.ortho = stof(token.word);

					token.GetWord();	// "zoom2"
					token.GetWord();	mqo.scene.zoom2 = stof(token.word);
		
					token.GetWord();	// "amb"
					token.GetWord();	mqo.scene.amb.x = stof(token.word);
					token.GetWord();	mqo.scene.amb.y = stof(token.word);
					token.GetWord();	mqo.scene.amb.z = stof(token.word);

					token.GetWord();	// "frontclip"
					token.GetWord();	mqo.scene.frontclip = stof(token.word);

					token.GetWord();	// "backclip"
					token.GetWord();	mqo.scene.backclip = stof(token.word);

					int	cnt = 0;
					token.GetWord();	// "dirlights"
					token.GetWord();	cnt = stof(token.word);
					token.GetWord();	// "{"

					for ( int i=0 ; i < cnt ; i++ )
					{
						vector<MQO::Scene::Dirlights>& tbl = mqo.scene.tbl_dirlights;
						tbl.emplace_back();
						token.GetWord();	// "light"
						token.GetWord();	// "{"

						token.GetWord();	// "dir"
						token.GetWord();	tbl[i].dir.x = stof(token.word);
						token.GetWord();	tbl[i].dir.y = stof(token.word);
						token.GetWord();	tbl[i].dir.z = stof(token.word);

						token.GetWord();	// "color"
						token.GetWord();	tbl[i].color.x = stof(token.word);
						token.GetWord();	tbl[i].color.y = stof(token.word);
						token.GetWord();	tbl[i].color.z = stof(token.word);

						token.GetWord();	// "}"
					}
				
				}
				if ( token.word=="Material" )
				{
					int cnt = 0;
					token.GetWord();	cnt = stof(token.word);

					token.GetWord();	// "{"

					for ( int i=0 ; i < cnt ; i++ )
					{
						map<string,MQO::Material>& tbl = mqo.tbl_material;

						//	(name)
						string	name;
						token.GetWord();	name = token.word;

						mqo.tbl_material_name[i] = name;	

						token.GetWord();	// "shader"
						token.GetWord();	// "("
						token.GetWord();	tbl[name].shader = stof(token.word);
						token.GetWord();	// ")"

						token.GetWord();	// "col"
						token.GetWord();	// "("
						token.GetWord();	tbl[name].col.r = stof(token.word);
						token.GetWord();	tbl[name].col.g = stof(token.word);
						token.GetWord();	tbl[name].col.b = stof(token.word);
						token.GetWord();	tbl[name].col.a = stof(token.word);
						token.GetWord();	// ")"

						token.GetWord();	// "dif"
						token.GetWord();	// "("
						token.GetWord();	tbl[name].dif = stof(token.word);
						token.GetWord();	// ")"

						token.GetWord();	// "amb"
						token.GetWord();	// "("
						token.GetWord();	tbl[name].amb = stof(token.word);
						token.GetWord();	// ")"

						token.GetWord();	// "emi"
						token.GetWord();	// "("
						token.GetWord();	tbl[name].emi = stof(token.word);
						token.GetWord();	// ")"

						token.GetWord();	// "spc"
						token.GetWord();	// "("
						token.GetWord();	tbl[name].spc = stof(token.word);
						token.GetWord();	// ")"

						token.GetWord();	// "powere"
						token.GetWord();	// "("
						token.GetWord();	tbl[name].power = stof(token.word);
						token.GetWord();	// ")"

					}
				}
				if ( token.word=="Object" )
				{
					int i = cntObject++;

					map<string,MQO::Object>& tbl = mqo.tbl_object;

					//	(name)
					string	name;
					token.GetWord();	name = token.word;
					token.GetWord();	// "{"

					token.GetWord();	// "depth"
					token.GetWord();	tbl[name].depth = stof(token.word);

					token.GetWord();	// "folding"
					token.GetWord();	tbl[name].folding = stof(token.word);

					token.GetWord();	// "scale"
					token.GetWord();	tbl[name].scale.x = stof(token.word);
					token.GetWord();	tbl[name].scale.y = stof(token.word);
					token.GetWord();	tbl[name].scale.z = stof(token.word);

					token.GetWord();	// "rotation"
					token.GetWord();	tbl[name].rotation.x = stof(token.word);
					token.GetWord();	tbl[name].rotation.y = stof(token.word);
					token.GetWord();	tbl[name].rotation.z = stof(token.word);

					token.GetWord();	// "translation"
					token.GetWord();	tbl[name].translation.x = stof(token.word);
					token.GetWord();	tbl[name].translation.y = stof(token.word);
					token.GetWord();	tbl[name].translation.z = stof(token.word);

					token.GetWord();	// "visible"
					token.GetWord();	tbl[name].visible = stof(token.word);

					token.GetWord();	// "locking"
					token.GetWord();	tbl[name].locking = stof(token.word);

					token.GetWord();	// "shading"
					token.GetWord();	tbl[name].shading = stof(token.word);

					token.GetWord();	// "facet"
					token.GetWord();	tbl[name].facet = stof(token.word);

					token.GetWord();	// "color"
					token.GetWord();	tbl[name].color.r = stof(token.word);
					token.GetWord();	tbl[name].color.g = stof(token.word);
					token.GetWord();	tbl[name].color.b = stof(token.word);

					token.GetWord();	// "color_type"
					token.GetWord();	tbl[name].color_type = stof(token.word);

					token.GetWord();	// "vertex"
					{
						int cnt_vertex = 0;
						token.GetWord();	cnt_vertex = stof(token.word);
						token.GetWord();	// "{"
						for ( int j=0 ; j < cnt_vertex ; j++ )
						{
							vector<vect3>& tbl = mqo.tbl_object[name].tbl_vertex;
							tbl.emplace_back();

							token.GetWord();	tbl[j].x = stof(token.word);
							token.GetWord();	tbl[j].y = stof(token.word);
							token.GetWord();	tbl[j].z = stof(token.word);
						}
						token.GetWord();	// "}"
					}

					token.GetWord();	// "face"
					{
						int cnt_face = 0;
						token.GetWord();	cnt_face = stof(token.word);
						token.GetWord();	// "{"
						for ( int j=0 ; j < cnt_face ; j++ )
						{
							vector<MQO::Object::Face>& tbl = mqo.tbl_object[name].tbl_face;
							tbl.emplace_back();

							// verttex index counter
							int	cnt_idx = 0;
							token.GetWord();	cnt_idx = stof(token.word);

							token.GetWord();	// "V"
							token.GetWord();	// "("
							for ( int k=0 ; k < cnt_idx ; k++ )
							{
								tbl[j].V.emplace_back();
								token.GetWord();	tbl[j].V[k] = stof(token.word);
							}
							token.GetWord();	// ")"

							token.GetWord();	// "M"
							token.GetWord();	// "("
							token.GetWord();	tbl[j].M = stof(token.word);
							token.GetWord();	// ")"

							token.GetWord();	// "UV"
							token.GetWord();	// "("
							for ( int k=0 ; k < cnt_idx ; k++ )
							{
								tbl[j].UV.emplace_back();
								token.GetWord();	tbl[j].UV[k].u = stof(token.word);
								token.GetWord();	tbl[j].UV[k].v = stof(token.word);
							}
							token.GetWord();	// ")"

						}
						token.GetWord();	// "}"
					}
					token.GetWord();	// "}" for "Object"
				}
			}
		}
	};


	Parsar_MQO mdl;
};
Lab33::Lab33() : pImpl( new Lab33::Impl ){}

//------------------------------------------------------------------------------
void Lab33::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	gra.Print(1,(float)text_y++,"33 : .mqo reader" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 0.0, 1.1,  8.988 );
		pers.cam.at = vect3( 0, 1.0, 0 );
		pers.cam.Update();

		cout<<fixed<<setprecision(8);	// 浮動小数出力桁数

		pImpl->mdl.LoadFile( "golf.mqo" );

	}

	if ( keys.R.hi ) pImpl->bResetAll = true;

	// モデル表示
	{
//		int cnt = 0;

		Impl::Parsar_MQO::MQO::Object& obj = pImpl->mdl.mqo.tbl_object["hole01"];

if(1)
{					vect3 v(0,0,0);
					vect3 v0 = v+vect3( 3, 0, 0);//obj.tbl_vertex[ 38 ];
					vect3 v1 = v+vect3( 0, 0, 10);//obj.tbl_vertex[ 42 ];
					vect3 v2 = v+vect3(-3, 0, 0);//obj.tbl_vertex[ 46 ];

					float w;					
					v0 = v0 * pers.cam.mat.invers();
					w = pers.getW(v0.z);
					v0.x = v0.x* w / pers.aspect;
					v0.y = v0.y* w 	;
					v0.z = w*pers.rate_w;	// 三次元ベクトルで返す都合上、ZにW値を入れている。

					v1 = v1 * pers.cam.mat.invers();
					w = pers.getW(v1.z);
					v1.x = v1.x* w / pers.aspect;
					v1.y = v1.y* w 	;
					v1.z = w*pers.rate_w;	// 三次元ベクトルで返す都合上、ZにW値を入れている。

					v2 = v2 * pers.cam.mat.invers();
					w = pers.getW(v2.z);
					v2.x = v2.x* w / pers.aspect;
					v2.y = v2.y* w 	;
					v2.z = w*pers.rate_w;	// 三次元ベクトルで返す都合上、ZにW値を入れている。

					if ( keys.E.rep ) pers.cam.pos.z+=0.01;
					if ( keys.D.rep ) pers.cam.pos.z-=0.01;
					if ( keys.W.hi ) pers.cam.pos.z+=0.01;
					if ( keys.S.hi ) pers.cam.pos.z-=0.01;
					if ( keys.Q.hi ) pers.cam.pos.z+=0.001;
					if ( keys.A.hi ) pers.cam.pos.z-=0.001;

				//	if ( v0.z > 0 && v1.z > 0 && v2.z > 0 ) 
v0.z=0;
v1.z=0;
v2.z=0;
gra.SetCulling(false);
					gra.Tri2d( v0.xy(), v1.xy(), v2.xy(), rgb(1,0,0) );
					gra.Pset2d(v1.xy(),rgb(1,1,0),5);
//					pers.pen.Print2d( gra, pers, vect2(0,0),0,0,  to_string(pers.cam.pos.z) );
					pers.pen.Print2d( gra, pers, vect2(0,0),0,32*0, "cam "+to_string(pers.cam.pos.x)+" "+ to_string(pers.cam.pos.y)+" "+ to_string(pers.cam.pos.z) );
					pers.pen.Print2d( gra, pers, vect2(0,0),0,32*1, to_string(v0.x)+" "+ to_string(v0.y)+" "+ to_string(v0.z) );
					pers.pen.Print2d( gra, pers, vect2(0,0),0,32*2, to_string(v1.x)+" "+ to_string(v1.y)+" "+ to_string(v1.z) );
					pers.pen.Print2d( gra, pers, vect2(0,0),0,32*3, to_string(v2.x)+" "+ to_string(v2.y)+" "+ to_string(v2.z) );

					gra.Tri2d( vect2(0.7,0), vect2(0.6,0), vect2(0,-30000000),rgb(0,1,0));
}
else
		for ( Impl::Parsar_MQO::MQO::Object::Face face : obj.tbl_face )
		{
			if ( face.V.size() == 3 ) // 三角形ポリゴンのみ処理
			{
				Impl::Parsar_MQO::MQO::Material	m = pImpl->mdl.mqo.tbl_material[ pImpl->mdl.mqo.tbl_material_name[ face.M ] ];

				//for ( int a : face.V )
//				if ( face.V[0]==41 && face.V[1]==39 &&face.V[0]==41 )
				{
					vect3 v0 = obj.tbl_vertex[ face.V[0] ];
					vect3 v1 = obj.tbl_vertex[ face.V[1] ];
					vect3 v2 = obj.tbl_vertex[ face.V[2] ];
					
					pers.pen.Tri3d( gra, pers, v0, v1 ,v2 , m.col.rgb() );
					pers.pen.Line3d( gra, pers, v0, v1 );
					pers.pen.Line3d( gra, pers, v0, v2 );
					pers.pen.Line3d( gra, pers, v2, v1 );
//if ( cnt++ < 100 ) break;
				}
			}
		}
	}


}
