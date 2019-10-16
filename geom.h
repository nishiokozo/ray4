// version 20170527
//	2019/06/25 to only vect3 & mat44
#include <cmath>
#include <cmath>
#ifndef GEOM_H
#define	GEOM_H
#include <cfloat>
typedef	float	MAT4[4][4];
typedef	float	MAT3[3][3];
typedef	float	MAT43[4][3];

void	mat4_rotateX( float* m, float th );
void	mat4_rotateY( float* m, float th );
void	mat4_rotateZ( float* m, float th );

//extern int	rgb( float r, float g , float b );


//#define	pi	3.141592654
//const static float pi = 3.141592654;
const static float pi = 3.14159265f;//35897932384626433832795;
const static float infinit =  FLT_MAX;//numeric_limits<float>::max();	//FLT_MAX


struct	I2
{
	union { int x, n0, p; };
	union { int y, n1, n; };
	I2(){};
	I2( int _x, int _y ) : x(_x), y(_y){}
};

struct	I3
{
	union { int x, n0, r; };
	union { int y, n1, g; };
	union { int z, n2, b; };
	I3(){};
	I3( int _x, int _y, int _z ) : x(_x), y(_y), z(_z){}
};

struct	I4
{
	union { int x, n0, r; };
	union { int y, n1, g; };
	union { int z, n2, b; };
	union { int w, n3, a; };
	I4(){};
	I4( int _x, int _y, int _z , int _w ) : x(_x), y(_y), z(_z), w(_w){}
};

struct	D2
{
	union { float x,u; };
	union { float y,v; };
	D2( float _x, float _y ) : x(_x), y(_y){}
};

struct	D3
{
	union { float x,r;	};
	union { float y,g;	};
	union { float z,b;	};
	D3(){};
	D3( D2 v, float _z ) : x(v.x), y(v.y), z(_z){}
	D3( float _x, float _y, float _z ) : x(_x), y(_y), z(_z){}
};

struct	D4
{
	union { float x, u, r; };
	union { float y, v, g; };
	union { float z, s, b; };
	union { float w, t, a; };
	D4(){};
	D4( float _x, float _y, float _z, float _w ) : x(_x), y(_y), z(_z), w(_w){}
	D4( D2 v, float _z, float _w ) : x(v.x), y(v.y), z(_z), w(_w){}
	D4( D3 v, float _w ) : x(v.x), y(v.y), z(v.z), w(_w){}
};

struct	ivect2 : public I2
{
	ivect2( int _p, int _n ) 
	{
		x=_p;
		y=_n;
	}
};

struct	ivect3 : public I3
{
	ivect3( int _n0,  int _n1, int _n2 ) 
	{
		n0 = _n0;
		n1 = _n1;
		n2 = _n2;
	}
};

struct	ivect4 : public I4
{
	ivect4( int _n0,  int _n1, int _n2, int _n3 ) 
	{
		n0 = _n0;
		n1 = _n1;
		n2 = _n2;
		n3 = _n3;
	}
};

class vect2
{
public:
//	float x,y;
	union { float x,w;};
	union { float y,h;};

	vect2() :x(0),y(0){}
	vect2( float f ) :x(f),y(f){}
	vect2( float _x, float _y) :x(_x),y(_y){}

//	vect2 abs() { return vect2( std::abs(x) , std::abs(y) ); }
	vect2 normalize() { return (*this)/abs(); }
	float abs() { return sqrt( x*x + y*y ); }

	void operator= ( vect2 v )  { x =  v.x; y =  v.y; }
	void operator*=( vect2 v )  { x *= v.x; y *= v.y; }
	void operator/=( vect2 v )  { x =  v.x; y =  v.y; }
	void operator+=( vect2 v )  { x += v.x; y += v.y; }
	void operator-=( vect2 v )  { x -= v.x; y -= v.y; }

	vect2 operator*( vect2 v ) const { return vect2( x * v.x, y * v.y ); }
	vect2 operator/( vect2 v ) const { return vect2( x / v.x, y / v.y ); }
	vect2 operator+( vect2 v ) const { return vect2( x + v.x, y + v.y ); }
	vect2 operator-( vect2 v ) const { return vect2( x - v.x, y-  v.y ); }

	vect2 operator-() const { return vect2( -x, -y ); } 
	vect2 operator+() const { return vect2(  x,  y ); } 

	void operator= ( float f ) { x =  f  ; y =  f  ; }
	void operator*=( float f ) { x *= f  ; y *= f  ; }
	void operator/=( float f ) { x /= f  ; y /= f  ; }
	void operator+=( float f ) { x += f  ; y += f  ; }
	void operator-=( float f ) { x -= f  ; y -= f  ; }

	vect2 operator*( float f ) const { return vect2( x * f, y * f ); } 
	vect2 operator/( float f ) const { return vect2( x / f, y / f ); } 
	vect2 operator+( float f ) const { return vect2( x + f, y + f ); } 
	vect2 operator=( float f ) const { return vect2( x - f, y - f ); } 

