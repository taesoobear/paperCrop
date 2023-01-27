#pragma once

#include "math_macro.h"
#include "../math/tvector.h"
namespace v
{
	int argMinRand(vectorn const& a, m_real thr=1.05, int start=0, int end=INT_MAX);
	void interpolate(vectorn & out, m_real t, vectorn const& a, vectorn const& b);
	m_real sample(vectorn const& in, m_real criticalTime);

}

namespace m
{
	// covariance * numData: usually called the matrix S.
	void covarianceN(vectorn& mean, matrixn& c, const matrixn& a) ;
	// det = e**log_det
	void LUinvert(matrixn& out, const matrixn& in, m_real& log_det);
	// det = det_mantissa * 10**det_exponent
	void LUinvert(matrixn& out, const matrixn& in, m_real & det_man, int& det_exp);

	void Diaginvert(vectorn& out, const vectorn& in, m_real& log_det);
	void SVinverse( matrixn& in_u, matrixn&mat ) ;

	// in=>u*diag(s)*vT. in is replaced by u.
	void SVdecompose(matrixn& in_u, vectorn & s, matrixn &v);
	void LUsolve(matrixn const & A, vectorn const& b, vectorn& x);
	void PIsolve(matrixn const & A, vectorn const& b, vectorn& x);
	matrixn diag(vectorn const& a);
	matrixn op1(m1::_op const& op, matrixn const& A);

	// returns v^T * M * v
	m_real vMv(vectorn const& v, matrixn const& M);
	// returns v^T * Diag(M) * v
	m_real vDv(vectorn const& v, vectorn const& diagM);

	// returns (a-b)^T * M *(a-b)
	m_real sMs(vectorn const& a, vectorn const& b, matrixn const& M);

	// returns (a-b)^T * Diag(M) *(a-b)
	m_real sDs(vectorn const& a, vectorn const& b, vectorn const& diagM);

	// returns (a-b)^T * (a-b)  : squared distance
	m_real ss(vectorn const& a, vectorn const& b);

	m_real determinant(const matrixn& E);

	void multAB(matrixn& out, matrixn const& a, matrixn const& b, bool transposeA=false, bool transposeB=false);
	void multABC(matrixn& out, matrixn const& a, matrixn const& b, matrixn const& c, bool transposeA=false, bool transposeB=false, bool transposeC=false);

	void multA_diagB(matrixn& c, matrixn const& a, vectorn const& b);
	void multAtB(vectorn& out, matrixn const& A, vectorn const& b);

	// c: input e: eigen values, v: eigen vectors (each column).
	// c can be recovered from e and v using
	//  c.op2(m2::multABAt(), v, m::diag(e));
	//
	// method==0 : jacobi
	// method==1 : tqli, tred2 -> faster, but only for symmetric tridiagonal matrices.
	// eigen values and corresponding eigen vectors are sorted in decreasing eigen-values order.
	void eigenDecomposition(matrixn const& c, vectorn & e, matrixn & v, int method=1);

	index2 argMin(matrixn const& a);
	index2 argMinRand(matrixn const& a, m_real thr=1.05);

}

namespace sop
{
	int interpolateInt(m_real t, int s, int e);
	m_real interpolate(m_real t, m_real s, m_real e);

	// smoothTransition(0)=0, smoothTransition(1)=1, dy/dx= 0 at x=0 and 1
	m_real smoothTransition(m_real a);
	// if(t<min) return v1 elseif (t>max) return v2, otherwise, inbetween v1 and v2 linearly.
	m_real clampMap(m_real t, m_real min, m_real max, m_real v1=0.0, m_real v2=1.0);
	m_real map(m_real t, m_real min, m_real max, m_real v1=0.0, m_real v2=1.0);
}



