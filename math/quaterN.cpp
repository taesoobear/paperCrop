#include "stdafx.h"
#include "mathclass.h"
#include "quater.h"
#include "quaterN.h"
#include "Operator.h"

quaterNView ::quaterNView (m_real* ptrr, int size, int str)
:quaterN(ptrr,size,str)
{
}

quaterNView quaterN::range(int start, int end, int step)
{
	return _range<quaterNView >(start, end, step);
}

quaterN::quaterN()
:_tvectorn<quater, m_real  >()
{
	/*
	m_nSize=0;
	m_nCapacity=0;
	m_aQuater=NULL;*/
}

quaterN::quaterN(int n)
:_tvectorn<quater, m_real  >()
{
	setSize(n);
}


void quaterN::assign(const quaterN& other)
{
	_tvectorn<quater, m_real  >::assign(other);
}


quaterN::~quaterN()
{
	//release();
}

/*void quaterN::release()
{
	if(m_nSize!=0)
	{
		ASSERT(m_aQuater);
		delete[] m_aQuater;
		m_aQuater=NULL;
		m_nCapacity=0;
		m_nSize=0;
	}
}*/
/*
void quaterN::setSize(int n)
{
	if(m_nSize==n) return;
	if(n<=m_nCapacity)	m_nSize=n;
	else
	{
		int capacity=MAX(m_nCapacity,10);
		// capacity가 nsize를 포함할때까지 doubling
		while(capacity<n)	capacity*=2;

		release();
		m_nSize=n;
		m_nCapacity=capacity;
		m_aQuater=new quater[capacity];
	}
}*/

void quaterN::displacementOnline(const quater& sq1, const quater& sq22, const quater& aq2, const quater& aq3, int duration)
{
	/////////////////////////////////
	// sq123
	// ____ src motion
	//     ++++ add motion
	//  aq123
	//    - guess position and velocity of srcMotion of this frame
	//     **** make displacementmap
	//
	////////////////////////

	quater sq2(sq22);
	quater sq3;
	quater sv;
	quater av;
	sv.difference(sq1, sq2);
	av.difference(aq2, aq3);

	// guess position
	sq3.mult(sv, sq2);
	quater aq1;
	// av*aq1=aq2;
	aq1.mult(av.inverse(), aq2);

	quater displacement1, displacement2;
	sq2.align(aq1);
	sq3.align(aq2);
	displacement1.difference(aq1, sq2);
	displacement2.difference(aq2, sq3);
	hermite(displacement1, displacement2, duration-1, quater(1,0,0,0), quater(1,0,0,0));
}


