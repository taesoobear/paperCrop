#pragma once

class vector3;
class quater;
class matrix3;
class transf;
/// row-major matrix4 class following opengl convention 
// (positions are multiplied at right-side of the matrix).
/** 
important: 
never make virtual functions. 
quaternion은 q1*q2*v형태로 정의상 오른쪽께 먼저 점에 곱해진다고 생각해야 한다.
openGL matrix는 quaternion과 마찮가지로 R1*r2*v형태로 오른쪽에 점이 곱해진다.
하지만 D3DXMATRIX는 v*R1*R2형태로 점이 왼쪽에 곱해지고, 마찬가지로 D3DXQUATERNION은 q1*q2가 원래 정의의 q2*q1과 일치하도록 
곱하기순서가 바뀌어 있다. 즉 D3DXMATRIX와 D3DXQUATERNION모두 왼쪽에 있는 것이 로컬, 가장 오른쪽에 있는 것이 글로벌이다.
양쪽의 convention을 맞추기 위해서는 transpose가 필요하다.
*/

#ifdef minor
#undef minor
#endif
class matrix4
{
public:

	union {
        struct {
            m_real        _11, _12, _13, _14;
            m_real        _21, _22, _23, _24;
            m_real        _31, _32, _33, _34;
            m_real        _41, _42, _43, _44;

        };
        m_real m[4][4];
    };

	matrix4(const matrix4& mat)					{ memcpy(this, &mat, sizeof(matrix4));}
	matrix4(const quater& rot, const vector3& trans)	{ setRotation(rot); setTranslation(trans);}
	matrix4(const transf& transf);
	matrix4();
	~matrix4();

	inline void identity()	{ setIdentityRot(); }

	// n-ary operators
	void lookAtLH(const vector3& eye, const vector3& at, const vector3& up);
	void lookAtRH(const vector3& eye, const vector3& at, const vector3& up);
	void setScaling(m_real sx, m_real sy, m_real sz);
	
	void setTransform(const quater& rot, const vector3& trans);
	void setTransform(const vector3& position, const vector3& scale, const quater& orientation);

	// all setRotation fuctions remove translation terms unless manually specified otherwise (bPreserveCurrentTranslation=true).
	void setRotation(const quater& q, bool bPreserveCurrentTranslation=false);
	void setRotation(const matrix3& m, bool bPreserveCurrentTranslation=false);
	void setRotation(const vector3& axis, m_real angle, bool bPreserveCurrentTranslation=false);
	void setRotationX(m_real angle);
	void setRotationY(m_real angle);
	void setRotationZ(m_real angle);
	void setRotation(const char* aChannel, m_real *aValue, bool bRightToLeft=false);	//!< from euler angle. aChannel="YXZ" or something like that.
	void setAxisRotation(const vector3& vecAxis, const vector3& front, const vector3& vecTarget); 	//!< front벡터를 vecAxis를 중심으로 회전해서 vecTarget과 vecAxis가 이루는 평면에 놓이도록 만드는 Matrix를 구한다.
	void setIdentityRot();	

	// setTranslation preserves rotation parts unless manually specified otherwise (bPreserveCurrentRotation=false)
	void setTranslation(const vector3& tx, bool bPreserveCurrentRotation=true);		//!< 다른 set계열 함수와 달리 rotation파트는 건드리지 않는다. 	

	void transpose();
	
	
	void mult(const matrix4& a, const matrix4& b);
	void mult(const matrix4& a, const quater& b);
	void mult(const quater& a, const matrix4& b);

	// unary operators
	void adjoint(const matrix4& a);
	void inverse(const matrix4& a);
	void extractRot(const matrix4& a);
	void transpose(const matrix4& a);
	void leftMult(const matrix4& a);	//!< this=a*this;
	void operator*=(const matrix4& a);	//!< this=this*a;
	void rightMult(const matrix4& a) { (*this)*=a;}
	void leftMult(m_real scalar);		//!< a=matScale*a
	void leftMultScaling(m_real sx, m_real sy, m_real sz);
	void leftMultRotation(const quater& b);
	void leftMultRotation(const vector3& axis, m_real angle);
	void leftMultTranslation(const vector3& vec);

	matrix4 operator*(matrix4 const& a) const { matrix4 t; t.mult(*this,a); return t;}
	vector3 operator*(vector3 const& a) const;
	
	// inquiry functions
	void decomposeLH(vector3& eye, vector3& at, vector3& up);
	void decomposeRH(vector3& eye, vector3& at, vector3& up);

	void setValue( m_real x00, m_real x01, m_real x02,
				   m_real x10, m_real x11, m_real x12,		// remaining values are set as 0 or 1.
				   m_real x20, m_real x21, m_real x22 )	;

	void setValue( m_real x00, m_real x01, m_real x02, m_real x03,
				   m_real x10, m_real x11, m_real x12, m_real x13,
				   m_real x20, m_real x21, m_real x22, m_real x23,
				   m_real x30, m_real x31, m_real x32, m_real x33)	;

	m_real determinant() const;
	m_real minor(const size_t r0, const size_t r1, const size_t r2, 
				const size_t c0, const size_t c1, const size_t c2) const;

private:
	void _discardTranslation();

};