namespace m0
{
	struct drawSignals : public _op
	{
		drawSignals(const char* filename, float fMin=0.f, float fMax=0.f, bool bMultiDimensional=false, const intvectorn& vXplot=intvectorn())
			: m_strFilename(filename), m_fMin(fMin), m_fMax(fMax), m_bMultiDimensional(bMultiDimensional), m_vXplot(vXplot),m_bUseTheSameMinMax(true) {}
		drawSignals(const char* filename, bool useTheSameMinMax, const intvectorn& vXplot=intvectorn());
		virtual void calc(matrixn& c) const;
		const char* m_strFilename;
		const intvectorn& m_vXplot;
		float m_fMin, m_fMax;
		bool m_bMultiDimensional;
		bool m_bUseTheSameMinMax;
	};

	struct diagonalize: public _op
	{
		diagonalize(){}
		virtual void calc(matrixn& c) const;
	};
	struct draw : public _op
	{
		draw(const char* filename, float fMin=0.f, float fMax=0.f)
			: m_strFilename(filename), m_fMin(fMin), m_fMax(fMax) {}
		virtual void calc(matrixn& c) const;
		const char* m_strFilename;
		float m_fMin, m_fMax;
	};

	struct plotScatteredData : public _op
	{
		plotScatteredData(const char* filename, const vectorn& min, const vectorn& max):m_vMin(min), m_vMax(max), m_strFilename(filename){}
		virtual void calc(matrixn& c) const;
		const char* m_strFilename;
		vectorn m_vMin, m_vMax;
	};

	struct useUnary : public _op
	{
		useUnary(const m1::_op& cOP):m_cOperator(cOP) {}
		virtual void calc(matrixn& c) const;
		const m1::_op& m_cOperator;
	};

	struct linspace : public _op
	{
		linspace(const vectorn& start, const vectorn& end, int n): m_vStart(start), m_vEnd(end), m_n(n) {}
		virtual void calc(matrixn& c) const;
		vectorn m_vStart;
		vectorn m_vEnd;
		int m_n;
	};
}

namespace m1
{
	struct inverseDiagonal : public _op
	{
		inverseDiagonal (){}
		virtual void calc(matrixn& c, matrixn const& a) const;
	};

	struct cofactor:public _op
	{
		cofactor(){}
		virtual void calc(matrixn& c, matrixn const& a) const;
	};
	struct add: public _op
	{
		add(){}
		virtual void calc(matrixn& c, matrixn const& a) const;
	};

	struct multAdd: public _op
	{
		multAdd(m_real mult):mMult(mult){}
		m_real mMult;
		virtual void calc(matrixn& c, matrixn const& a) const;
	};

	struct transpose:public _op
	{
		transpose(){}
		virtual void calc(matrixn& c, const matrixn& a) const {c.transpose(a);}
	};
	struct LUinvert: public _op
	{
		LUinvert(){}
		virtual void calc(matrixn& c, const matrixn& a) const;/// c.op(a)
	};

	struct filter : public _op
	{
		filter(const vectorn& kernel, int numIter=1);
		filter(int kernelSize, int numIter=1);
		virtual void calc(matrixn& c, const matrixn& a) const;/// c.op(a)
		vectorn m_vKernel;
		int m_nNumIter;
	};

	struct splineFit : public _op
	{
		splineFit(int degree):m_nDegree(degree){}
		virtual void calc(matrixn& c, const matrixn& a) const;/// c.op(a)
		int m_nDegree;
	};

	struct superSampling : public _op
	{
		superSampling (int xn, int start=0, int end=INT_MAX):m_nXn(xn), m_nStart(start), m_nEnd(end){}
		virtual void calc(matrixn& c, const matrixn& a) const;/// c.op(a)
		int m_nXn, m_nStart, m_nEnd;
	};

	struct downSampling : public _op
	{
		downSampling (int xn, int start=0, int end=INT_MAX):m_nXn(xn), m_nStart(start), m_nEnd(end){}
		virtual void calc(matrixn& c, const matrixn& a) const;/// c.op(a)
		int m_nXn, m_nStart, m_nEnd;
	};


