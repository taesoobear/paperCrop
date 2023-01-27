#ifndef _MATRIX_N_H_
#define _MATRIX_N_H_

#if _MSC_VER>1000
#pragma once
//#pragma message("Compiling matrix.h - this should happen just once per project.\n")
#endif
#include "vector_n.h"
#include <list>
#include <vector>
#include "vector3N.h"
#include "quaterN.h"
class matrixn;
class matrixnView;
class intmatrixn;
class intmatrixnView;


namespace m2
{
	struct _op
	{
		virtual void calc(matrixn& c, const matrixn& a, const matrixn& b) const {ASSERT(0);}
	};
}

namespace m1
{
	struct _op
	{
		virtual void calc(matrixn& c, const matrixn& a) const {ASSERT(0);}
	};
}

namespace m0
{
	struct _op
	{
		virtual void calc(matrixn& c) const {ASSERT(0);}
	};
}

#include "template_matrix.h"
//! 2D matrix ?´ë˜??
/*! conventions:
	1. ê·??ì²´ë¡?ë§¤íŠ¸ë¦?Š¤ë¥??œí˜„?˜ê±°??
	2. multi-dimensinal vector???œê°„ì¶??˜ì—´ ?•íƒœ (ê°?rowë²¡í„°ê°€ multi-dimensional vector, ì¦? ?œê°„ì¶•ì´ row ë°©í–¥???œë‹¤. )
	3. single-dimension signal ?¬ëŸ¬ê°? (?œê°„ì¶•ì´ columnë°©í–¥???œë‹¤.)
	?¬ì‹¤ 2?€ 3?€ êµ¬ë³„??ëª…í™•?˜ì? ?Šì?ë§? ?œë¡œ ?…ë¦½?ì¸ singal?€ ë³´í†µ 3ë²?convention???í•˜??ê²ƒì´ ê°ê° ?…ë¦½?ìœ¼ë¡?
	smoothing?˜ê±°??ê·¸ë˜?„ë? ê·¸ë¦´???ˆì–´ ?¸í•œ ë°˜ë©´(ex: ?¼ì† speed signal, ?¤ë¥¸??speed signal)
	?¬ì???ê°™ì? ?…ë¦½?ì´ì§€ ?Šì? signal?€ vector???˜ì—´??2ë²?convention???í•˜??ê²ƒì´ ì¢‹ê² ??
	\ingroup group_math
*/

class intmatrixn : public _tmat<int>
{
protected:
	intmatrixn(int* _ptr, int _n, int _m, int _stride) :_tmat<int>(_ptr, _n, _m, _stride){}
public:
	// copy constructors : copy values
	intmatrixn (const _tmat<int>& other):_tmat<int>()		{ assign(other);	}
	intmatrixn (const intmatrixn& other):_tmat<int>()		{ assign(other);	}
	intmatrixn():_tmat<int>(){}
	virtual ~intmatrixn(){}

	intmatrixn& operator=(const _tmat<int>& other)	{ assign(other); return *this;}
	intmatrixn& operator=(const intmatrixn& other)	{ assign(other); return *this;}
	intmatrixn& operator=(const intmatrixnView& other);//{ assign(other); return *this;}

	// if not found, return -1 else return the found row-index.
	int findRow(intvectorn const& row) const;

	/* already defined in _tmat<int>
	int     rows()    const			{ return n; }
	int     cols() const			{ return m; }

	void	  pushBack(const intvectorn& rowVec);
	void	  popBack(intvectorn* pOut=NULL);*/

	intmatrixnView range(int startRow, int endRow, int startColumn=0, int endColumn=INT_MAX);
	const intmatrixnView range(int startRow, int endRow, int startColumn=0, int endColumn=INT_MAX) const;

	// L-valueë¡??¬ìš©? ìˆ˜ ?ˆëŠ” reference vector ë¥?return?œë‹¤.
	intvectornView		row(int i)const			{ return _row<intvectornView>(i);}
	intvectornView		column(int i)const		{ return _column<intvectornView>(i);}
};

class intmatrixnView : public intmatrixn
{
public:
	// copy constructors : get reference
	intmatrixnView(const _tmat<int>& other)			{ _assignRef(other);	}
	intmatrixnView(const intmatrixn& other)			{ _assignRef(other);	}
	intmatrixnView(const intmatrixnView& other)		{ _assignRef(other);	}

	// L-valueë¡??¬ìš©? ë•Œ??copy
	intmatrixn& operator=(const _tmat<int>& other)	{ _tmat<int>::assign(other); return *this;}
	intmatrixn& operator=(intmatrixn const& other)			{ intmatrixn::assign(other); return *this;};
	intmatrixn& operator=(const intmatrixnView& other)	{ assign(other); return *this;}