	friend	vect2 operator*( float f, vect2 v )  { return vect2( f * v.x, f * v.y ); }
	friend	vect2 operator/( float f, vect2 v )  { return vect2( f / v.x, f / v.y ); }
	friend	vect2 operator+( float f, vect2 v )  { return vect2( f + v.x, f + v.y ); }
	friend	vect2 operator-( float f, vect2 v )  { return vect2( f - v.x, f - v.y ); }

	void dump() const
	{
		printf("(vect2)%9.6f %9.6f\n", x, y );
	}
	void dump( const char* str ) const
	{
		printf("%s<%9.6f %9.6f>\n", str, x, y );
	}

};

extern	float deg2rad( float n );
extern	float rad2deg( float n );
extern	float atan2_360( float y, float x );
extern	float atan2_180( float y, float x );

class mat44;
class mat33;
class vect3;

typedef vect3 rgb;

extern mat33 midentity();
extern mat33 mrotx( float f);
extern mat33 mroty( float f);
extern mat33 mrotz( float f);



class vect3
{
public:
	union { float	x, r, pitch; };
	union { float	y, g, head,	yaw; }; 
	union {	float	z, b, bank,	roll; };


	vect3() :x(0),y(0),z(0){};
	vect3( float f ) :x(f), y(f), z(f){};
	vect3( float _x, float _y, float _z) :x(_x), y(_y), z(_z){};

	vect3( vect2 v, float _z ) { x = v.x; y = v.y; z = _z;};

	vect2 xy() {return vect2(x,y);}
	
//	vect3 abs() { return vect3( std::abs(x) , std::abs(y) ); }
	vect3 normalize() { return (*this)/abs(); }
	float abs() { return sqrt( x*x + y*y + z*z ); }

	bool operator==( vect3 v ) { return ( x == v.x &&  y == v.y &&  z == v.z); }
	bool operator!=( vect3 v ) { return !( x == v.x &&  y == v.y &&  z == v.z); }

	vect3 operator*=( vect3 v ) { x *= v.x; y *= v.y; z *= v.z;  return *this; }
	vect3 operator/=( vect3 v ) { x /= v.x; y /= v.y; z /= v.z;  return *this; }
	vect3 operator+=( vect3 v ) { x += v.x; y += v.y; z += v.z;  return *this; }
	vect3 operator-=( vect3 v ) { x -= v.x; y -= v.y; z -= v.z;  return *this; }

	vect3 operator*( vect3 v ) const { vect3	ret; ret.x = x * v.x; ret.y = y * v.y; ret.z = z * v.z; return ret; }
	vect3 operator/( vect3 v ) const { vect3	ret; ret.x = x / v.x; ret.y = y / v.y; ret.z = z / v.z; return ret; }
	vect3 operator-( vect3 v ) const { vect3	ret; ret.x = x - v.x; ret.y = y - v.y; ret.z = z - v.z; return ret; }
	vect3 operator+( vect3 v ) const { vect3	ret; ret.x = x + v.x; ret.y = y + v.y; ret.z = z + v.z; return ret; }

//	vect3 operator-() const { vect3	ret; ret.x = -x; ret.y = -y; ret.z = -z; return ret; }
//	vect3 operator+() const { vect3	ret; ret.x =  x; ret.y =  y; ret.z =  z; return ret; }
	vect3 operator-() const { return vect3( -x, -y, -z ); } 
	vect3 operator+() const { return vect3(  x,  y,  z ); } 

	vect3 operator*( float f ) const { vect3	ret; ret.x = x * f; ret.y = y * f; ret.z = z * f; return ret; }
	vect3 operator/( float f ) const { float a = 1.0f / f; vect3	ret; ret.x = x * a; ret.y = y * a; ret.z = z * a; return ret; }

//	friend	vect3 operator*( float f, vect3 v )  { vect3	ret; ret.x = f * v.x; ret.y = f * v.y; ret.z = f * v.z;  return ret; }
//	friend	vect3 operator+( float f, vect3 v )  { vect3	ret; ret.x = f + v.x; ret.y = f + v.y; ret.z = f + v.z;  return ret; }

	friend	vect3 operator*( float f, vect3 v )  { return vect3( f * v.x, f * v.y, f * v.z ); }
	friend	vect3 operator/( float f, vect3 v )  { return vect3( f / v.x, f / v.y, f / v.z ); }
	friend	vect3 operator+( float f, vect3 v )  { return vect3( f + v.x, f + v.y, f + v.z ); }
	friend	vect3 operator-( float f, vect3 v )  { return vect3( f - v.x, f - v.y, f - v.z ); }

	void rotateByAxis( vect3 pos, vect3 axis, float th );
	void rotateByAxis( vect3 axis, float th );


