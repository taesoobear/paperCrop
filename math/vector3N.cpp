#include "stdafx.h"
#include "mathclass.h"
#include "vector.h"
#include "vector3N.h"
//#include "nr/nr.h"
#include "Operator.h"
#include "../utility/operatorString.h"


vector3NView ::vector3NView (m_real* ptrr, int size, int str)
:vector3N(ptrr,size,str)
{
}

vector3NView vector3N::range(int start, int end, int step)
{
	return _range<vector3NView>(start, end,step);
}

vector3N vector3N::operator*(m_real s) const
{
	vector3N out=*this;
	for(int i=0; i<out.size(); i++)
		out[i]*=s;
	return out;
}


vector3N::vector3N()
:_tvectorn<vector3, m_real>()
{	
}


vector3N::vector3N(int n)
:_tvectorn<vector3, m_real>()
{
	setSize(n);
}

vector3N::~vector3N()
{
	//release();
}

void vector3N::displacementOnline(const vector3& sp1, const vector3& sp2, const vector3& ap2, const vector3& ap3, int duration)
{
	/////////////////////////////////
	// sp123
	// ____ src motion
	//     ++++ add motion
	//  ap123
	//    - guess position and velocity of srcMotion of this frame
	//     **** make displacementmap
	//
	////////////////////////

	vector3 sp3;
	vector3 sv;
	vector3 av;
	sv.difference(sp1, sp2);
	av.difference(ap2, ap3);
	
	// guess position
	sp3.add(sp2, sv);
	vector3 ap1;
	// ap1+av=ap2;	
	ap1.sub(ap2, av);

   	vector3 displacement1, displacement2;
	displacement1.difference(ap1, sp2);
	displacement2.difference(ap2, sp3);
	hermite(displacement1, displacement2, duration-1, vector3(0,0,0), vector3(0,0,0));
}

void vector3N::displacement(const vector3& sp1, const vector3& sp2, const vector3& ap2, const vector3& ap3, int start, int end)
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

    vector3 center,center_halfvel, center_3halfvel;
	vector3 sp3, ap1;
	vector3 sv;
	vector3 av;
	sv.difference(sp1, sp2);
	av.difference(ap2, ap3);
	
	center.add(sp2,ap2);
	center/=2.f;

	center_halfvel.add(sv, av);
	center_halfvel/=4.f;

	// the following two lines are heuristic velocity adjustment. (Think about it by drawing the situation by yourself.)
	vector3 center_halfvel2=(ap2-sp2)/((float)(end-start)*8.f);
	center_halfvel+=center_halfvel2;

	center_3halfvel.mult(center_halfvel,3);

    // guess position
	vector3 m2,m1,p0,p1;
	m2.sub(center, center_3halfvel);
	m1.sub(center, center_halfvel);
	p0.add(center, center_halfvel);
	p1.add(center, center_3halfvel);

	static vector3N disp1, disp2;

	vector3 disp_sp1, disp_sp2;
	disp_sp1.difference(sp1, m2);
	disp_sp2.difference(sp2, m1);
	
	vector3 disp_ap2, disp_ap3;
	disp_ap2.difference(ap2, p0);
	disp_ap3.difference(ap3, p1);

	disp1.hermite(vector3(0,0,0), vector3(0,0,0), ABS(start)-1, disp_sp1, disp_sp2);
	disp2.hermite(disp_ap2, disp_ap3, end-1, vector3(0,0,0), vector3(0,0,0));

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


	
}

void _calcHalfvel(const vector3& p1, const vector3& p2, vector3& halfvel)
{
	vector3 v;
	v.difference(p1, p2);
	halfvel.mult(v, 0.5);
}

void vector3N::displacement(const vector3& sps, const vector3& sp1, const vector3& sp2, 
						const vector3& ap1, const vector3& ap2, const vector3& spe, int start, int end)
{	
	vector3 shalfvel;
	vector3 ahalfvel;
	_calcHalfvel(sp1, sp2, shalfvel);
	_calcHalfvel(ap2, ap1, ahalfvel);
    vector3 starget, atarget;		
	starget.add(sp2,shalfvel);
	atarget.add(ap1,ahalfvel);
    // starget = atarget이 되도록 하는 transform을 계산한다.
	vector3 centerTarget;
	centerTarget.lerp(starget, atarget, 0.5);
	
	vector3 sdiff, adiff;
	sdiff.difference(starget, centerTarget);
	
	
	// sps->starget*scale=starget
    

}

