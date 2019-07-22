// version 20170527
//	2019/06/25 to only vect3 & mat44
#include <cmath>
#ifndef GEOM_H
#define	GEOM_H
typedef	double	MAT4[4][4];

void	mat4_rotateX( double* m, double th );
void	mat4_rotateY( double* m, double th );
void	mat4_rotateZ( double* m, double th );


//#define	pi	3.141592654
//const static double pi = 3.141592654;
const static double pi = 3.1415926535897932384626433832795;
struct	I2
{
	union { int x; int n0;	int p; };
	union { int y; int n1;	int n; };
	I2(){};
	I2( int _x, int _y ) : x(_x), y(_y){}
};

struct	I3
{
	union { int x; int n0;	int r; };
	union { int y; int n1;	int g; };
	union { int z; int n2;	int b; };
	I3(){};
	I3( int _x, int _y, int _z ) : x(_x), y(_y), z(_z){}
};

struct	I4
{
	union { int x; int n0;	int r; };
	union { int y; int n1;	int g; };
	union { int z; int n2;	int b; };
	union { int w; int n3;	int a; };
	I4(){};
	I4( int _x, int _y, int _z , int _w ) : x(_x), y(_y), z(_z), w(_w){}
};

struct	D2
{
	union { double x;	double u; };
	union { double y;	double v; };
	D2( double _x, double _y ) : x(_x), y(_y){}
};

struct	D3
{
	union { double x;	double r; };
	union { double y;	double g; };
	union { double z;	double b; };
	D3(){};
	D3( D2 v, double _z ) : x(v.x), y(v.y), z(_z){}
	D3( double _x, double _y, double _z ) : x(_x), y(_y), z(_z){}
};

struct	D4
{
	union { double x;	double u;	double r; };
	union { double y;	double v;	double g; };
	union { double z;	double s;	double b; };
	union { double w;	double t;	double a; };
	D4(){};
	D4( double _x, double _y, double _z, double _w ) : x(_x), y(_y), z(_z), w(_w){}
	D4( D2 v, double _z, double _w ) : x(v.x), y(v.y), z(_z), w(_w){}
	D4( D3 v, double _w ) : x(v.x), y(v.y), z(v.z), w(_w){}
};

struct	ivect2 : public I2
{
//	int	p,n;
	ivect2( int _p, int _n ) 
	{
		x=_p;
		y=_n;
	}
	
};

class vect2
{
public:
	double x,y;

	vect2() :x(0),y(0){}
	vect2( double f ) :x(f),y(f){}
	vect2( double _x, double _y) :x(_x),y(_y){}

	double length() { return sqrt( x*x + y*y ); }

	void operator= ( vect2 v )  { x =  v.x; y =  v.y; }
	void operator*=( vect2 v )  { x *= v.x; y *= v.y; }
	void operator/=( vect2 v )  { x =  v.x; y =  v.y; }
	void operator+=( vect2 v )  { x += v.x; y += v.y; }
	void operator-=( vect2 v )  { x -= v.x; y -= v.y; }

	vect2 operator*( vect2 v ) const { return vect2( x*v.x, y*v.y ); }
	vect2 operator/( vect2 v ) const { return vect2( x/v.x, y/v.y ); }
	vect2 operator+( vect2 v ) const { return vect2( x+v.x, y+v.y ); }
	vect2 operator-( vect2 v ) const { return vect2( x-v.x, y-v.y ); }

	vect2 operator-() const { return vect2( -x, -y ); } 
	vect2 operator+() const { return vect2(  x,  y ); } 

	void operator= ( double f ) { x =  f  ; y =  f  ; }
	void operator*=( double f ) { x *= f  ; y *= f  ; }
	void operator/=( double f ) { x /= f  ; y /= f  ; }
	void operator+=( double f ) { x += f  ; y += f  ; }
	void operator-=( double f ) { x -= f  ; y -= f  ; }

	vect2 operator*( double f ) const { return vect2( x*f, y*f ); } 
	vect2 operator/( double f ) const { return vect2( x/f, y/f ); } 
	vect2 operator+( double f ) const { return vect2( x+f, y+f ); } 
	vect2 operator=( double f ) const { return vect2( x-f, y-f ); } 

	friend	vect2 operator*( double f, vect2 v )  { return vect2( f * v.x, f * v.y ); }
	friend	vect2 operator/( double f, vect2 v )  { return vect2( f / v.x, f / v.y ); }
	friend	vect2 operator+( double f, vect2 v )  { return vect2( f + v.x, f + v.y ); }
	friend	vect2 operator-( double f, vect2 v )  { return vect2( f - v.x, f - v.y ); }

};