	struct derivative: public _op
	{
		derivative(){}
		virtual void calc(matrixn& c, const matrixn& a) const;
	};

	struct covariance : public _op
	{
		covariance(){}
		virtual void calc(matrixn& c, const matrixn& a) const;
	};

}

namespace m2
{
	struct multAtBA : public _op
	{
		multAtBA(){}
		// C=A^T*B*A
		virtual void calc(matrixn& c, matrixn const& a, matrixn const& b) const;
	};

	struct multABAt : public _op
	{
		// C=A*B*A^T
		multABAt(){}
		virtual void calc(matrixn& c, matrixn const& a, matrixn const& b) const;
	};


	struct linstitch: public _op
	{
		m_real mStrength;
		// strength가 클수록 c0stitch에 가까워진다.(overshooting확률이 적어진다.) 0보다 큰값을 대입해야한다.
		linstitch(m_real strength=5):mStrength(strength){}
		// a의 마지막 프레임이 b의 첫프레임과 동일해지도록 stitch. 최종 길이는 a.size()+b.size()-1이 된다.
		virtual void calc(matrixn& c, const matrixn& a, const matrixn& b) const;
	};


	// deprecated: use matrixn::multABt instead.
	struct multABt : public _op
	{ // C=A*B^T
		multABt(){}
		virtual void calc(matrixn& c, matrixn const& a, matrixn const& b) const;
	};



}
namespace v0
{
	/// uniform sampling : sample centers of intervals in linspace of size n+1; eg> uniform (0,3, size=3) -> (  0.5, 1.5, 2.5 ).
	struct uniformSampling : public _op
	{
		m_real x1, x2;
		int nSize;
		uniformSampling (m_real xx1, m_real xx2, int nnSize=-1):x1(xx1),x2(xx2),nSize(nnSize){}
		virtual void calc(vectorn& c) const;

	};

	struct linspace : public _op
	{
		linspace(m_real start, m_real end, int nSize=-1):mStart(start), mEnd(end), mnSize(nSize){}
		virtual void calc(vectorn& c) const { c.linspace(mStart,mEnd, mnSize);}
		m_real mStart,mEnd;
		int mnSize;
	};

	struct colon : public _op
	{
		colon(int start, int end, int stepSize):mStart(start), mEnd(end), mStepSize(stepSize){}
		virtual void calc(vectorn& c) const;
		int mStart, mEnd, mStepSize;
	};

	struct transition : public _op
	{
		transition(m_real start, m_real end, int nSize):mStart(start), mEnd(end), mnSize(nSize){}
		virtual void calc(vectorn& c) const ;
		m_real mStart,mEnd;
		int mnSize;
	};

	struct pow : public _op
	{
		pow(m_real k):mK(k){}
		virtual void calc(vectorn &c) const;
		m_real mK;
	};

	struct decay : public _op
	{
		// 0으로 떨어지는 커브를 만든다.
		enum decayT { LINEAR, COS, TRANSITION } ;
		decay(m_real startvalue, int nSize, decayT type=COS):mStart(startvalue), mnSize(nSize), mType(type){}
		void operator()(vectorn& c) const;
		virtual void calc(vectorn& c) const ;
		m_real mStart;
		int mnSize;
		decayT mType;
	};


	// usage: v0::zeroToOne(10, SLOW_START)(c);	-> c를 10프레임짜리 0에서 1로가는 커브로 초기화.
	struct zeroToOne
	{
		// 0에서 1로 올라가는 커브를 만든다.
		// SLOW_START는 커브 기울기가 0에서 시작해서 점점커짐.
		// SLOW_END는 커브 기울기가 점점 0으로 수렴.
		// TRANSITION은 커브 시작과 끝 기울기가 0
		enum { LINEAR, SLOW_START, SLOW_END, TRANSITION } ;
		zeroToOne(int nSize, int type=SLOW_START):mnSize(nSize), mType(type){}
		void operator()(vectorn& c) const;
		int mnSize;
		int mType;
	};

