#ifndef _VECTOR_N_H_
#define _VECTOR_N_H_
#pragma once
class matrixn;
class bitvectorn;
#include "vector.h"
#include "quater.h"
#include "../utility/TypeString.h"
//#include "../utility/util.h"

#include <typeinfo>
class matrixnView;
class matrixn;
//class cmplxvectorn ;
class intvectorn;
class intvectornView;
class vectorn;
class vectornView;
#include "template_math.h"

#if _MSC_VER > 1000
//#pragma message("Compiling vector_n.h - this should happen just once per project.\n")
#endif

class intvectorn : public _tvectorn<int>
{
protected:
	intvectorn(int* ptrr, int size, int stride):_tvectorn<int>(ptrr,size,stride){}	// reference
public:
	intvectorn():_tvectorn<int>(){}

	explicit intvectorn(int n):_tvectorn<int>()				{setSize(n);}
	explicit intvectorn( int, int x, ...);	// n dimensional vector	(ex) : vectorn(3, 1, 2, 3);
	// copy constructor : 항상 카피한다.
	intvectorn(const _tvectorn<int>& other):_tvectorn<int>()		{ assign(other);}
	intvectorn(const intvectorn& other):_tvectorn<int>()			{ assign(other);}
	intvectorn(const intvectornView& other);

	// 값을 copy한다.
	intvectorn& operator=(const _tvectorn<int>& other)		{ _tvectorn<int>::assign(other);return *this;}
	intvectorn& operator=(const intvectorn& other)			{ assign(other);return *this;}
	intvectorn& operator=(const intvectornView& other);

	friend bool operator==(intvectorn const& a, intvectorn const& b)
	{
		if(a.size()!=b.size()) return false;
		for(int i=0; i<a.size(); i++)
			if(a[i]!=b[i]) return false;
		return true;
	}

	// L-value로 사용될수 있는, reference array를 만들어 return 한다.
	// ex) v.range(0,2).setValues(2, 1.0, 2.0);
	intvectornView	range(int start, int end, int step=1);
	const intvectornView	range(int start, int end, int step=1) const;

	intvectorn& assignBits(const bitvectorn& bits);

	~intvectorn(){}

	int count(int (*s2_func)(int ,int ), int value, int start=0, int end=INT_MAX);
	intvectorn&  colon(int start, int end, int stepSize=1);
	intvectorn&  sortedOrder(vectorn const & input);
	intvectorn&  makeSamplingIndex(int nLen, int numSample);
	// 첫프레임과 마지막 프레임은 반드시 포함하고 나머지는 그 사이에서 uniform sampling
	intvectorn&  makeSamplingIndex2(int nLen, int numSample);

	intvectorn& findIndex(intvectorn const& source, int value);
	intvectorn& findIndex(bitvectorn const& source, bool value, int start=0, int end=INT_MAX);
	// return -1 if not found.
	int findFirstIndex(int value) const;

	intvectorn&  setAt( intvectorn const& columnIndex, _tvectorn<int> const& value);
	intvectorn&  setAt( intvectorn const& columnIndex, int value);

	void decode(const TString& input);

	int aggregate(CAggregate::aggregateOP eOP) const;
	int maximum() const;
	int minimum() const;
	int sum() const;

	vectorn toVectorn();

	TString output(const char* left="[", const char* typeString="%d", const char* seperator=",", const char* right="]");

	// all following functions are deprecated.
	inline void push_back(int x)						{ pushBack(x);}

	// use the corresponding functions in class intIntervals instead.
	void runLengthEncode(const intvectorn& source);
	void runLengthEncode(const bitvectorn& source, int start=0, int end=INT_MAX);
	void runLengthDecode(bitvectorn& out, int size);
	void runLengthEncodeCut(const bitvectorn& cutState, int start=0, int end=INT_MAX);

};

// reference로 받아온다.
class intvectornView :public intvectorn
{
public:
	// L-value로 사용될수 있는, reference array로 만든다.
	intvectornView (int* ptrr, int size, int stride);
	// copy constructor : get reference.
	intvectornView(const _tvectorn<int>& other)				{ assignRef(other);}
	intvectornView(const intvectorn& other);//				{ assignRef(other);}
	intvectornView(const intvectornView& other)				{ assignRef(other);}
	~intvectornView (){}

	// L-value로 사용되는 경우, 값을 copy한다.
	intvectorn& operator=(const _tvectorn<int>& other)		{ _tvectorn<int>::assign(other);return *this;}
	intvectorn& operator=(const intvectorn& other);//		{ assign(other);return *this;}
	intvectorn& operator=(const intvectornView& other)		{ assign(other);return *this;}

};