	// L-valueë¡??¬ìš©? ìˆ˜ ?ˆëŠ”, reference matrixë¡?ë§Œë“ ??
	intmatrixnView (int* ptr, int nrow, int ncol, int stride);
	virtual ~intmatrixnView ();

};

class matrixn : public _tmat<m_real>
{
protected:
	matrixn(m_real* _ptr, int _n, int _m, int _stride) :_tmat<m_real>(_ptr, _n, _m, _stride){}
public:
	matrixn():_tmat<m_real>(){}
	matrixn ( int x, int y);

	// copy constructors : copy values.
	matrixn (const _tmat<m_real>& other):_tmat<m_real>()	{ _tmat<m_real>::assign(other);	}
	matrixn (const matrixn& other):_tmat<m_real>()		{ assign(other);	}
	matrixn (const matrixnView& other);//:_tmat<m_real>{ assign(other);	}

	virtual ~matrixn();

	// copy values.
	matrixn& operator=(const _tmat<m_real>& other)	{ _tmat<m_real>::assign(other); return *this;}
	matrixn& operator=(matrixn const& other)			{ _tmat<m_real>::assign(other); return *this;};
	matrixn& operator=(const matrixnView& other);//{ assign(other); return *this;}

	vector3 row3(int row) const						{ return vector3(value(row,0), value(row,1), value(row, 2));}

	// L-valueë¡??¬ìš©? ìˆ˜ ?ˆëŠ” reference vector ë¥?return?œë‹¤.
	vectornView		row(int i)const					{ return _row<vectornView>(i);}
	vectornView		column(int i)const				{ return _column<vectornView>(i);}
	vectornView		diag() const					{ return _diag<vectornView>();}

	// L-valueë¡??¬ìš©?????ˆëŠ” reference quaterN??return?œë‹¤.
	quaterNView toQuaterN() const;
	vector3NView toVector3N() const;

	matrixnView range(int startRow, int endRow, int startColumn=0, int endColumn=INT_MAX);
	const matrixnView range(int startRow, int endRow, int startColumn=0, int endColumn=INT_MAX) const;

	// ëª¨ë“  ?ì†Œê°€ valid?œì? ê²€?¬í•œ?? NaN?±ì´ ?ˆìœ¼ë©?false return.
	bool isValid() const;


	//////////////////////////////////////////////////////////////////////
	// void operations
	//////////////////////////////////////////////////////////////////////

	matrixn&  identity(int n);

	matrixn&  assign( matrixn const& other)	{ _tmat<m_real>::assign(other); return *this;}
	matrixn&  assign( matrix4 const&, bool bOnly3x3=false);
	matrixn&  assign( vector3N const& );
	matrixn&  assign( quaterN const& );

	// slow binary operations, I recommend you to use original function.
	friend matrixn operator+( matrixn const& a, matrixn const& b);
	friend matrixn operator-( matrixn const& a, matrixn const& b);
	friend matrixn operator*( matrixn const& a, matrixn const& b);
	friend matrixn operator/( matrixn  const& a, m_real b);
	friend matrixn operator*( matrixn const& a, m_real b );
	friend matrixn operator*( m_real b , matrixn const& a);

	// n-ary operations
	matrixn&	concatColumns( std::list<matrixn*> matrixes);

	// scalar unary functions
	m_real op1(CAggregate::aggregateOP eOP) const;
	m_real    distance(matrixn const& other)const ;	// matrix ?ê°œ ?¬ì´??ê±°ë¦¬, ?•ì˜??êµ¬í˜„ ì°¸ê³ 
	inline m_real length() const		{ return op1(CAggregate::LENGTH);}
	inline m_real minimum() const		{ return op1(CAggregate::MINIMUM);}
	inline m_real maximum()	const		{ return op1(CAggregate::MAXIMUM);}
	inline m_real sum()	const			{ return op1(CAggregate::SUM);}
	inline m_real squareSum() const		{ return op1(CAggregate::SQUARESUM);}
	inline m_real avg() const			{ return op1(CAggregate::AVG);}

	void	  pushBack3(const vector3& rowVec);

	void normalize(const vectorn &min, const vectorn&max);
	void toVector(vectorn& vec) const;			//!< concat all rows of this matrix into one large vector.
	vectorn toVector() const	{vectorn v; toVector(v); return v;}
	matrixn& fromVector(const vectorn& vec, int column);

	TString output(const char* formatString="%f", int start=0, int end=INT_MAX) const;

	// sum of diagonal elements.
	m_real trace() const;

	//////////////////////////////////////////////////////////////////
	// followings are deprecated (use NR, m, m1 or m2 namespaces)
	//////////////////////////////////////////////////////////////////
	matrixn& align() ;
	/// simplified interface. return eigenVectors=(*this)[i] and eigenValues[i]
	void eigenVectors(const matrixn& input, vectorn& eigenValues);
	// fourier spectrum
	void spectrum(const vectorn& input, int windowSize);