	// usage: v0::oneToZero(10, SLOW_START)(c);	-> c를 10프레임짜리 1에서 0으로가는 커브로 초기화.
	struct oneToZero
	{
		// 0에서 1로 올라가는 커브를 만든다.
		// SLOW_START는 커브 기울기가 0에서 시작해서 점점작아짐.
		// SLOW_END는 커브 기울기가 점점 0으로 수렴.
		// TRANSITION은 커브 시작과 끝 기울기가 0
		enum { LINEAR, SLOW_START, SLOW_END, TRANSITION } ;
		oneToZero(int nSize, int type=SLOW_START):mnSize(nSize), mType(type){}
		void operator()(vectorn& c) const;
		int mnSize;
		int mType;
	};

	struct drawSignals : public _op
	{
		drawSignals(const char* filename, float fMin=0.f, float fMax=0.f, const intvectorn& vXplot=intvectorn())
			: m_strFilename(filename),m_strCommentname(NULL), m_fMin(fMin), m_fMax(fMax), m_vXplot(vXplot) {}
		drawSignals(const char* filename,const char* commentName, float fMin=0.f, float fMax=0.f, const intvectorn& vXplot=intvectorn())
				: m_strFilename(filename),m_strCommentname(commentName), m_fMin(fMin), m_fMax(fMax), m_vXplot(vXplot) {}
		virtual void calc(vectorn& c) const;
		const char* m_strFilename;
		const char* m_strCommentname;
		const intvectorn& m_vXplot;
		float m_fMin, m_fMax;
	};

	/// c=SORT(c) 처럼 unaryop를 사용한다.
	struct useUnary : public _op
	{
		useUnary(const v1::_op& cOP):m_cOperator(cOP) {}
		virtual void calc(vectorn& c) const;
		const v1::_op& m_cOperator;
	};



}


namespace v1
{
	struct reverse
	{
		template <class VEC_TYPE>
		void operator()(VEC_TYPE& c, const VEC_TYPE& a) const
		{
			c.setSize(a.size());
			for(int i=0; i<a.size(); i++)
				c[c.size()-i-1]=a[i];
		}
	};

	/// c=ADD(c,d) 처럼 binaryop를 사용한다. eg) matrix1.op1(m1::useBinary(m2::...()))..
	struct useBinary : public _op
	{
		useBinary(const v2::_op& cOP):m_op(cOP) {}
		virtual void calc(vectorn& c, const vectorn& a) const;
		const v2::_op& m_op;
	};

	struct downSampling : public _op
	{
		downSampling (int xn, int start=0, int end=INT_MAX):m_nXn(xn), m_nStart(start), m_nEnd(end){}
		virtual void calc(vectorn& c, const vectorn& a) const;/// c.op(a)
		int m_nXn, m_nStart, m_nEnd;
	};


	struct delta: public _op
	{
		delta(){}
		// c.size()=a.size()-1. c[i]=a[i+1]-a[i].
		void operator()(vectorn& c, const vectorn& a) const;
		virtual void calc(vectorn& c, const vectorn& a) const;
	};

	struct cumulate: public _op
	{
		cumulate(){}
		virtual void calc(vectorn& c, const vectorn& a) const;

	};

	struct secondDerivative: public _op
	{
		secondDerivative(){}
		virtual void calc(vectorn& c, const vectorn& a) const;
	};

	struct freq : public _op
	{
		freq(int windowSize):m_nWindowSize(windowSize) {}
		virtual void calc(vectorn& c, const vectorn& a) const;
		int m_nWindowSize;
	};


	struct each: public _op
	{
		each(void (*s1_func)(m_real&,m_real)):op(s1_func){}
		virtual void calc(vectorn& c, const vectorn& a) const			{ c.each1(op, a); }
		void (*op)(m_real&,m_real);
	};

	struct radd : public _op
	{
		radd(){}
		virtual void calc(vectorn& c, const vectorn& a) const			{ c.each1(s1::RADD, a); }
	};