void vector3N::hermite(const vector3& a, const vector3& b, int duration, const vector3& c, const vector3& d)
{
	vector3 vel1, vel2;
	vel1.linearVelocity(a,b);
	vel2.linearVelocity(c,d);

	float totalTime=duration+1;
	vel1/=(1.f/totalTime);	// 시간으로 나눠주었다. 단위는 1프레임
	vel2/=(1.f/totalTime);	// 시간으로 나눠주었다. 단위는 1프레임

	setSize(duration);

	for(int i=0; i<duration; i++)
	{
		row(i).hermite(a, vel1, d, vel2, (float)(i+1)/totalTime);
	}
}

void vector3N::transition(const vector3& a, const vector3& b, int duration)
{
	setSize(duration);

	float totalTime=duration+1;
	float currTime;
	for(int i=0; i<duration; i++)
	{
		currTime=(float)(i+1)/totalTime;
		float t=-2.f*CUBIC(currTime)+3.f*SQR(currTime);
		row(i).lerp(a, b, t);
	}
}

void vector3N::assign(const vector3N& other)
{
	setSize(other.rows());
	for(int i=0; i<rows(); i++)
		row(i)=other.row(i);
}

void vector3N::assign(const matrixn& other)
{
	if(other.cols()!=3) return;

	setSize(other.rows());
	for(int i=0; i<rows(); i++)
		row(i)=other.row(i).toVector3();
}

void vector3N::c0stitch(int discontinuity)
{
	// I used cos function not to distort the start and end position.
	if(discontinuity<2)
	{
		printf("stitch impossible\n");
		return;
	}
	vector3 sv, av, center;
	sv.difference(row(discontinuity-2), row(discontinuity-1));
	av.difference(row(discontinuity+1), row(discontinuity));
	sv/=2.f;
	av/=2.f;    
	center.lerp(row(discontinuity-1), row(discontinuity), 0.5);

	vector3 strans, atrans;
	strans.difference(row(discontinuity-1)+sv, center);
	atrans.difference(row(discontinuity)+av, center);

	for(int i=0; i<discontinuity; i++)
		row(i)+=strans*pow(double(i)/ ((double)discontinuity-0.5),1.5);


	for(int i=discontinuity; i<rows(); i++)
		row(i)+=atrans*pow(1.f-(double(i-discontinuity)+0.5)/ ((double)(rows()-discontinuity)-0.5), 1.5);
}

/*
void vector3N::c0stitch1Gap()
{

	// 000X000 (signal의 center값이 invalid하다. 이 값을 무시하고 stitching한다.)
	// use cos function not to distort the start and end position.
	ASSERT(rows()%2==1);
	ASSERT(rows()>5);

	int discontinuity=rows()/2+1;

	vector3 sv, av, center;
	sv.difference(row(discontinuity-2), row(discontinuity-1));
	av.difference(row(discontinuity+2), row(discontinuity+1));
    
	vector3 strans, atrans;
	strans.difference(row(discontinuity-1)+sv, center);
	atrans.difference(row(discontinuity+1)+av, center);

	for(int i=0; i<discontinuity; i++)
		row(i)+=strans*(1-cos((M_PI/2.f)*float(i)/ ((float)discontinuity)));

	row(i)=center;

	for(int i=discontinuity+1; i<rows(); i++)
		row(i)+=atrans*(1-sin((M_PI/2.f)*(float(i-discontinuity))/ ((float)(rows()-discontinuity-1))));
}*/


void vector3N::c1stitch(int discontinuity)
{
    vector3N displacementMap;
	displacementMap.displacement(row(discontinuity-2), row(discontinuity-1), row(discontinuity), row(discontinuity+1), 0-discontinuity, rows()-discontinuity);

	for(int i=0; i<rows(); i++)
	{
		row(i)+=displacementMap[i];
	}
}


void vector3N::stitchTest(void (vector3N::*func)(int discontinuity), const char* filename)
{
	vector3N test;
	test.setSize(40);

	for(int i=0; i<20; i++)
	{
	test[i].x=20-i;
	test[i].y=i;
	test[i].z=20-i;
	}

	for(int i=20; i<40; i++)
	{
	test[i].x=20-i+10;
	test[i].y=i+10;
	test[i].z=i;
	}


	matrixn before(40,6);
	matrixn after(40,6);

	before.range(0,40, 0,3).assign(test);
	(test.*func)(20);
	after.range(0,40, 0,3).assign(test);

	for(int i=0; i<20; i++)
	{
		test[i].x=20-cos(i/10.0);
		test[i].y=cos(i/15.0);
		test[i].z=20-i;
	}

	for(int i=20; i<40; i++)
	{
		test[i].x=20-cos(i/10.0)+10;
		test[i].y=i+10;
		test[i].z=sin(i/20.0);
	}

	before.range(0,40, 3,6).assign(test);
	(test.*func)(20);
	after.range(0,40, 3,6).assign(test);

	before.op0(m0::drawSignals(sz1::format("%s_before.bmp", filename),0,0,true));
	after.op0(m0::drawSignals(sz1::format("%s_after.bmp", filename),0,0,true));
}

