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

	struct MQO_1_1
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
			string	name;
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
				int				cnt;	//	頂点数
				vector<int>		V;
				int				M;
				vector<vect2>	UV;
			};
		
			string	name;
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
		vector<Material> 	tbl_material;
		vector<Object>		tbl_object;

		//------------------------------------------------------------------------------
		void	show()
		//------------------------------------------------------------------------------
		{
			// Scene
			cout << "Scene.pos       " << scene.pos.x << " , " << scene.pos.y << " , " << scene.pos.z << endl;
			cout << "Scene.lookat    " << scene.lookat.x << " , " << scene.lookat.y << " , " << scene.lookat.z << endl;
			cout << "Scene.head      " << scene.head << endl;
			cout << "Scene.pich      " << scene.pich << endl;
			cout << "Scene.bank      " << scene.bank << endl;
			cout << "Scene.ortho     " << scene.ortho << endl;
			cout << "Scene.zoom2     " << scene.zoom2 << endl;
			cout << "Scene.amb       " << scene.amb.x << " , " << scene.amb.y << " , " << scene.amb.z << endl;
			cout << "Scene.frontclip " << scene.frontclip << endl;
			cout << "Scene.backclip  " << scene.backclip << endl;
			for ( unsigned int i = 0 ; i < scene.tbl_dirlights.size() ; i++ )
			{
				cout << "Scene.dirlight[" << to_string(i) << "]" << "dir   " <<scene.tbl_dirlights[i].dir.x << " , " <<scene.tbl_dirlights[i].dir.y << " , " <<scene.tbl_dirlights[i].dir.z << endl;
				cout << "Scene.dirlight[" << to_string(i) << "]" << "color " <<scene.tbl_dirlights[i].color.x << " , " <<scene.tbl_dirlights[i].color.y << " , " <<scene.tbl_dirlights[i].color.z << endl;
			}

			// Material
			for ( unsigned int i = 0 ; i < tbl_material.size() ; i++ )
			{
				cout << "Material[" << to_string(i) << "]" << ".name   " << tbl_material[i].name << endl;
				cout << "Material[" << to_string(i) << "]" << ".shader " << to_string(tbl_material[i].shader) << endl;
				cout << "Material[" << to_string(i) << "]" << ".rgba   " << to_string(tbl_material[i].col.r) << " , " << to_string(tbl_material[i].col.g) << " , " << to_string(tbl_material[i].col.b) << " , " << to_string(tbl_material[i].col.a) << endl;
				cout << "Material[" << to_string(i) << "]" << ".dif " << to_string(tbl_material[i].dif) << endl;
				cout << "Material[" << to_string(i) << "]" << ".amb " << to_string(tbl_material[i].amb) << endl;
				cout << "Material[" << to_string(i) << "]" << ".emi " << to_string(tbl_material[i].emi) << endl;
				cout << "Material[" << to_string(i) << "]" << ".spc " << to_string(tbl_material[i].spc) << endl;
				cout << "Material[" << to_string(i) << "]" << ".power " << to_string(tbl_material[i].power) << endl;
			}

			// Object
			for ( unsigned int i = 0 ; i < tbl_object.size() ; i++ )
			{
				cout << "Object[" << to_string(i) << "]" << ".name        " << tbl_object[i].name << endl;
				cout << "Object[" << to_string(i) << "]" << ".depth       " << tbl_object[i].depth << endl;
				cout << "Object[" << to_string(i) << "]" << ".folding     " << tbl_object[i].folding << endl;
				cout << "Object[" << to_string(i) << "]" << ".scale       " << to_string(tbl_object[i].scale.x) << " , " << to_string(tbl_object[i].scale.y) << " , " << to_string(tbl_object[i].scale.z) << endl;
				cout << "Object[" << to_string(i) << "]" << ".rotation    " << to_string(tbl_object[i].rotation.x) << " , " << to_string(tbl_object[i].rotation.y) << " , " << to_string(tbl_object[i].rotation.z) << endl;
				cout << "Object[" << to_string(i) << "]" << ".translation " << to_string(tbl_object[i].translation.x) << " , " << to_string(tbl_object[i].translation.y) << " , " << to_string(tbl_object[i].translation.z) << endl;
				cout << "Object[" << to_string(i) << "]" << ".visible     " << tbl_object[i].visible << endl;
				cout << "Object[" << to_string(i) << "]" << ".locking     " << tbl_object[i].locking << endl;
				cout << "Object[" << to_string(i) << "]" << ".shading     " << tbl_object[i].shading << endl;
				cout << "Object[" << to_string(i) << "]" << ".facet       " << tbl_object[i].facet << endl;
				cout << "Object[" << to_string(i) << "]" << ".color       " << to_string(tbl_object[i].color.r) << " , " << to_string(tbl_object[i].color.g) << " , " << to_string(tbl_object[i].color.b) << endl;
				cout << "Object[" << to_string(i) << "]" << ".color_type  " << tbl_object[i].color_type << endl;
			}

		}
	}mqo;


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
			
			while( idx < text.length() )
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
		
			return ( idx < text.length() );
		}
	};

};
Lab33::Lab33() : pImpl( new Lab33::Impl ){}