	matrixn&  inverse(const matrixn& other);
	matrixn&  pseudoInverse( const matrixn& other);	//!< return inv(A'A)A' (for overdetermined least square problem)
	
    matrixn&  derivative(matrixn const& positions);	// ex) velocities.delta(positions);
	matrixn&  derivative(matrixn const& positions, bitvectorn const& discontinuity);	// ex) velocities.delta(positions);
	matrixn&  derivativeQuater(matrixn const& rotations);	// ex) velocities.delta(positions);
	matrixn&  distanceMat(matrixn const&); //!< a matrix??ê°??ì†Œ ë²¡í„° ëª¨ë“  ì¡°í•© ?¬ì´??ê±°ë¦¬ êµ¬í•¨.


	matrixn&  distance(matrixn const&, matrixn const&);		//!< ??matrix??ê°??ì†Œ ë²¡í„° ëª¨ë“  ì¡°í•© ?¬ì´??ê±°ë¦¬ êµ¬í•¨.

	// followings are deprecated (please refer to operatorTemplate.hpp)
	matrixn& each2(m_real (*s2_func)(m_real,m_real), matrixn const& a, matrixn const& b);
	matrixn& each2(m_real (*s2_func)(m_real,m_real), matrixn const& a, m_real b);
	matrixn& each2(m_real (*s2_func)(m_real,m_real), m_real a, matrixn const& b);
	matrixn& each1(void (*s1_func)(m_real&,m_real), matrixn const& a);
	matrixn& each1(const v1::_op& op, vectorn const& a);
	matrixn& each1(const v1::_op& op, matrixn const& a);
	matrixn& each0(const v0::_op& op);
	matrixn& each0(vectorn& (vectorn::*func)());

	matrixn& op0(const m0::_op & op)											{ op.calc(*this); return *this;}
	matrixn& op0(const m1::_op & op) ;
	matrixn& op1(const m1::_op & op, const matrixn& other)						{ op.calc(*this, other); return *this;}
	matrixn& op2(const m2::_op & op, matrixn const& a, matrixn const& b)		{ op.calc(*this, a,b); return *this;}
	//matrixn& each2(vectorn& (vectorn::*func)(vectorn const&, vectorn const& ), matrixn const& a, matrixn const& b);
	matrixn& each2(_tvectorn<m_real>& (_tvectorn<m_real>::*func)(_tvectorn<m_real> const&, _tvectorn<m_real> const& ),
		matrixn const& a, matrixn const& b);
	matrixn& each2(_tvectorn<m_real>& (_tvectorn<m_real>::*func)(_tvectorn<m_real> const&, _tvectorn<m_real> const& ),
		matrixn const& a, vectorn const& b);

	// return by value. (slow)
	matrixn Each2(m_real (*s2_func)(m_real,m_real), matrixn const& b) const;

private:

};


// matrixUtil
/*
namespace matrixUtil
{
	matrixn&  resample(matrixn const& mat, int nSample);	// When num row is the number of data, it uniformly samples the data so that num row becomes nSample.
	matrixn&  linearResample(matrixn const& mat, vectorn const& samplingIndex);

	matrixn&  mergeUpDown( matrixn const&, matrixn const& );
	matrixn&  mergeLeftRight( matrixn const&, matrixn const& );
	void      splitUpDown( matrixn&, matrixn& );
	void      splitLeftRight( matrixn&, matrixn& );
	void      splitUpDown( matrixn&, matrixn&, int );
	void      splitLeftRight( matrixn&, matrixn&, int );

	void      LUdecompose( int* ) ;
	void	  LUsubstitute( int*, vectorn& );
	void      LUinverse( matrixn& ) ;
	void	spectrum(const vectorn& input, int windowSize);

void resample(int nSample);	// When num row is the number of data, it uniformly resamples this data so that num row becomes nSample.
}*/


class matrixnView : public matrixn
{
public:
	// copy constructors : get reference
	matrixnView(const _tmat<m_real>& other)		{ _assignRef(other);	}
	matrixnView(const matrixn& other)			{ _assignRef(other);	}
	matrixnView(const matrixnView& other)		{ _assignRef(other);	}

	// L-valueë¡??¬ìš©? ë•Œ??copy
	matrixn& operator=(const _tmat<m_real>& other)	{ _tmat<m_real>::assign(other); return *this;}
	matrixn& operator=(matrixn const& other)			{ matrixn::assign(other); return *this;};
	matrixn& operator=(const matrixnView& other)	{ assign(other); return *this;}

	// L-valueë¡??¬ìš©? ìˆ˜ ?ˆëŠ”, reference matrixë¡?ë§Œë“ ??
	matrixnView (m_real* ptr, int nrow, int ncol, int stride);
	virtual ~matrixnView ();

    // enable const to non-const casting (so that mat.range(...) can be directly used as a function argument.)
    inline operator matrixn& () const { return (matrixn&)(*this);}
};
#endif