	struct rsub : public _op
	{
		rsub(){}
		virtual void calc(vectorn& c, const vectorn& a) const			{ c.each1(s1::RSUB, a); }
	};

	struct rdiv : public _op
	{
		rdiv(){}
		virtual void calc(vectorn& c, const vectorn& a) const			{ c.each1(s1::RDIV, a); }
	};

	struct rmult: public _op
	{
		rmult(){}
		virtual void calc(vectorn& c, const vectorn& a) const			{ c.each1(s1::RDIV, a); }
	};

/*

*/
	struct sort: public _op
	{
		sort(){}
		virtual void calc(vectorn& c, const vectorn& a) const;
	};

	struct filter : public _op
	{
		filter(const vectorn& kernel, int numIter=1);
		filter(int kernelSize, int numIter=1);
		virtual void calc(vectorn& c, const vectorn& a) const;/// c.op(a)
		vectorn m_vKernel;
		int m_nNumIter;
	};

	/// c의 일부 컬럼(m_vIndex)의 값을 a로 부터 가져와 할당한다. (즉 m_vIndex.size()==a.size())
	struct assign : public _op
	{
		assign(){}
		assign(const intvectorn& index);
		assign(int start, int end, int stepSize=1);
		virtual void calc(vectorn& c, const vectorn& a) const;/// c.op(a)
		intvectorn m_vIndex;
	};


}

namespace v2
{
	// adaptive filtering
	struct filter : public _op
	{
		filter(float fFrameTime):m_fFrameTime(fFrameTime){}
		virtual void calc(vectorn& c, const vectorn& source, const vectorn& kernelSize) const;/// c.op(a)
		float m_fFrameTime;
	};


	struct multAdd : public _op
	{
		multAdd(){}
		virtual void calc(vectorn& c, const vectorn& a, const vectorn& b) const;
		//virtual void calc(vectorn& c, const vectorn& a, m_real b) const;
	};

	struct cross : public _op
	{
		cross(){}
		virtual void calc(vectorn& c, const vectorn& a, const vectorn& b) const;
	};

	struct minimum : public _op
	{
		minimum(){}
		virtual void calc(vectorn& c, const vectorn& a, const vectorn& b) const;
	};

	struct maximum : public _op
	{
		maximum(){}
		virtual void calc(vectorn& c, const vectorn& a, const vectorn& b) const;
	};

	struct add : public _op
	{
		add(){}
		virtual void calc(vectorn& c, const vectorn& a, const vectorn& b) const;
		//virtual void calc(vectorn& c, const vectorn& a, m_real b) const;
		//virtual void calcInt(intvectorn& c, const intvectorn& a, const intvectorn& b) const;
		//virtual void calcInt(intvectorn& c, const intvectorn& a, int b) const;
	};

	struct sub : public _op
	{
		sub(){}
		virtual void calc(vectorn& c, const vectorn& a, const vectorn& b) const;
		//virtual void calc(vectorn& c, const vectorn& a, m_real b) const;
	};

	struct mid : public _op
	{
		mid(){}
		virtual void calc(vectorn& c, const vectorn& a, const vectorn& b) const;
	};

	struct interpolate : public _op
	{
		interpolate (m_real t):m_fT(t){}
		virtual void calc(vectorn& c, const vectorn& a, const vectorn& b) const;
		m_real m_fT;
	};
}

namespace sv2
{
	struct dotProduct : public _op
	{
		dotProduct(){}
		virtual m_real calc(const vectorn& a, const vectorn& b) const { return a%b; }
	};

/*	struct angle2D : public _op
	{
		angle2D(){}
		virtual m_real calc(const vectorn& a, const vectorn& b) const { return a.angle2D(b); }
	};*/

	struct angle : public _op
	{
		angle(){}
		virtual m_real calc(const vectorn& a, const vectorn& b) const { return a.angle(b); }
	};
}