void quaterN::displacement(const quater& sp1, const quater& sp2, const quater& ap22, const quater& ap33, int start, int end)
{
	/////////////////////////////////
	// sp123
	// ____ src motion
	//     ++++ add motion
	//  ap123
	//    -- guess position and velocity of srcMotion of these frames
	//   **** make displacementmap
	//  m21
	//  p  01
	//disp1
	//  ***
	//disp2
	//   ***
	////////////////////////

	quater ap2(ap22);
	quater ap3(ap33);

	//ASSERT(sp2%sp1>0);
	ap2.align(sp2);
	ap3.align(ap2);

	quater center,center_halfvel, center_3halfvel;
	quater sp3, ap1;
	quater sv;
	quater av;
	sv.difference(sp1, sp2);
	av.difference(ap2, ap3);

	center.interpolate(0.5f, sp2,ap2);

	center_halfvel.interpolate(0.5f, sv, av);

	// the following two lines are heuristic velocity adjustment. (Think about it by drawing the situation by yourself.)

	quater center_halfvel2;
	center_halfvel2.difference(sp2, ap2);
	center_halfvel2.align(quater(1,0,0,0));
	center_halfvel2.scale(1.f/((float)(end-start)*8.f));
	center_halfvel.leftMult(center_halfvel2);

	center_3halfvel=center_halfvel;
	center_3halfvel.scale(0.5f*3.f);

	center_halfvel.scale(0.5f);


	// guess position
	quater m2,m1,p0,p1;
	m2.mult( center_3halfvel.inverse(), center);
	m1.mult( center_halfvel.inverse(), center);
	p0.mult(center_halfvel, center);
	p1.mult(center_3halfvel, center);

	static quaterN disp1, disp2;

	quater disp_sp1, disp_sp2;
	disp_sp1.difference(sp1, m2);
	disp_sp2.difference(sp2, m1);

	quater disp_ap2, disp_ap3;
	disp_ap2.difference(ap2, p0);
	disp_ap3.difference(ap3, p1);

	disp1.hermite(quater(1,0,0,0), quater(1,0,0,0), ABS(start)-1, disp_sp1, disp_sp2);
	disp2.hermite(disp_ap2, disp_ap3, end-1, quater(1,0,0,0), quater(1,0,0,0));

	ASSERT(end-start==disp1.rows()+disp2.rows()+2);
	setSize(end-start);

	// forward filling
	for(int i=0; i<disp1.rows(); i++)
		row(i)=disp1.row(i);

	// center filling
	row(ABS(start)-1)=disp_sp2;
	row(ABS(start))=disp_ap2;

	// backward filling
	for(int i=0; i<disp2.rows(); i++)
		row(rows()-i-1)=disp2.row(disp2.rows()-i-1);


	///////////////////
	//testing code backup
	/*
	quaterN test;
	test.setSize(40);

	for(int i=0; i<20; i++)
	{
	test[i].setRotation(vector3(0,1,0), (20-i)/20.f);
	}

	for(int i=20; i<40; i++)
	{
	test[i].setRotation(vector3(0,1,0), (-i+10)/20.f);
	}

	matrixn temp;
	temp.assign(test);
	temp.op0(m0::drawSignals("qtemp1.bmp",-1.f,1.f,true));

	quaterN disp;
	disp.displacement(test[18], test[19], test[20],test[21], -20,20);

	for(int i=0; i<40; i++)
	test[i].leftMult(disp[i]);
	temp.assign(test);
	temp.op0(m0::drawSignals("qtemp2.bmp",-1.f,1.f,true));*/


}
void quaterN::hermite(const quater& a, const quater& b, int duration, const quater& c, const quater& d, float interval)
{
	vector3 vel1, vel2;
	vel1.angularVelocity(a,b);
	vel2.angularVelocity(c,d);

	float totalTime=duration+1;
	vel1/=(interval/totalTime);	// 시간으로 나눠주었다. 단위는 interval프레임
	vel2/=(interval/totalTime);	// 시간으로 나눠주었다. 단위는 interval프레임

	setSize(duration);
#ifdef SLOW_VERSION
	for(int i=0; i<duration; i++)
	{
		row(i).hermite(a, vel1, d, vel2, (float)(i+1)/totalTime);
	}
#else
	// factoring some part in the hermite function.
	const quater& qa=a;
	vector3& wa=vel1;
	const quater& qb=d;
	vector3& wb=vel2;

	quater q0, q1, q2, q3;

	// p0=pa, p1=pa+va/3, p2=pb-vb/3, p3=pb
	// -> quaternion으로 바꾸면..
	q0=qa;
	q1.mult((wa/3.f).quaternion(), qa);
	q2.mult((wb/3.f).quaternion().inverse(), qb);
	q3=qb;

	for(int i=0; i<duration; i++)
	{
		row(i).bezier(q0, q1, q2, q3, (float)(i+1)/totalTime);
	}
#endif
}

void quaterN::transition(const quater& aa, const quater& b, int duration)
{
	quater a(aa);
	a.align(b);

	setSize(duration);

	float totalTime=duration+1;
	float currTime;
	for(int i=0; i<duration; i++)
	{
		currTime=(float)(i+1)/totalTime;
		float t=-2.f*CUBIC(currTime)+3.f*SQR(currTime);
		row(i).slerp(a, b, t);
	}
}

void quaterN::transition0(const quater& aa, const quater& bb, int duration)
{
	quater a(aa);
	quater b(bb);
	quater qid;
	qid.identity();
	a.align(qid);
	b.align(qid);
	// kovar paper (prefer identity quaternion (more safe))
	setSize(duration);

	float totalTime=duration+1;
	float currTime;
	quater c, d, qi;
	qi.identity();
	for(int i=0; i<duration; i++)
	{
		currTime=(float)(i+1)/totalTime;
		float t=-2.f*CUBIC(currTime)+3.f*SQR(currTime);
		c.slerp(a, qi, t);
		d.slerp(qi, b, t);
		row(i).slerp(c, d, currTime);
	}
}