	void dump() const
	{
		printf("(vect3)%9.6f %9.6f %9.6f\n", x, y, z );
	}

	void dump( const char* str ) const
	{
		printf("%s<%9.6f %9.6f %9.6f>\n", str, x, y, z );
	}

};


mat33 mrotateByAxis( vect3 axis, float th );

extern float	vect3_dotproduct( float a[3], float b[3] );
extern void		vect3_crossproduct( float v[3], float a[3], float b[3] );
extern vect3	       cross( vect3 a, vect3 b );
extern void		vect3_normalize( float v[3] );
extern void		vect3_normalize( vect3* v );
extern float	vect3_length( float v[3] );
extern void		vec4_multiplyMatrix( float v[4], float m[16], float a[4] );
extern float	dot( vect3 a, vect3 b );

extern vect3 refract( vect3 a, vect3 b, float n );
extern vect3 reflect( vect3 I, vect3 N );


extern vect2		min( vect2 a, vect2 b );
extern vect2		max( vect2 a, vect2 b );
extern vect3		min( vect3 a, vect3 b );
extern vect3		max( vect3 a, vect3 b );

extern float	max( float a, float b );
extern vect3		max( const float a, vect3 b );
extern vect3		max( vect3 b, float a );
extern float	min( float a, float b );
extern vect3		min( float a, vect3 b );
extern vect3		min( vect3 b, float a );
///
extern vect3	mix( float f, vect3 a, vect3 b );
extern float	mod( float a, float b );
extern float	length( vect3 a );
extern float	length( vect3 a, vect3 b );
//extern vect3	normalize( vect3 a );
extern vect3	normalize( vect3 a );


//#define	RAD(n)	((n)*M_PI/180.0f)
//#define	DEG(n)	((n)*180.0f/M_PI)
#define	M00	0
#define	M01	1
#define	M02	2
#define	M03	3
#define	M10	4
#define	M11	5
#define	M12	6
#define	M13	7
#define	M20	8
#define	M21	9
#define	M22	10
#define	M23	11
#define	M30	12
#define	M31	13
#define	M32	14
#define	M33	15

//#define	rad(n)	((n)*M_PI/180.0f)
//#define	deg(n)	((n)*180.0f/M_PI)

class	mat33
{
public:
	float m[3][3];

	//-----------------------------------------------------------------------------
	mat33(
	//-----------------------------------------------------------------------------
		float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22
	)
	{
		//	m[行][列]
		m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;
		m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;
		m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;
	}

	//-----------------------------------------------------------------------------
	mat33( const mat44 &m44 )
	//-----------------------------------------------------------------------------
	{
		m[0][0] = m[0][0];	m[0][1] = m[0][1];	m[0][2] = m[0][2];
		m[1][0] = m[1][0];	m[1][1] = m[1][1];	m[1][2] = m[1][2];
		m[2][0] = m[2][0];	m[2][1] = m[2][1];	m[2][2] = m[2][2];
	}

	//-----------------------------------------------------------------------------
	vect3 operator*( vect3 v ) const
	//-----------------------------------------------------------------------------
	{
		//	m[行][列] x v[列]
		return vect3(
			m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z ,
			m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z ,
			m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z 
		);
	}

	//-----------------------------------------------------------------------------
	friend	vect3 operator*( vect3 v, const mat33 m )
	//-----------------------------------------------------------------------------
	{
		//	v[行] x m[行][列]
		return vect3(
			v.x*m.m[0][0] + v.y*m.m[1][0] + v.z*m.m[2][0] ,
			v.x*m.m[0][1] + v.y*m.m[1][1] + v.z*m.m[2][1] ,
			v.x*m.m[0][2] + v.y*m.m[1][2] + v.z*m.m[2][2]
		);
	}

	//-----------------------------------------------------------------------------
	mat33 operator*( const mat33 m ) const
	//-----------------------------------------------------------------------------
	{
		const MAT3&	a = this->m;
		const MAT3&	b = m.m;

		//	a[行][列] x b[行][列]
		return mat33(
			a[0][0] * b[0][0] +  a[0][1] * b[1][0] +  a[0][2] * b[2][0],
			a[0][0] * b[0][1] +  a[0][1] * b[1][1] +  a[0][2] * b[2][1],
			a[0][0] * b[0][2] +  a[0][1] * b[1][2] +  a[0][2] * b[2][2],

			a[1][0] * b[0][0] +  a[1][1] * b[1][0] +  a[1][2] * b[2][0],
			a[1][0] * b[0][1] +  a[1][1] * b[1][1] +  a[1][2] * b[2][1],
			a[1][0] * b[0][2] +  a[1][1] * b[1][2] +  a[1][2] * b[2][2],

			a[2][0] * b[0][0] +  a[2][1] * b[1][0] +  a[2][2] * b[2][0],
			a[2][0] * b[0][1] +  a[2][1] * b[1][1] +  a[2][2] * b[2][1],
			a[2][0] * b[0][2] +  a[2][1] * b[1][2] +  a[2][2] * b[2][2]
		);
	}