namespace v0
{
	// v0 namespace에는 doSomething(c); 형태의 class나 function이 정의된다. (operatorTemplate)
	struct abstractClass 	// 상속할 필요 없음.
	{
		void operator()(vectorn& c){}
	};

	// deprecated
	struct _op
	{
		virtual void calc(vectorn& c) const {ASSERT(0);}
	};
}

namespace v1
{
	// v1 namespace에는 doSomething(c,a); 형태의 class나 function이 정의된다. (operatorTemplate)
	struct abstractClass 	// 상속할 필요 없음.
	{
		void operator()(vectorn& c, const vectorn& a){}
	};

	// deprecated
	struct _op
	{
		virtual void calc(vectorn& c, const vectorn& a) const {ASSERT(0);}
	};
}

namespace v2
{
	// v2 namespace에는 doSomething(c,a,b); 형태의 class나 function이 정의된다. (operatorTemplate)
	struct abstractClass	// 상속할 필요 없음.
	{
		void operator()(vectorn& c, const vectorn& a, const vectorn& b){}
	};
	// deprecated
	struct _op
	{
		virtual void calc(vectorn& c, const vectorn& a, const vectorn& b) const {ASSERT(0);}
	};
}

// deprecated
namespace sv2
{
	struct _op
	{
		virtual m_real calc(const vectorn& a, const vectorn& b) const {ASSERT(0);return 0.0;}
	};
}


class vectorn : public _tvectorn<m_real>
{
protected:
	vectorn(m_real* ptrr, int size, int stride):_tvectorn<m_real>(ptrr,size,stride){}
public:
	vectorn();
	vectorn(const vector3& other);
	vectorn(const quater& other);

	// 값을 카피해서 받아온다.
	vectorn(const _tvectorn<m_real>& other);
	vectorn(const vectorn& other);
	vectorn(const vectornView& other);

	explicit vectorn( int x):_tvectorn<m_real>() { setSize(x);}

	// n dimensional vector	(ex) : vectorn(3, 1.0, 2.0, 3.0);
	explicit vectorn( int n, m_real x);
	explicit vectorn( int n, m_real x, m_real y);
	explicit vectorn( int n, m_real x, m_real y, m_real z);
	explicit vectorn( int n, m_real x, m_real y, m_real z, m_real w, ...);	// n dimensional vector	(ex) : vectorn(3, 1.0, 2.0, 3.0);

	~vectorn(){}

	matrixnView column() const;	// return n by 1 matrix, which can be used as L-value (reference matrix)
	matrixnView row() const;	// return 1 by n matrix, which can be used as L-value (reference matrix)
	matrixnView matView(int nrow, int ncol);

	// L-value로 사용될수 있는, reference array를 만들어 return 한다.
	// ex) v.range(0,2).setValues(2, 1.0, 2.0);
	vectornView range(int start, int end, int step=1);
	const vectornView range(int start, int end, int step=1) const	;

	//////////////////////////////////////////////////////////////////////
	// binary operations.
	//////////////////////////////////////////////////////////////////////
	vectorn&  normalize(vectorn const& min, vectorn const& max);				//!< scale data so that min->0, max->1

	// slow binary operations, I recommend you to use _tvectorn<T>::add function, instead.
	friend vectorn operator+( vectorn const& a, vectorn const& b);
	friend vectorn operator-( vectorn const& a, vectorn const& b);
	friend vectorn operator*( vectorn const& a, vectorn const& b );
	friend vectorn operator+( vectorn const& a, m_real b);
	friend vectorn operator-( vectorn const& a, m_real b);
	friend vectorn operator*( vectorn const& a, m_real b);
	friend vectorn operator/( vectorn const& a, m_real b);
	friend vectorn operator*( matrixn const& a, vectorn const& b );


	//////////////////////////////////////////////////////////////////////
	// binary scalar operations
	//////////////////////////////////////////////////////////////////////
	m_real angle(vectorn const& other) const;			// calc angle between 0 to pi
	m_real angle2D(vectorn const& b) const;			// calc angle between 0 to 2pi
	m_real distance(vectorn const& other) const;
	m_real cosTheta(vectorn const& other) const;			// cosTheta for every dimension
	m_real sinTheta(vectorn const& b) const;				// sinTheta for 2 dimension
	bool   isSimilar(vectorn const& other,m_real thr=0.00001) const	{ if(distance(other)<=thr) return true; return false;}

