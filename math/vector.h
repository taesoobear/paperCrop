#ifndef _VECTOR3_H_
#define _VECTOR3_H_
#if _MSC_VER > 1000
#pragma once
#endif
#include "../utility/TypeString.h"
class quater;
class matrix4;
class vectorn;
class matrixn;
/// never make virtual functions.
/** 이유: 이것의 주소는 direct하게 D3DXVECTOR3의 주소로 바뀔수 있다. virtual function을 만들면 VTable때문에 이것이 안된다.
	\ingroup group_math
*/

#if _MSC_VER > 1000
//#pragma message("Compiling vector.h - this should happen just once per project.\n")
#endif

class vector3
{
	public:

	m_real x, y, z;

    // constructors
    vector3() {}
	vector3( m_real v)				{ x=v; y=v; z=v;}
	vector3( m_real xx, m_real yy, m_real zz )				{ x=xx; y=yy; z=zz;}
    //vector3( m_real a[3] )								{ x=a[0]; y=a[1]; z=a[2]; }

	// binary operations
	// this가 output이다.
	void add(const vector3&, const vector3&);
	void sub(const vector3&, const vector3&);
	void multadd(const vector3&, m_real);	//!< c.multadd(b,w) -> c+=b*w;
	void mult(const vector3& ,m_real);
	void mult(const matrix4& mat, const vector3& v);
	void divide(const vector3& v, m_real a)	{ mult(v, 1/a);};
	void cross(const vector3&, const vector3&);
	vector3 cross(const vector3& other) const	{ vector3 c; c.cross(*this, other); return c;}
	vector3 mult(const vector3& o) const		{ return vector3(x*o.x, y*o.y, z*o.z);}
	void normalize( const vector3 & );
	vector3 dir()	{ vector3 temp; temp.normalize(*this); return temp; }
	void negate(const vector3&);
	void interpolate( m_real, vector3 const&, vector3 const& );
	void lerp( vector3 const& a, vector3 const& b, m_real t)	{ interpolate(t, a, b);}
	void blend(const vectorn& weight, const matrixn& aInputVector);
	//! quaternion ln
	/*!	A unit quaternion, is defined by:
	Q == (cos(theta), sin(theta) * v) where |v| = 1
	The natural logarithm of Q is, ln(Q) = (0, theta * v)*/

	void ln( const quater& q);
	void log( const quater& q)		{ ln(q);}

	// to avoid unnecessary copy, use q.exp(v) instead.
	quater exp() const;
	void rotate( const quater& q);
	void rotate( const quater& q, vector3 const& in);
	void angularVelocity( quater const& q1, quater const& q2);
	void linearVelocity(vector3 const& v1, vector3 const& v2);
	inline void difference(vector3 const& v1, vector3 const& v2)	{ linearVelocity(v1, v2);}

    m_real    operator%( vector3 const&) const;
    m_real    operator/( vector3 const&) const;
    vector3    operator+( vector3 const& ) const;
    vector3    operator-( vector3 const& ) const;

    vector3    operator*( m_real ) const;
    vector3    operator/( m_real ) const;
	friend vector3    operator*( m_real, vector3 const& ) ;

	// element multiplication (예전 버전에서는 crossproduct 로 정의되어 있었음. 주의할 것).
	vector3    operator*( vector3 const& ) const;
	void operator*=(vector3 const& );
	bool operator==(vector3 const& )const;

	// unary operations
	void add(m_real value);
	void add(const vector3& other);
	void sub(m_real value);
	void sub(const vector3& other);
	void column(int col, const matrix4& other);	//!< extract a column from the rotation part in a matrix
	void row(int col, const matrix4& other);		//!< extract a row from the rotation part in a matrix
	void translation(const matrix4& other);		//!< extract translation vector from a matrix
	void imaginaries(const quater& in);			//!< extract imagenaries from a quaternion
	m_real distance(const vector3& other) const;
	m_real squaredDistance(const vector3& other) const;
	void leftMult( const matrix4& mat);		//!< 당연히 this=mat*this; 로 정의된다.
	void rotationVector(const quater& in);
	quater quaternion() const;	//!< rotation vector를 quaternion으로 바꾼다.	== quater q; q.setRotation(*this); }

	void operator+=( vector3 const& );
    void operator-=( vector3 const& );
    void operator*=( m_real );
    void operator/=( m_real );
	vector3& operator=(vector3 const& );
	void hermite(const vector3& p1, const vector3& t1, const vector3& p2, const vector3& t2, m_real t);	//!< hermite interpolation of p1 and p2. 0<=t<=1
	vector3  operator-() const	{ vector3 c; c.negate(*this); return c;};

	inline void makeFloor( const vector3& cmp )
    {
        if( cmp.x < x ) x = cmp.x;
        if( cmp.y < y ) y = cmp.y;
        if( cmp.z < z ) z = cmp.z;
    }

    inline void makeCeil( const vector3& cmp )
    {
        if( cmp.x > x ) x = cmp.x;
        if( cmp.y > y ) y = cmp.y;
        if( cmp.z > z ) z = cmp.z;
    }

	// functions
	void normalize();
	void zero();
	m_real length() const;
	// calc 0 to pi angle
    m_real angle( vector3 const& ) const;
	m_real cosTheta(vector3 const&) const;
	m_real sinTheta(vector3 const& b) const;
	// calc 0 to 2pi angle assuming z=0 (plane constraint)
	m_real angle2d(vector3 const& b) const;
	// calc -pi to pi angle assuming z=0 (plane constraint)
	m_real angle2ds(vector3 const& b) const;

	// calc -pi to pi angle assuming axis=0 (plane constraint): axis 0:x, 1: y, 2:z
	m_real angle2ds(vector3 const& b, int axis) const;

    m_real& operator[] (int i)						{ return (&x)[i];}
	const m_real& operator[] (int i) const			{ return (&x)[i];}
	m_real  getValue( int i ) const					{ return (&x)[i];}
    void   getValue( m_real d[3] )					{ d[0]=x; d[1]=y; d[2]=z;}

    void   setValue( m_real d[3] )					{ x=d[0]; y=d[1]; z=d[2]; }
	void   setValue(m_real xx, m_real yy, m_real zz )	{ x=xx; y=yy; z=zz;}
	void   setValue(const vectorn& other, int start=0);

	TString output() const;

	inline operator const m_real*() const		{ return &x;}
	inline operator m_real*()					{ return &x;}


};

#endif