//------------------------------------------------------------------------------
void Lab33::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"33 : .mqo reader" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 0.0, 2.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );
		pers.cam.Update();


		Impl::Token_MQO token( "tst.mqo" );
		
//		cout << text << endl;
		cout << "--" << endl;

		int cntObject =0;

		while( token.GetWord() )
		{
			if ( token.word=="Thumbnail" )
			{
				// このタグは無視
			}

			if ( token.word=="Format" )
			{
				token.GetWord();	// "Text
				token.GetWord();	// "Ver"
				token.GetWord();	if ( token.word != "1.1" ) { cout<< ".mqo Ver "<<token.word<< " is not support" << endl; exit(1);};
			}
			
			if ( token.word=="Scene" )
			{
				token.GetWord();	// "{"

				token.GetWord();	// "pos"
				token.GetWord();	pImpl->mqo.scene.pos.x = stof(token.word);
				token.GetWord();	pImpl->mqo.scene.pos.y = stof(token.word);
				token.GetWord();	pImpl->mqo.scene.pos.z = stof(token.word);

				token.GetWord();	// "lookat"
				token.GetWord();	pImpl->mqo.scene.lookat.x = stof(token.word);
				token.GetWord();	pImpl->mqo.scene.lookat.y = stof(token.word);
				token.GetWord();	pImpl->mqo.scene.lookat.z = stof(token.word);

				token.GetWord();	// "head"
				token.GetWord();	pImpl->mqo.scene.head = stof(token.word);

				token.GetWord();	// "pich"
				token.GetWord();	pImpl->mqo.scene.pich = stof(token.word);

				token.GetWord();	// "bank"
				token.GetWord();	pImpl->mqo.scene.bank = stof(token.word);

				token.GetWord();	// "ortho"
				token.GetWord();	pImpl->mqo.scene.ortho = stof(token.word);

				token.GetWord();	// "zoom2"
				token.GetWord();	pImpl->mqo.scene.zoom2 = stof(token.word);
	
//cout <<token.word <<" ??? "<<endl;
				token.GetWord();	// "amb"
				token.GetWord();	pImpl->mqo.scene.amb.x = stof(token.word);
				token.GetWord();	pImpl->mqo.scene.amb.y = stof(token.word);
				token.GetWord();	pImpl->mqo.scene.amb.z = stof(token.word);

				token.GetWord();	// "frontclip"
				token.GetWord();	pImpl->mqo.scene.frontclip = stof(token.word);

				token.GetWord();	// "backclip"
				token.GetWord();	pImpl->mqo.scene.backclip = stof(token.word);

				int	cnt = 0;
				token.GetWord();	// "dirlights"
				token.GetWord();	cnt = stof(token.word);
				token.GetWord();	// "{"

				for ( int i=0 ; i < cnt ; i++ )
				{
					vector<Impl::MQO_1_1::Scene::Dirlights>& tbl = pImpl->mqo.scene.tbl_dirlights;
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
					vector<Impl::MQO_1_1::Material>& tbl = pImpl->mqo.tbl_material;
					tbl.emplace_back();

					//	(name)
					token.GetWord();	tbl[i].name = token.word;

					token.GetWord();	// "shader"
					token.GetWord();	// "("
					token.GetWord();	tbl[i].shader = stof(token.word);
					token.GetWord();	// ")"

					token.GetWord();	// "col"
					token.GetWord();	// "("
					token.GetWord();	tbl[i].col.r = stof(token.word);
					token.GetWord();	tbl[i].col.g = stof(token.word);
					token.GetWord();	tbl[i].col.b = stof(token.word);
					token.GetWord();	tbl[i].col.a = stof(token.word);
					token.GetWord();	// ")"

					token.GetWord();	// "dif"
					token.GetWord();	// "("
					token.GetWord();	tbl[i].dif = stof(token.word);
					token.GetWord();	// ")"

					token.GetWord();	// "amb"
					token.GetWord();	// "("
					token.GetWord();	tbl[i].amb = stof(token.word);
					token.GetWord();	// ")"

					token.GetWord();	// "emi"
					token.GetWord();	// "("
					token.GetWord();	tbl[i].emi = stof(token.word);
					token.GetWord();	// ")"

					token.GetWord();	// "spc"
					token.GetWord();	// "("
					token.GetWord();	tbl[i].spc = stof(token.word);
					token.GetWord();	// ")"

					token.GetWord();	// "powere"
					token.GetWord();	// "("
					token.GetWord();	tbl[i].power = stof(token.word);
					token.GetWord();	// ")"

				}
			}
			if ( token.word=="Object" )
			{
				int i = cntObject++;

				vector<Impl::MQO_1_1::Object>& tbl = pImpl->mqo.tbl_object;
				tbl.emplace_back();

				//	(name)
				token.GetWord();	tbl[i].name = token.word;
				token.GetWord();	// "{"

				token.GetWord();	// "depth"
				token.GetWord();	tbl[i].depth = stof(token.word);

				token.GetWord();	// "folding"
				token.GetWord();	tbl[i].folding = stof(token.word);

				token.GetWord();	// "scale"
				token.GetWord();	tbl[i].scale.x = stof(token.word);
				token.GetWord();	tbl[i].scale.y = stof(token.word);
				token.GetWord();	tbl[i].scale.z = stof(token.word);

				token.GetWord();	// "rotation"
				token.GetWord();	tbl[i].rotation.x = stof(token.word);
				token.GetWord();	tbl[i].rotation.y = stof(token.word);
				token.GetWord();	tbl[i].rotation.z = stof(token.word);

				token.GetWord();	// "translation"
				token.GetWord();	tbl[i].translation.x = stof(token.word);
				token.GetWord();	tbl[i].translation.y = stof(token.word);
				token.GetWord();	tbl[i].translation.z = stof(token.word);

				token.GetWord();	// "visible"
				token.GetWord();	tbl[i].visible = stof(token.word);

				token.GetWord();	// "locking"
				token.GetWord();	tbl[i].locking = stof(token.word);

				token.GetWord();	// "shading"
				token.GetWord();	tbl[i].shading = stof(token.word);

				token.GetWord();	// "facet"
				token.GetWord();	tbl[i].facet = stof(token.word);

				token.GetWord();	// "color"
				token.GetWord();	tbl[i].color.r = stof(token.word);
				token.GetWord();	tbl[i].color.g = stof(token.word);
				token.GetWord();	tbl[i].color.b = stof(token.word);

				token.GetWord();	// "color_type"
				token.GetWord();	tbl[i].color_type = stof(token.word);

				token.GetWord();	// "vertex"
				{
					int cnt_vertex = 0;
					token.GetWord();	cnt_vertex = stof(token.word);
					token.GetWord();	// "{"
					for ( int j=0 ; j < cnt_vertex ; j++ )
					{
						vector<vect3>& tbl = pImpl->mqo.tbl_object[i].tbl_vertex;
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
						vector<Impl::MQO_1_1::Object::Face>& tbl = pImpl->mqo.tbl_object[i].tbl_face;
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
		

		cout<<fixed<<setprecision(8);	// 浮動小数出力桁数
		pImpl->mqo.show();

		//--
		

	}


	

	if ( keys.R.hi ) pImpl->bResetAll = true;

}