	friend m_real operator%( vectorn const&, vectorn const& );		// dot product
	friend bool operator<(vectorn const& a, vectorn const& b);
	friend bool operator>(vectorn const& a, vectorn const& b);
	friend bool operator<=(vectorn const& a, vectorn const& b) { return !(a>b);};
	friend bool operator>=(vectorn const& a, vectorn const& b) { return !(a<b);};
	friend bool operator==(vectorn const& a, vectorn const& b) { return a.isSimilar(b);};

	//////////////////////////////////////////////////////////////////////
	// unary operations
	//
	// - binary operation이 unary로 사용가능하기에, 많이 생략되었음.
	//     ex) a.add(a,b)
	//////////////////////////////////////////////////////////////////////
	vectorn& assign(const vector3& other);
	vectorn& assign(const quater& other);
	vectorn& normalize(vectorn const& a);
	vectorn& assign(const vectorn& other) { _tvectorn<m_real>::assign(other);	return *this;}
	vectorn& resample(vectorn const& vec, int numSample);
	vectorn& concaten(vectorn const& a);

	// 카피해서 받아온다.

	// 카피해서 받아온다.

	vectorn& operator=(const _tvectorn<m_real>& other)	{ _tvectorn<m_real>::assign(other);return *this;}
	vectorn& operator=(const vectorn& other);//		{ assign(other);return *this;}
	vectorn& operator=(const vectornView& other);//	{ assign(other);return *this;}

    vectorn& operator=( vector3 const& other)		{ return assign(other);};
	vectorn& operator=( quater const& other)		{ return assign(other);};
	vectorn& derivative(vectorn const& a);
	vectorn Extract(const intvectorn& columns)	const { vectorn c; c.extract(*this, columns); return c;}
	vectorn& sort(vectorn const& source, intvectorn& sortedIndex);

	// slow unary operations (negation)
	friend vectorn operator-( vectorn const& a);

	//////////////////////////////////////////////////////////////////////
	// void operations
	//////////////////////////////////////////////////////////////////////
	vectorn& op0(const v0::_op&);
	vectorn& normalize();
	vectorn& negate();

	//////////////////////////////////////////////////////////////////////
	// aggregate functions
	//////////////////////////////////////////////////////////////////////

	m_real aggregate(CAggregate::aggregateOP eOP) const;
	m_real length() const ;
	m_real minimum() const;
	m_real maximum()	const;
	m_real sum()	const;
	m_real squareSum() const;
	m_real avg() const;

	// matrix의 각 column vector들에 scalar unary를 적용한다. (결과 vector dim은 cols())
	// ex) v.aggregateColumn(vectorn::minimum, mat);
	vectorn& aggregateColumn(CAggregate::aggregateOP eOP, matrixn const& mat);
	// matrix의 각 row vector들에 scalar unary를 적용한다. (결과 vector dim은 rows())
	vectorn& aggregate(CAggregate::aggregateOP eOP, matrixn const& mat);

	vectorn& minimum(const matrixn& other) { return aggregateColumn(CAggregate::MINIMUM, other);}
	vectorn& maximum(const matrixn& other) { return aggregateColumn(CAggregate::MAXIMUM, other);}
	vectorn& lengths(const matrixn& other) { return aggregate(CAggregate::LENGTH, other);}
	vectorn& curvature(const matrixn& pos) ;
//	vectorn& lengths(cmplxvectorn const& a);


	void findMax(m_real& max_v, int& max_index,int start=0,int end=INT_MAX) const;
	void findMin(m_real& min_v, int& min_index,int start=0,int end=INT_MAX) const;
	vectorn& colon(m_real start, m_real stepSize, int nSize=-1);
	void colon2(m_real start, m_real end, m_real stepSize=1);
	vectorn& linspace(m_real x1, m_real x2, int nSize=-1);
	//!< uniform sampling : sample centers of intervals in linspace of size n+1; eg> uniform (0,3, size=3) -> (  0.5, 1.5, 2.5 ).
	vectorn& uniform(m_real x1, m_real x2, int nSize=-1);


	///////////////////////////////////////////////////////////////////////
	// Utility functions
	//////////////////////////////////////////////////////////////////////

	TString output(const char* formatString="%f", int start=0, int end=INT_MAX) const;
	void	  load(const char* filename, bool bLoadFromBinaryFile=true);
	void      save(const char* filename, bool bSaveIntoBinaryFile=true);

	vectorn& fromMatrix(matrixn const& mat);

	void setVec3( int start, const vector3& src)	{ for(int i=start; i<start+3; i++) (*this)[i]=src.getValue(i-start);}
	void setQuater( int start, const quater& src)	{ for(int i=start; i<start+4; i++) (*this)[i]=src.getValue(i-start);}