void quaterN::hermite0(const quater& a, const quater& b, int duration, const quater& c, const quater& d, float interval)
{
	quater qi(1,0,0,0);
	hermite(a,b,duration, qi, qi, interval);
	quaterN temp;
	temp.hermite(qi,qi,duration, c, d, interval);

	float totalTime=duration+1;
	for(int i=0; i<duration; i++)
	{
		row(i).slerp(row(i), temp.row(i), (float)(i+1)/totalTime );
	}
}

void quaterN::align()
{
	for(int i=0; i<rows()-1; i++)
		row(i+1).align(row(i));
}

void quaterN::c0stitch(int discontinuity)
{
	// I used cos function not to distort the start and end position.
	//align();

	quater sv, av, center;
	sv.difference(row(discontinuity-2), row(discontinuity-1));
	av.difference(row(discontinuity+1), row(discontinuity));
	sv.align(quater(1,0,0,0));
	av.align(sv);
	sv/=2.f;
	av/=2.f;
	center.safeSlerp(row(discontinuity-1), row(discontinuity), 0.5);
	center.align(quater(1,0,0,0));


	quater strans, atrans;
	strans.difference(sv*row(discontinuity-1), center);
	atrans.difference(av*row(discontinuity), center);
	strans.align(quater(1,0,0,0));
	atrans.align(quater(1,0,0,0));

	quater temp;
	for(int i=0; i<discontinuity; i++)
	{
		temp=strans;
		temp.scale(pow(double(i)/ ((double)discontinuity-0.5), 1.5));
		row(i).leftMult(temp);
	}


	for(int i=discontinuity; i<rows(); i++)
	{
		temp=atrans;
		temp.scale(pow(1.f-(double(i-discontinuity)+0.5)/ ((double)(rows()-discontinuity)-0.5), 1.5));
		row(i).leftMult(temp);
	}
}

void quaterN::c1stitch(int discontinuity)
{
	quaterN displacementMap;
	displacementMap.displacement(row(discontinuity-2), row(discontinuity-1), row(discontinuity), row(discontinuity+1), 0-discontinuity, rows()-discontinuity);

	for(int i=0; i<rows(); i++)
	{
		row(i).leftMult(displacementMap[i]);
	}
}

void quaterN::decomposeStitch(int discontinuity)
{
	quaterN rotY, offset;
	decompose(rotY, offset);
	rotY.c1stitch(discontinuity);
	offset.c1stitch(discontinuity);
	combine(rotY, offset);
}

void quaterN::decompose(quaterN& rotY, quaterN& offset) const
{
	rotY.setSize(rows());
	offset.setSize(rows());
	for(int i=0; i<rows(); i++)
		row(i).decompose(rotY[i], offset[i]);
}

void quaterN::combine(const quaterN& rotY, const quaterN& offset)
{
	for(int i=0; i<rows(); i++)
		row(i).mult(rotY[i], offset[i]);
}

void quaterN::hermite(int discontinuity)
{
	quaterN herm;
	herm.hermite(row(0), row(1), rows()-2, row(rows()-2), row(rows()-1));

	for(int i=1; i<rows()-1; i++)
	{
		row(i)=herm.row(i-1);
	}
}

namespace v
{
	void c0stitch(vectorn& v, int discontinuity);
}

void quaterN::linstitch(int discontinuity)
{
}


// a의 마지막 프레임이 b의 첫프레임과 동일해지도록 stitch. 최종 길이는 a.size()+b.size()-1이 된다.
void quaterN::linstitch(quaterN const& a, quaterN const& b)
{
}