	//-----------------------------------------------------------------------------
	mat33 operator*=( const mat33 a ) 
	//-----------------------------------------------------------------------------
	{

		*this = *this * a;
		
		return *this;
	}

	//-----------------------------------------------------------------------------
	void rotateByAxis( vect3 axis, float th )
	//-----------------------------------------------------------------------------
	{
		// x軸をaixsに合わせたマトリクスを作り
		
		float ry	= atan2( axis.z , axis.x );
		float lxz	= sqrt( axis.z * axis.z + axis.x*axis.x );
		float rz	= atan2( axis.y , lxz );
		mat33 mr = mrotz( rz )  * mroty( ry );

		// 作成した行列のaxis軸で回転
		(*this) *= mr;
		(*this) *= mrotx(th);
		(*this) *= mr.invers();

	}

	//-----------------------------------------------------------------------------
	mat33 invers()
	//-----------------------------------------------------------------------------
	{
#if 0

/*		return mat33(
			m[0][0],	m[0][1],	m[0][2],
			m[1][0],	m[1][1],	m[1][2],
			m[2][0],	m[2][1],	m[2][2]
		);
*/

#else

		mat33 A(*this);
		mat33 I(1,0,0, 0,1,0, 0,0,1);
		
		int d = 3;  //配列の次数

		//掃き出し法
		for( int i = 0 ; i < d ; i++ )
		{
			float f = 1/A.m[i][i];
			for( int j=0 ; j < d ; j++ )
			{
				A.m[i][j] *= f;
				I.m[i][j] *= f;
			}
			for( int j=0 ; j < d ; j++ )
			{
				if(i!=j)
				{
					f = A.m[j][i];
					for( int k=0 ; k < d ; k++ )
					{
						A.m[j][k] -= A.m[i][k] * f;
						I.m[j][k] -= I.m[i][k] * f;
					}
				}
			}
		}


		return I;
#endif

	}

	void dump()
	{
		printf("mat33:\n");
		printf("%9.6f %9.6f %9.6f \n", m[0][0], m[0][1], m[0][2] );
		printf("%9.6f %9.6f %9.6f \n", m[1][0], m[1][1], m[1][2] );
		printf("%9.6f %9.6f %9.6f \n", m[2][0], m[2][1], m[2][2] );
	}
	void dump( const char* str )
	{
		printf("mat33:%s\n", str);
		printf("%9.6f %9.6f %9.6f \n", m[0][0], m[0][1], m[0][2] );
		printf("%9.6f %9.6f %9.6f \n", m[1][0], m[1][1], m[1][2] );
		printf("%9.6f %9.6f %9.6f \n", m[2][0], m[2][1], m[2][2] );
	}

};



//---
class	mat44
{
public:

	union
	{
		float	m_array[16];
		MAT4	m;
	};
	mat44()
	{
		//	m[行][列]
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;	m[1][3] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}

	mat44(
		float m00, float m01, float m02, float m03, 
		float m10, float m11, float m12, float m13, 
		float m20, float m21, float m22, float m23, 
		float m30, float m31, float m32, float m33 
	)
	{
		//	m[行][列]
		m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;	m[0][3] = m03;
		m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;	m[1][3] = m13;
		m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;	m[2][3] = m23;
		m[3][0] = m30;	m[3][1] = m31;	m[3][2] = m32;	m[3][3] = m33;
	}