	vector3 toVector3(int startIndex=0)	const	{ vector3 out; for(int i=0; i<3; i++) out[i]=getValue(i+startIndex); return out;};
	quater toQuater(int startIndex=0) const		{ quater out; for(int i=0; i<4; i++) out[i]=getValue(i+startIndex); return out;};

	friend class matrixn;

	// deprecated
	void getVec3( int start, vector3& src) const	{ for(int i=start; i<start+3; i++) src[i-start]=value(i);}
	void getQuater( int start, quater& src) const	{ for(int i=start; i<start+4; i++) src[i-start]=value(i);}

	int getSize() const	{ return size();}


	// deprecated - v::for_each, v::for_each1, v::for_each2 로 바꾸는 중.(operatorTemplate.hpp)

	// vector의 각 value들에 scalar binary를 적용한다.
	// ex) v.each(s2::MINIMUM, a, b);

	vectorn& each2(m_real (*s2_func)(m_real,m_real), vectorn const& a, vectorn const& b);
	vectorn& each2(const sv2::_op& op, const matrixn& a, const matrixn& b);
	vectorn& each2(const sv2::_op& op, const matrixn& a, const vectorn& b);
	vectorn& each1(void (*s1_func)(m_real&,m_real), vectorn const& a);
	vectorn& each1(void (*s1_func)(m_real&,m_real), m_real a);
	vectorn& each0(void (*s1_func)(m_real&,m_real))							{ return each1(s1_func, *this);}

	void op2(const v2::_op& op, const vectorn& a, const vectorn& b)			{ op.calc(*this, a,b);	}
	void op1(const v1::_op& op, const vectorn& a)							{ op.calc(*this, a);	}

	vectorn Each(void (*s1_func)(m_real&,m_real)) const;
	vectorn Each(m_real (*s2_func)(m_real,m_real), vectorn const& b) const;

};



class vectornView :public vectorn
{
public:
	// L-value로 사용될수 있는, reference array로 만든다.
	vectornView (m_real* ptrr, int size, int stride);
	// 값을 reference로 받아온다.
	vectornView(const _tvectorn<m_real>& other)		{ assignRef(other);}
	vectornView(const vectorn& other)				{ assignRef(other);}
	vectornView(const vectornView& other)			{ assignRef(other);}

	~vectornView (){}

	// L-value로 사용되는 경우, 값을 copy한다.
	vectorn& operator=(const vectorn & other)			{ assign(other);return *this;}
	vectorn& operator=(const _tvectorn<m_real>& other)	{ _tvectorn<m_real>::assign(other);return *this;}
	vectorn& operator=(const vectornView& other)		{ assign(other);return *this;}
	vectorn& operator=( vector3 const& other)		{ return assign(other);};
	vectorn& operator=( quater const& other)		{ return assign(other);};

    inline operator vectorn& () const { return (vectorn&)(*this);}
};



/*
namespace math
{
	void makeSamplingIndex(vectorn& out, int nLen, int numSample);		 //!< a simplified uniform sampling
	void colon(vectorn& out, m_real stepSize, int nSize=-1);//!< start부터 stepSize간격으로 size()만큼 채움. (start, start+stepSize, start+stepSize*2,...)
	void linspace(vectorn& out, m_real x1, m_real x2, int nSize=-1);		 //!< generates a vector of linearly equally spaced points between X1 and X2; eg> linspace(0,3, size=4) -> (0,   1,   2,   3)
	void uniform(vectorn& out, m_real x1, m_real x2, int nSize=-1);		 //!< uniform sampling : sample centers of intervals in linspace of size n+1; eg> uniform (0,3, size=3) -> (  0.5, 1.5, 2.5 ).
	void findMin(vectorn const& in, m_real& min, int& min_index) const;
	void findMax(m_real& max, int& max_index) const;
	int argMin(vectorn const& in)			{ m_real min; int min_index; findMin(min, min_index); return min_index;}
	int argMax(vectorn const& in)			{ m_real max; int max_index; findMax(max, max_index); return max_index;}
	int argNearest(vectorn const& in, m_real value) ;
	vectorn& interpolate(vectorn const& a, vectorn const& b, m_real t);

}*/


namespace v
{
	void eig(vectorn& eigenvalues, const matrixn& mat);
	void linspace(vectorn& out, m_real x1, m_real x2, int nSize=-1);		 //!< generates a vector of linearly equally spaced points
	void findMin(const vectorn& v, m_real& min_v, int& min_index) ;
	void findMax(const vectorn& v, m_real& max_v, int& max_index) ;
	intvectorn colon(int start, int end);
}

#endif