class vect3
{
public:
	union { double	x;	double	r;	 };
	union { double	y;	double	g;	 }; 
	union {	double	z;	double  b;   };

	vect3(){};
	vect3( double a ) { x = a; y = a; z = a; };
	vect3( double _x, double _y, double _z ) { x = _x; y = _y; z = _z; };

	vect3 operator*=( vect3 v ) { x *= v.x; y *= v.y; z *= v.z;  return *this; }
	vect3 operator/=( vect3 v ) { x /= v.x; y /= v.y; z /= v.z;  return *this; }
	vect3 operator+=( vect3 v ) { x += v.x; y += v.y; z += v.z;  return *this; }
	vect3 operator-=( vect3 v ) { x -= v.x; y -= v.y; z -= v.z;  return *this; }

	vect3 operator*( vect3 v ) const { vect3	ret; ret.x = x * v.x; ret.y = y * v.y; ret.z = z * v.z; return ret; }
	vect3 operator/( vect3 v ) const { vect3	ret; ret.x = x / v.x; ret.y = y / v.y; ret.z = z / v.z; return ret; }
	vect3 operator-( vect3 v ) const { vect3	ret; ret.x = x - v.x; ret.y = y - v.y; ret.z = z - v.z; return ret; }
	vect3 operator+( vect3 v ) const { vect3	ret; ret.x = x + v.x; ret.y = y + v.y; ret.z = z + v.z; return ret; }

	vect3 operator-() const { vect3	ret; ret.x = -x; ret.y = -y; ret.z = -z; return ret; }
	vect3 operator+() const { vect3	ret; ret.x =  x; ret.y =  y; ret.z =  z; return ret; }

	vect3 operator*( double f ) const { vect3	ret; ret.x = x * f; ret.y = y * f; ret.z = z * f; return ret; }
	vect3 operator/( double f ) const { double a = 1.0f / f; vect3	ret; ret.x = x * a; ret.y = y * a; ret.z = z * a; return ret; }

	friend	vect3 operator*( double f, vect3 v )  { vect3	ret; ret.x = f * v.x; ret.y = f * v.y; ret.z = f * v.z;  return ret; }
	friend	vect3 operator+( double f, vect3 v )  { vect3	ret; ret.x = f + v.x; ret.y = f + v.y; ret.z = f + v.z;  return ret; }


	void Print()
	{
		printf("(vect3)%9.6f %9.6f %9.6f\n", x, y, z );
	}

	void Print( const char* str )
	{
		printf("%s<%9.6f %9.6f %9.6f>\n", str, x, y, z );
	}

};



extern double	vect3_dotproduct( double a[3], double b[3] );
extern void		vect3_crossproduct( double v[3], double a[3], double b[3] );
extern vect3	       cross( vect3 a, vect3 b );
extern void		vect3_normalize( double v[3] );
extern void		vect3_normalize( vect3* v );
extern double	vect3_length( double v[3] );
extern void		vec4_multiplyMatrix( double v[4], double m[16], double a[4] );
extern double	dot( vect3 a, vect3 b );

extern vect3 refract( vect3 a, vect3 b, double n );
extern vect3 reflect( vect3 I, vect3 N );


extern vect2		min( vect2 a, vect2 b );
extern vect2		max( vect2 a, vect2 b );
extern vect3		min( vect3 a, vect3 b );
extern vect3		max( vect3 a, vect3 b );

extern double	max( double a, double b );
extern vect3		max( const double a, vect3 b );
extern vect3		max( vect3 b, double a );
extern double	min( double a, double b );
extern vect3		min( double a, vect3 b );
extern vect3		min( vect3 b, double a );
///
extern vect3	mix( double f, vect3 a, vect3 b );
extern double	mod( double a, double b );
extern double	length( vect3 a );
extern double	length( vect3 a, vect3 b );
extern vect3	normalizXe( vect3 a );
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
extern	double rad( double n );
extern	double deg( double n );


//---
class	mat44
{
public:
	union
	{
		double	m_array[16];
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
		double m00, double m01, double m02, double m03, 
		double m10, double m11, double m12, double m13, 
		double m20, double m21, double m22, double m23, 
		double m30, double m31, double m32, double m33 
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

	double* GetArray(){ return m_array; };

	void identity()
	{
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;	m[1][3] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}

	void setRotateX( double rx )
	{
		double c=cos(rx);
		double s=sin(rx);
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = c;	m[1][2] = -s;	m[1][3] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = s;	m[2][2] = c;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}
	void setRotateY( double ry )
	{
		double c=cos(ry);
		double s=sin(ry);
		m[0][0] = c;	m[0][1] = 0.0f;	m[0][2] = s;	m[0][3] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0;	m[1][3] = 0.0f;
		m[2][0] = -s;	m[2][1] = 0.0;	m[2][2] = c;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}
	void setRotateZ( double rz )
	{
		double c=cos(rz);
		double s=sin(rz);
		m[0][0] = c;	m[0][1] = -s;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
		m[1][0] = s;	m[1][1] =  c;	m[1][2] = 0.0f;	m[1][3] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}
	