	vect3 operator*( vect3 v ) const
	{
		//	m[行][列] x v[列]
		return vect3(
			m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3] ,
			m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3] ,
			m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3] 
		);
	}

	friend	vect3 operator*( vect3 v, const mat44 m )
	{
		//	v[行] x m[行][列]
		return vect3(
			v.x*m.m[0][0] + v.y*m.m[1][0] + v.z*m.m[2][0] + 1.0*m.m[3][0] ,
			v.x*m.m[0][1] + v.y*m.m[1][1] + v.z*m.m[2][1] + 1.0*m.m[3][1] ,
			v.x*m.m[0][2] + v.y*m.m[1][2] + v.z*m.m[2][2] + 1.0*m.m[3][2] 
		);
	}

	mat44 operator*( const mat44 m ) const
	{
		const MAT4& a = this->m;
		const MAT4& b = m.m;

		//	a[行][列] x b[行][列]
		return mat44(
			a[0][0] * b[0][0] +  a[0][1] * b[1][0] +  a[0][2] * b[2][0] +  a[0][3] * b[3][0],
			a[0][0] * b[0][1] +  a[0][1] * b[1][1] +  a[0][2] * b[2][1] +  a[0][3] * b[3][1],
			a[0][0] * b[0][2] +  a[0][1] * b[1][2] +  a[0][2] * b[2][2] +  a[0][3] * b[3][2],
			a[0][0] * b[0][3] +  a[0][1] * b[1][3] +  a[0][2] * b[2][3] +  a[0][3] * b[3][3],

			a[1][0] * b[0][0] +  a[1][1] * b[1][0] +  a[1][2] * b[2][0] +  a[1][3] * b[3][0],
			a[1][0] * b[0][1] +  a[1][1] * b[1][1] +  a[1][2] * b[2][1] +  a[1][3] * b[3][1],
			a[1][0] * b[0][2] +  a[1][1] * b[1][2] +  a[1][2] * b[2][2] +  a[1][3] * b[3][2],
			a[1][0] * b[0][3] +  a[1][1] * b[1][3] +  a[1][2] * b[2][3] +  a[1][3] * b[3][3],

			a[2][0] * b[0][0] +  a[2][1] * b[1][0] +  a[2][2] * b[2][0] +  a[2][3] * b[3][0],
			a[2][0] * b[0][1] +  a[2][1] * b[1][1] +  a[2][2] * b[2][1] +  a[2][3] * b[3][1],
			a[2][0] * b[0][2] +  a[2][1] * b[1][2] +  a[2][2] * b[2][2] +  a[2][3] * b[3][2],
			a[2][0] * b[0][3] +  a[2][1] * b[1][3] +  a[2][2] * b[2][3] +  a[2][3] * b[3][3],

			a[3][0] * b[0][0] +  a[3][1] * b[1][0] +  a[3][2] * b[2][0] +  a[3][3] * b[3][0],
			a[3][0] * b[0][1] +  a[3][1] * b[1][1] +  a[3][2] * b[2][1] +  a[3][3] * b[3][1],
			a[3][0] * b[0][2] +  a[3][1] * b[1][2] +  a[3][2] * b[2][2] +  a[3][3] * b[3][2],
			a[3][0] * b[0][3] +  a[3][1] * b[1][3] +  a[3][2] * b[2][3] +  a[3][3] * b[3][3]
		);
	}

	mat44 operator*=( const mat44 a ) 
	{

		*this = *this * a;
		
		return *this;
	}

	float* GetArray(){ return m_array; };

	void identity()
	{
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;	m[1][3] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}

	void setRotateX( float rx )
	{
		float c=cos(rx);
		float s=sin(rx);
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = c;	m[1][2] = -s;	m[1][3] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = s;	m[2][2] = c;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}
	void setRotateY( float ry )
	{
		float c=cos(ry);
		float s=sin(ry);
		m[0][0] = c;	m[0][1] = 0.0f;	m[0][2] = s;	m[0][3] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0;	m[1][3] = 0.0f;
		m[2][0] = -s;	m[2][1] = 0.0;	m[2][2] = c;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}
	void setRotateZ( float rz )
	{
		float c=cos(rz);
		float s=sin(rz);
		m[0][0] = c;	m[0][1] = -s;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
		m[1][0] = s;	m[1][1] =  c;	m[1][2] = 0.0f;	m[1][3] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}
	
	void AddTranslate( vect3 pos )
	{
		m[3][0] += pos.x;
		m[3][1] += pos.y;
		m[3][2] += pos.z;
	}
	void SetTranslate( vect3 pos )
	{
		m[3][0] = pos.x;
		m[3][1] = pos.y;
		m[3][2] = pos.z;
	}
	void SetTranslate( float x, float y, float z )
	{
		m[3][0] = x;
		m[3][1] = y;
		m[3][2] = z;
	}

	vect3 GetVecX()
	{
		return vect3( m[0][0], m[0][1], m[0][2] );
	}
	vect3 GetVecY()
	{
		return vect3( m[1][0], m[1][1], m[1][2] );
	}
	vect3 GetVecZ()
	{
		return vect3( m[2][0], m[2][1], m[2][2] );
	}
	vect3 GetVecT()
	{
		return vect3( m[3][0], m[3][1], m[3][2] );
	}
	void SetVecX( vect3 v )
	{
		m[0][0] = v.x; 
		m[0][1] = v.y; 
		m[0][2] = v.z;
	}
	void SetVecY( vect3 v )
	{
		m[1][0] = v.x; 
		m[1][1] = v.y; 
		m[1][2] = v.z;
	}
	void SetVecZ( vect3 v )
	{
		m[2][0] = v.x; 
		m[2][1] = v.y; 
		m[2][2] = v.z;
	}
	void SetVecT( vect3 v )
	{
		m[3][0] = v.x; 
		m[3][1] = v.y; 
		m[3][2] = v.z;
	}

	void LookAt( vect3 pos, vect3 at, vect3 up )
	{
		vect3	z = (at-pos).normalize();
		vect3	x = cross( up, z ).normalize();
		vect3	y = cross( z, x ).normalize();
	
		m[0][0] =x.x;	m[0][1] =x.y;	m[0][2] =x.z;	m[0][3] = 0.0f;
		m[1][0] =y.x;	m[1][1] =y.y;	m[1][2] =y.z;	m[1][3] = 0.0f;
		m[2][0] =z.x;	m[2][1] =z.y;	m[2][2] =z.z;	m[2][3] = 0.0f;
		m[3][0] =pos.x;	m[3][1] =pos.y;	m[3][2] =pos.z;	m[3][3] = 1.0f;
	}
	void LookFor( vect3 pos, vect3 z, vect3 up )
	{
		vect3	x = cross( up, z );
		vect3	y = cross( z, x );
	
		m[0][0] =x.x;	m[0][1] =x.y;	m[0][2] =x.z;	m[0][3] = 0.0f;
		m[1][0] =y.x;	m[1][1] =y.y;	m[1][2] =y.z;	m[1][3] = 0.0f;
		m[2][0] =z.x;	m[2][1] =z.y;	m[2][2] =z.z;	m[2][3] = 0.0f;
		m[3][0] =pos.x;	m[3][1] =pos.y;	m[3][2] =pos.z;	m[3][3] = 1.0f;
	}
	void RotateXYZ( vect3 rot )
	{
		this->RotateX( rot.x );
		this->RotateY( rot.y );
		this->RotateZ( rot.z );
	}
	void RotateX( float f)
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= mat44(
			1,  0,  0,  0,
			0,  c, -s,  0,
			0,  s,  c,  0,
			0,  0,  0,  1
		);
	}
	void RotateY( float f)
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= mat44(
			c,  0, -s,  0,
			0,  1,  0,  0,
			s,  0,  c,  0,
			0,  0,  0,  1
		);
	}
	void RotateZ( float f)
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= mat44(
			c, -s,  0,  0,
			s,	c,  0,  0,
			0,  0,  1,  0,
			0,  0,  0,  1
		);
	}
	void Tlanslate( vect3 vec )
	{
		m[3][0] += vec.x;
		m[3][1] += vec.y;
		m[3][2] += vec.z;
	}
	void Tlanslate( float x, float y, float z )
	{
		m[3][0] += x;
		m[3][1] += y;
		m[3][2] += z;
	}
	
	void Scale( float x, float y, float z )
	{
		m[0][0] *= x;
		m[1][1] *= y;
		m[2][2] *= z;
	}
	void Scale( vect3 scale )
	{
		m[0][0] *= scale.x;
		m[1][1] *= scale.y;
		m[2][2] *= scale.z;
	}

	//-----------------------------------------------------------------------------
	mat33 GetRotate()
	//-----------------------------------------------------------------------------
	{
		return mat33(
			m[0][0],	m[0][1],	m[0][2],
			m[1][0],	m[1][1],	m[1][2],
			m[2][0],	m[2][1],	m[2][2]
		);
	}

	//-----------------------------------------------------------------------------
	mat44 invers()
	//-----------------------------------------------------------------------------
	{
#if 1

		mat44 A(*this);
		mat44 I(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
		
		int z1=4;  //配列の次数

		//掃き出し法
		for( int i = 0 ; i < z1 ; i++ )
		{
			float f =1/A.m[i][i];
			for( int j = 0 ; j < z1 ; j++ )
			{
				A.m[i][j] *= f;
				I.m[i][j] *= f;
			}
			for( int j = 0 ; j < z1 ; j++ )
			{
				if( i !=j )
				{
					f= A.m[j][i];
					for( int k = 0 ; k < z1 ; k++ )
					{
						A.m[j][k] -= A.m[i][k]*f;
						I.m[j][k] -= I.m[i][k]*f;
					}
				}
			}
		}
		return I;
#else
		float a[4][4];

		a[0][0] = m[0][0];
		a[0][1] = m[0][1]; 
		a[0][2] = m[0][2]; 
		a[0][3] = m[0][3]; 
		a[1][0] = m[1][0]; 
		a[1][1] = m[1][1]; 
		a[1][2] = m[1][2]; 
		a[1][3] = m[1][3]; 
		a[2][0] = m[2][0]; 
		a[2][1] = m[2][1]; 
		a[2][2] = m[2][2]; 
		a[2][3] = m[2][3]; 
		a[3][0] = m[3][0]; 
		a[3][1] = m[3][1]; 
		a[3][2] = m[3][2]; 
		a[3][3] = m[3][3]; 



//		float inv_a[4][4]; //ここに逆行列が入る
		mat44 inv_a;
		
		float buf; //一時的なデータを蓄える
		int i,j,k; //カウンタ
		int z1=4;  //配列の次数

		//単位行列を作る
		for(i=0;i<z1;i++)
		{
			for(j=0;j<z1;j++)
			{
				inv_a.m[i][j]=(i==j)?1.0:0.0;
			}
		}
		//掃き出し法
		for(i=0;i<z1;i++)
		{
			buf=1/a[i][i];
			for(j=0;j<z1;j++)
			{
				a[i][j]*=buf;
				inv_a.m[i][j]*=buf;
			}
			for(j=0;j<z1;j++)
			{
				if(i!=j)
				{
					buf=a[j][i];
					for(k=0;k<z1;k++)
					{
						a[j][k]-=a[i][k]*buf;
						inv_a.m[j][k]-=inv_a.m[i][k]*buf;
					}
				}
			}
		}
/*
		//逆行列を出力
		for(i=0;i<z1;i++)
		{
			for(j=0;j<z1;j++)
			{
				//	    printf(" %z2",inv_a[i][j]);
				m[i][j] = inv_a[i][j];
			}
			//	  printf("\n");
		}
*/
		return inv_a;
#endif
	}

	void dump()
	{
		printf("mat44:\n");
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[0][0], m[0][1], m[0][2], m[0][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[1][0], m[1][1], m[1][2], m[1][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[2][0], m[2][1], m[2][2], m[2][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[3][0], m[3][1], m[3][2], m[3][3] );
	}
	void dump( const char* str )
	{
		printf("mat44:%s\n", str);
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[0][0], m[0][1], m[0][2], m[0][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[1][0], m[1][1], m[1][2], m[1][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[2][0], m[2][1], m[2][2], m[2][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[3][0], m[3][1], m[3][2], m[3][3] );
	}

};

//---
class	mat43// : mat33
{
public:
	float m[4][3];
	float pos[3];

	//-----------------------------------------------------------------------------
	mat43(
	//-----------------------------------------------------------------------------
		float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22,
		float m30, float m31, float m32
	)
	{
		//	m[行][列]
		m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;
		m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;
		m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;
		pos[0] = m30;
		pos[1] = m31;
		pos[2] = m32;
	}

	//-----------------------------------------------------------------------------
	vect3 operator*( vect3 v ) const
	//-----------------------------------------------------------------------------
	{
		//	m[行][列] x v[列]
		return vect3(
			m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z ,
			m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z ,
			m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z 
		);
	}

	//-----------------------------------------------------------------------------
	friend	vect3 operator*( vect3 v, const mat43 m )
	//-----------------------------------------------------------------------------
	{
		//	v[行] x m[行][列]
		return vect3(
			v.x*m.m[0][0] + v.y*m.m[1][0] + v.z*m.m[2][0] ,
			v.x*m.m[0][1] + v.y*m.m[1][1] + v.z*m.m[2][1] ,
			v.x*m.m[0][2] + v.y*m.m[1][2] + v.z*m.m[2][2]
		);
	}

	//-----------------------------------------------------------------------------
	mat43 operator*( const mat43 m ) const
	//-----------------------------------------------------------------------------
	{
		const MAT43&	a = this->m;
		const MAT43&	b = m.m;

		//	a[行][列] x b[行][列]
		return mat43(
			a[0][0] * b[0][0] +  a[0][1] * b[1][0] +  a[0][2] * b[2][0] +  0.0f * b[3][0],
			a[0][0] * b[0][1] +  a[0][1] * b[1][1] +  a[0][2] * b[2][1] +  0.0f * b[3][1],
			a[0][0] * b[0][2] +  a[0][1] * b[1][2] +  a[0][2] * b[2][2] +  0.0f * b[3][2],

			a[1][0] * b[0][0] +  a[1][1] * b[1][0] +  a[1][2] * b[2][0] +  0.0f * b[3][0],
			a[1][0] * b[0][1] +  a[1][1] * b[1][1] +  a[1][2] * b[2][1] +  0.0f * b[3][1],
			a[1][0] * b[0][2] +  a[1][1] * b[1][2] +  a[1][2] * b[2][2] +  0.0f * b[3][2],

			a[2][0] * b[0][0] +  a[2][1] * b[1][0] +  a[2][2] * b[2][0] +  0.0f * b[3][0],
			a[2][0] * b[0][1] +  a[2][1] * b[1][1] +  a[2][2] * b[2][1] +  0.0f * b[3][1],
			a[2][0] * b[0][2] +  a[2][1] * b[1][2] +  a[2][2] * b[2][2] +  0.0f * b[3][2],

			a[3][0] * b[0][0] +  a[3][1] * b[1][0] +  a[3][2] * b[2][0] +  1.0f * b[3][0],
			a[3][0] * b[0][1] +  a[3][1] * b[1][1] +  a[3][2] * b[2][1] +  1.0f * b[3][1],
			a[3][0] * b[0][2] +  a[3][1] * b[1][2] +  a[3][2] * b[2][2] +  1.0f * b[3][2]
		);
	}

	//-----------------------------------------------------------------------------
	mat43 operator*=( const mat43 a ) 
	//-----------------------------------------------------------------------------
	{

		*this = *this * a;
		
		return *this;
	}

	//-----------------------------------------------------------------------------
	mat43 invers()
	//-----------------------------------------------------------------------------
	{

		mat44 A(
			m[0][0],	m[0][1],	m[0][2],	0.0,
			m[1][0],	m[1][1],	m[1][2],	0.0,
			m[2][0],	m[2][1],	m[2][2],	0.0,
			pos[0],		pos[1],		pos[2],		1.0
		);
		mat44 I(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
		
		int z1=4;  //配列の次数

		//掃き出し法
		for( int i = 0 ; i < z1 ; i++ )
		{
			float f =1/A.m[i][i];
			for( int j = 0 ; j < z1 ; j++ )
			{
				A.m[i][j] *= f;
				I.m[i][j] *= f;
			}
			for( int j = 0 ; j < z1 ; j++ )
			{
				if( i !=j )
				{
					f= A.m[j][i];
					for( int k = 0 ; k < z1 ; k++ )
					{
						A.m[j][k] -= A.m[i][k]*f;
						I.m[j][k] -= I.m[i][k]*f;
					}
				}
			}
		}
		return mat43
		(
			I.m[0][0],	I.m[0][1],	I.m[0][2],
			I.m[1][0],	I.m[1][1],	I.m[1][2],
			I.m[2][0],	I.m[2][1],	I.m[2][2],
			I.m[3][0],	I.m[3][1],	I.m[3][2]
		);
		
	}

	void dump()
	{
		printf("mat43:\n");
		printf("%9.6f %9.6f %9.6f \n", m[0][0], m[0][1], m[0][2] );
		printf("%9.6f %9.6f %9.6f \n", m[1][0], m[1][1], m[1][2] );
		printf("%9.6f %9.6f %9.6f \n", m[2][0], m[2][1], m[2][2] );
		printf("%9.6f %9.6f %9.6f \n", pos[0], pos[1], pos[2] );
	}
	void dump( const char* str )
	{
		printf("mat43:%s\n", str);
		printf("%9.6f %9.6f %9.6f \n", m[0][0], m[0][1], m[0][2] );
		printf("%9.6f %9.6f %9.6f \n", m[1][0], m[1][1], m[1][2] );
		printf("%9.6f %9.6f %9.6f \n", m[2][0], m[2][1], m[2][2] );
		printf("%9.6f %9.6f %9.6f \n", pos[0], pos[1], pos[2] );
	}

};



void	mat4_Frustum( mat44 m, float l, float r, float b, float t, float n, float f );
void	mat4_Ortho( float* m, float l, float r, float b, float t, float n, float f );

int	mat4_print( mat44 m );
int	mat4_print( float* m );
void	mat4_frustum( float* m, float l, float r, float b, float t, float n, float f );
void	mat4_translate( mat44 m, float x, float y, float z );
void	mat4_translate( float* m, float x, float y, float z );
void	mat4_translate( float* m, vect3 vec );
void	mat4_translate( mat44 m, vect3 vec );
void	mat4_rotateX( mat44 m, float th );
void	mat4_rotateY( mat44 m, float th );
void	mat4_rotateZ( mat44 m, float th );
void	mat4_scaling( mat44 m, float sx, float sy, float sz  );
void	mat4_scaling( float* m, float sx, float sy, float sz  );
void	mat4_scaling( float* m, vect3 vecScale  );
void	mat4_scaling( mat44 m, vect3 vecScale  );
void	mat4_transpose( float* m, float a[16] );

void	mat4_copy( float* m, float a[16] );

void 	mat4_invers( mat44 a, const mat44 b );
//void	mat4_invers ( float* out, float* in );
void	mat4_identity( float* m );
void	mat4_identity( mat44 m );

void	mat4_getTranslate( float* m, float x, float y, float z );
void	mat4_getRotateX( float* m, float th );
void	mat4_getRotateY( float* m, float th );
void	mat4_getRotateZ( float* m, float th );
void	mat4_getScale( float* m, float sx, float sy, float sz  );


float*	mat4_identity();

void 	mat4_glFrustumf ( float* m,  float left, float right, float bottom, float top, float near, float far);
void 	mat4_perspective ( mat44 m, float fovy, float aspect, float zNear, float zFar);
void 	mat4_perspective (float* m, float fovy, float aspect, float zNear, float zFar);
void 	mat4_invers ( float* m, const float* in );
void	mat4_add( float* m, float* a, float* b );
void	mat4_sub( float* m, float* a, float* b );
void	mat4_div( float* m, float f );

// for raytrace
void mat4_ray_perspective (float* m, float fovy, float aspect );
void	mat4_otrho( float* m, float l, float r, float b, float t, float n, float f );

mat44	mat4_GetTranslate( float x, float y, float z );
mat44	mat4_GetRotateX( float th );
mat44	mat4_GetRotateY( float th );
mat44	mat4_GetRotateZ( float th );
void	mat4_multiply( float* m, const float* a, const float* y1 );
#endif