void quaterN::c0stitch(quaterN const& a, quaterN const& b)
{
	quater center;
	center.safeSlerp(a.row(a.rows()-1), b.row(0), 0.5);
	center.align(quater(1,0,0,0));

	setSize(a.rows()+b.rows()-1);

	quater strans, atrans;
	strans.difference(a.row(a.rows()-1), center);
	atrans.difference(b.row(0), center);
	strans.align(quater(1,0,0,0));
	atrans.align(quater(1,0,0,0));

	quater temp;
	for(int i=0; i<a.rows(); i++)
	{
		temp=strans;
		temp.scale(pow(double(i)/ ((double)a.rows()-1), 1.5));
		row(i).mult(temp, a.row(i));
//		printf("%f ", pow(double(i)/ ((double)a.rows()-1), 1.5));
	}

	for(int i=a.rows(); i<rows(); i++)
	{
		temp=atrans;
		temp.scale(pow(1.f-(double(i-a.rows()+1))/ ((double)(b.rows()-1)), 1.5));
		row(i).mult(temp, b.row(i-a.rows()+1));
//		printf("%f ", pow(1.f-(double(i-a.rows()+1))/ ((double)(b.rows()-1)), 1.5));
	}
	//printf("\n");
	align();
}

void quaterN::c0stitchOnline(quaterN const& a, quaterN const& b)
{
	quater center=a.row(a.rows()-1);
	setSize(a.rows()+b.rows()-1);

	quater atrans;
	atrans.difference(b.row(0), center);
	atrans.align(quater(1,0,0,0));

	quater temp;

	for(int i=0; i<a.rows(); i++)
	{
		row(i)=a.row(i);
	}

	for(int i=a.rows(); i<rows(); i++)
	{
		temp=atrans;
		temp.scale(pow(1.f-(double(i-a.rows()+1))/ ((double)(b.rows()-1)), 1.5));
		row(i).mult(temp, b.row(i-a.rows()+1));
//		printf("%f ", pow(1.f-(double(i-a.rows()+1))/ ((double)(b.rows()-1)), 1.5));
	}
	//printf("\n");
	align();
}
void quaterN::c0stitchForward(quaterN const& a, quaterN const& b)
{
	quater center=b.row(0);

	setSize(a.rows()+b.rows()-1);

	quater strans;
	strans.difference(a.row(a.rows()-1), center);
	strans.align(quater(1,0,0,0));

	quater temp;
	for(int i=0; i<a.rows(); i++)
	{
		temp=strans;
		temp.scale(pow(double(i)/ ((double)a.rows()-1), 1.5));
		row(i).mult(temp, a.row(i));
	}

	for(int i=a.rows(); i<rows(); i++)
	{
		row(i)=b.row(i-a.rows()+1);
	}

	align();
}

void quaterN::c1stitch(quaterN const& a, quaterN const& b)
{
	// use hermite curve();
	c0stitch(a, b);

	/////////////////////////////////

	//    123 : time
	// _____ src motion
	//     ++++ add motion
	//     | con
	//     - guess velocity of srcMotion of this frame
	//   **** make displacementmap
	//  m21
	//  p  01
	//disp1
	//  ***
	//disp2
	//   ***
	////////////////////////

	int con=a.rows()-1;

	quater sp1(row(con-1));	// src position at time 1
	quater p2(row(con));	// center position at time 2
	quater ap3(row(con+1));	// add position at time3

	quater sv;
	quater av;
	quater cv;
	sv.difference(sp1, p2);
	av.difference(p2, ap3);

	cv.interpolate(0.5f, sv, av);	// desired velocity at con.

	// guess position of desired curve
	quater p1, p3;
	p1.mult( cv.inverse(), p2);
	p3.mult( cv, p2);

	quater sp3;
	sp3.mult(sv, p2);
	quater ap1;
	ap1.mult(av.inverse(), p2);

	static quaterN disp1, disp2;

	quater disp_sp2, disp_sp3;
	disp_sp2.identity();	// p2->p2
	disp_sp3.difference(sp3, p3);

	quater disp_ap1, disp_ap2;
	disp_ap1.difference(ap1, p1);
	disp_ap2.identity();

	// only half of velocity difference is stitched
	disp_sp3.scale(0.5);
	disp_ap1.scale(0.5);

	disp1.hermite(quater(1,0,0,0), quater(1,0,0,0), a.rows(), disp_sp2, disp_sp3);
	disp2.hermite(disp_ap1, disp_ap2, b.rows(), quater(1,0,0,0), quater(1,0,0,0));

	// forward filling
	for(int i=0; i<disp1.rows(); i++)
		row(i)*=disp1.row(i);

	// backward filling
	for(int i=0; i<disp2.rows(); i++)
		row(rows()-i-1)*=disp2.row(disp2.rows()-i-1);
}