	void SetTranslate( vect3 pos )
	{
		m[3][0] = pos.x;
		m[3][1] = pos.y;
		m[3][2] = pos.z;
	}
	void SetTranslate( double x, double y, double z )
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
		vect3	z = normalizXe(at-pos);
		vect3	x = cross( up, z );
		vect3	y = cross( z, x );
	
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
	void RotateX( double f)
	{
		double	c = cos(f);
		double	s = sin(f);
		*this *= mat44(
			1,  0,  0,  0,
			0,  c, -s,  0,
			0,  s,  c,  0,
			0,  0,  0,  1
		);
	}
	void RotateY( double f)
	{
		double	c = cos(f);
		double	s = sin(f);
		*this *= mat44(
			c,  0, -s,  0,
			0,  1,  0,  0,
			s,  0,  c,  0,
			0,  0,  0,  1
		);
	}
	void RotateZ( double f)
	{
		double	c = cos(f);
		double	s = sin(f);
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
	void Tlanslate( double x, double y, double z )
	{
		m[3][0] += x;
		m[3][1] += y;
		m[3][2] += z;
	}
	
	void Scale( double x, double y, double z )
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

	void Print()
	{
		printf("mat44:\n");
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[0][0], m[0][1], m[0][2], m[0][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[1][0], m[1][1], m[1][2], m[1][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[2][0], m[2][1], m[2][2], m[2][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[3][0], m[3][1], m[3][2], m[3][3] );
	}
	void Print( const char* str )
	{
		printf("mat44:%s\n", str);
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[0][0], m[0][1], m[0][2], m[0][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[1][0], m[1][1], m[1][2], m[1][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[2][0], m[2][1], m[2][2], m[2][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[3][0], m[3][1], m[3][2], m[3][3] );
	}

};


void	mat4_Frustum( mat44 m, double l, double r, double b, double t, double n, double f );
void	mat4_Ortho( double* m, double l, double r, double b, double t, double n, double f );

int	mat4_print( mat44 m );
int	mat4_print( double* m );
void	mat4_frustum( double* m, double l, double r, double b, double t, double n, double f );
void	mat4_translate( mat44 m, double x, double y, double z );
void	mat4_translate( double* m, double x, double y, double z );
void	mat4_translate( double* m, vect3 vec );
void	mat4_translate( mat44 m, vect3 vec );
void	mat4_rotateX( mat44 m, double th );
void	mat4_rotateY( mat44 m, double th );
void	mat4_rotateZ( mat44 m, double th );
void	mat4_scaling( mat44 m, double sx, double sy, double sz  );
void	mat4_scaling( double* m, double sx, double sy, double sz  );
void	mat4_scaling( double* m, vect3 vecScale  );
void	mat4_scaling( mat44 m, vect3 vecScale  );
void	mat4_transpose( double* m, double a[16] );

void	mat4_copy( double* m, double a[16] );

void 	mat4_invers( mat44 a, const mat44 b );
//void	mat4_invers ( double* out, double* in );
void	mat4_identity( double* m );
void	mat4_identity( mat44 m );

void	mat4_getTranslate( double* m, double x, double y, double z );
void	mat4_getRotateX( double* m, double th );
void	mat4_getRotateY( double* m, double th );
void	mat4_getRotateZ( double* m, double th );
void	mat4_getScale( double* m, double sx, double sy, double sz  );


double*	mat4_identity();

void 	mat4_glFrustumf ( double* m,  double left, double right, double bottom, double top, double near, double far);
void 	mat4_perspective ( mat44 m, double fovy, double aspect, double zNear, double zFar);
void 	mat4_perspective (double* m, double fovy, double aspect, double zNear, double zFar);
void 	mat4_invers ( double* m, const double* in );
void	mat4_add( double* m, double* a, double* b );
void	mat4_sub( double* m, double* a, double* b );
void	mat4_div( double* m, double f );

// for raytrace
void mat4_ray_perspective (double* m, double fovy, double aspect );
void	mat4_otrho( double* m, double l, double r, double b, double t, double n, double f );

mat44	mat4_GetTranslate( double x, double y, double z );
mat44	mat4_GetRotateX( double th );
mat44	mat4_GetRotateY( double th );
mat44	mat4_GetRotateZ( double th );
void	mat4_multiply( double* m, const double* a, const double* y1 );
#endif