namespace v
{
	void c0stitch(vectorn& v, int discontinuity)
	{
		// I used cos function not to distort the start and end position.
		if(discontinuity<2)
		{
			printf("stitch impossible\n");
			return;
		}
		m_real sv, av, center;
		sv=v[discontinuity-1]-v[discontinuity-2];
		av=v[discontinuity]-v[discontinuity+1];
		sv/=2.f;
		av/=2.f;    
		center=(v[discontinuity-1]+v[discontinuity])* 0.5;

		m_real strans, atrans;
		strans= center-v[discontinuity-1]+sv;
		atrans=center-v[discontinuity]+av ;

		for(int i=0; i<discontinuity; i++)
			v[i]+=strans*pow(double(i)/ ((double)discontinuity-0.5),1.5);

		for(int i=discontinuity; i<v.size(); i++)
			v[i]+=atrans*pow(1.f-(double(i-discontinuity)+0.5)/ ((double)(v.size()-discontinuity)-0.5), 1.5);
	}

}

void vector3N::linstitch(int discontinuity)
{
}


void vector3N::c0stitch(vector3N const& a, vector3N const& b)
{
	// I used cos function not to distort the start and end position.
	if(a.size()<2)
	{
		printf("stitch impossible\n");
		return;
	}
	vector3 center;
	center.lerp(a.row(a.rows()-1), b.row(0), 0.5);

	vector3 strans, atrans;
	strans.difference(a.row(a.rows()-1), center);
	atrans.difference(b.row(0), center);

	setSize(a.rows()+b.rows()-1);
	int centerm=a.rows();
	for(int i=0; i<centerm; i++)
		row(i)=a.row(i)+strans*pow(double(i)/ ((double)centerm-1.0),1.5);

	for(int i=centerm; i<rows(); i++)
		row(i)=b.row(i-centerm+1)+atrans*pow(1.f-double(i-centerm+1)/double(rows()-centerm), 1.5);
}

void vector3N::linstitch(vector3N const& a, vector3N const& b)
{
}

void vector3N::hermite(int discontinuity)
{
	vector3N herm;
	herm.hermite(row(0), row(1), rows()-2, row(rows()-2), row(rows()-1));
	
	for(int i=1; i<rows()-1; i++)
	{
		row(i)=herm.row(i-1);
	}
}


void vector3N::translate(const vector3& trans)
{
	for(int i=0; i<rows(); i++)
	{
		row(i)+=trans;
	}
}

void vector3N::rotate(const quater& q) //!< Data 전체를 Rotate한다. root orientation quat앞에 곱해져서 제일 나중에
{	
	for(int i=0; i<rows(); i++)
	{
		row(i).rotate(q, row(i));
	}
}

	/*
void vector3N::c2stitch()
{

	c0stitch();

    vector3N firstDeriv;
	firstDeriv.setSize(rows()-1);

	for(int i=0; i<rows()-1; i++)
		firstDeriv[i].difference(row(i), row(i+1));

	firstDeriv.c0stitch1Gap();

	for(int i=1; i<rows(); i++)
		row(i).add(row(i-1),firstDeriv[i-1]);
}
*/

vectornView		vector3N::x() const
{
	m_real* ptr;
	int stride, n2, on2;
	_getPrivate(ptr, stride, n2, on2);
	
	return vectornView(ptr,rows(), stride);
}
	vectornView		vector3N::y() const
	{
		m_real* ptr;
		int stride, n2, on2;
		_getPrivate(ptr, stride, n2, on2);
		
		return vectornView(&ptr[1],rows(), stride);
	}
	vectornView		vector3N::z() const
	{
		m_real* ptr;
		int stride, n2, on2;
		_getPrivate(ptr, stride, n2, on2);
		
		return vectornView(&ptr[2],rows(), stride);
	}
	vectornView		vector3N::column(int i) const
	{
		m_real* ptr;
		int stride, n2, on2;
		_getPrivate(ptr, stride, n2, on2);
		
		return vectornView(&ptr[i],rows(), stride);
	}
