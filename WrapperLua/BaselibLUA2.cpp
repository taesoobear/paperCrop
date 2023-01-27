
/* 
   luabind�� �ٲٴ� ��.
   lua
   function f(a) return a.x+a.y end

// C++
luabind::object table = luabind::newtable(L);
table["x"] = 1;
table["y"] = 2;
functor<int> f(L,"f");
f(table);

You can put a table into another table.

// C++
luabind::object table = luabind::newtable(L);
luabind::object subtable = luabind::newtable(L);
table["sub"] = subtable;
functor<int> f(L,"f");
f(table);

*/

#include "stdafx.h"
#include "BaselibLUA.h"


#include <iostream>

#include "image/Image.h"
#include "image/ImageProcessor.h"

#include "math/matrix3.h"
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
#include <luabind/object.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/copy_policy.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/discard_result_policy.hpp>
#include <luabind/dependency_policy.hpp>
#include <luabind/luabind.hpp>

#include "BaselibLUA2.h"
#include "BaselibLUA3.h"

using namespace luabind;
void addBaselibModule(lua_State* L)
{

	// assumes that L is already opened and that luabind::open(L) is already called.

	// image
	{
		module(L)[
			class_<CImage>("CImage")
			.def(constructor<>())
			.def("GetWidth", &CImage::GetWidth)
			.def("GetHeight", &CImage::GetHeight)
			.def("Load", &CImage::Load)
			.def("Save", &CImage::Save)
			.def("save", &CImage::save)
			.def("create", &CImage::Create)
			.def("CopyFrom", &CImage::CopyFrom)
			.def("concatVertical", &Imp::concatVertical)
			.def("downsample4", &Imp::downsample4)
			.def("rotateRight", &Imp::rotateRight)
			.def("rotateLeft", &Imp::rotateLeft)
			.def("sharpen", &Imp::sharpen)
			.def("contrast", &Imp::contrast)
			.def("dither", &Imp::dither)
			.def("gammaCorrect", &Imp::gammaCorrect)
			.def("gamma", &Imp::gammaCorrect)
			.def("crop", &Imp::crop)
			.def("resize", &Imp::resize)
			.def("blit", &Imp::blit)
			.def("drawBox", &Imp::drawBox),
			class_<TRect>("TRect")
				.def(constructor<>())
				.def(constructor<int,int,int,int>())
				.def_readwrite("left", &TRect::left)
				.def_readwrite("right", &TRect::right)
				.def_readwrite("top", &TRect::top)
				.def_readwrite("bottom", &TRect::bottom)

				];
	}

	// vector3
	{
		void (vector3::*add1)(const vector3&, const vector3&) =&vector3::add;
		void (vector3::*sub1)(const vector3&, const vector3&) =&vector3::sub;
		void (vector3::*cross1)(const vector3&, const vector3&) =&vector3::cross;
		void (vector3::*add2)(const vector3&) =&vector3::add;
		void (vector3::*sub2)(const vector3&) =&vector3::sub;

		struct vector3_wrap
		{
			static void assign(vector3& l, object const& ll) 
			{
				if (type(ll) != LUA_TTABLE)
					throw std::range_error("vector3_assign");
				l.x=object_cast<double>(ll[1]);	// lua indexing starts from 1.
				l.y=object_cast<double>(ll[2]);
				l.z=object_cast<double>(ll[3]);
			}
			static std::string out(vector3& v)
			{
				return std::string(v.output());
			}

		};

		module(L)[
			class_<vector3>("vector3")
			.def(constructor<>())
			.def(constructor<double, double, double>())
			.def_readwrite("x", &vector3::x)
			.def_readwrite("y", &vector3::y)
			.def_readwrite("z", &vector3::z)
			.def("add", add1)
			.def("sub", sub1)
			.def("radd", add2)
			.def("rsub", sub2)
			.def("cross", cross1)
			.def("multadd", &vector3::multadd)
			.def("length", &vector3::length)
			.def(-self) // neg (unary minus)
			.def(self + self) // add (homogeneous)
			.def(self * self) // mul
			.def("assign", &vector3_wrap::assign)
			.def("set", &vector3_wrap::assign)
			.def("interpolate", &vector3::interpolate)
			.def("difference", &vector3::difference)
			.def("rotationVector", &vector3::rotationVector)
			.def("__tostring", &vector3_wrap::out)
			];
	}


	// TStrings
	{

		struct TStrings_wrap
		{
			static void assign(TStrings& l, object const& ll) 
			{
				if (type(ll) != LUA_TTABLE)
					throw std::range_error("TStrings_assign");

				l.resize(0);

				for(luabind::iterator i(ll), end; i!=end; ++i)
				{
					l.pushBack(luabind::object_cast<const char*>(*i));
				}
			}
			static std::string out(TStrings& v, int i)
			{
				return std::string(v[i]);
			}

			static void in(TStrings& v, int i, std::string& a)
			{
				v[i]=a.c_str();
			}

		};

		module(L)[
			class_<TStrings>("TStrings")
			.def(constructor<>())
			.def("assign", &TStrings_wrap::assign)
			.def("data", &TStrings_wrap::out)
			.def("setData", &TStrings_wrap::in)
			];
	}

	// quater
	{	
		struct quater_wrap
		{
			static void assign(quater& l, object const& ll) 
			{
				if (type(ll) != LUA_TTABLE)
					throw std::range_error("quater_assign");
				l.w=object_cast<double>(ll[1]);	// lua indexing starts from 1.
				l.x=object_cast<double>(ll[2]);	// lua indexing starts from 1.
				l.y=object_cast<double>(ll[3]);
				l.z=object_cast<double>(ll[4]);
			}
			static std::string out(quater& v)
			{
				return std::string(v.output());
			}

			static double toRadian(double deg)
			{
				return TO_RADIAN(deg);
			}

			static double toDegree(double rad)
			{
				return TO_DEGREE(rad);
			}

		};


		double (quater::*rotationAngle1)(void) const=&quater::rotationAngle;
		void (quater::*normalize1)()=&quater::normalize;

		module(L)[
			def("toRadian",&quater_wrap::toRadian),
			def("toDegree",&quater_wrap::toDegree),
			class_<quater>("quater")
				.def(constructor<>())
				.def(constructor<double, double, double, double>())
				.def_readwrite("x", &quater::x)
				.def_readwrite("y", &quater::y)
				.def_readwrite("z", &quater::z)
				.def_readwrite("w", &quater::w)
				.def("slerp", &quater::slerp)
				.def("safeSlerp", &quater::safeSlerp)
				.def("decomposeTwistTimesNoTwist", &quater::decomposeTwistTimesNoTwist)
				.def("decomposeNoTwistTimesTwist", &quater::decomposeNoTwistTimesTwist)
				.def("scale", &quater::scale)
				.def(-self) // neg (unary minus)
				.def(self + self) // add (homogeneous)
				.def(self * self) // mul
				.def("length", &quater::length)
				.def("rotationAngle", rotationAngle1)
				.def("rotationAngleAboutAxis", &quater::rotationAngleAboutAxis)
				.def("assign", &quater_wrap::assign)
				.def("set", &quater_wrap::assign)
				.def("leftMult", &quater::leftMult)
				.def("setRotation", (void (quater::*)(const vector3&, double))&quater::setRotation)
				.def("setRotation", (void (quater::*)(const vector3& ))&quater::setRotation)
				.def("setRotation", (void (quater::*)(const matrix4& a))&quater::setRotation)
				.def("normalize", normalize1)
				.def("align", &quater::align)
				.def("difference", &quater::difference)
				.def("__tostring", &quater_wrap::out)
				];
	}

	// matrix4
	{
		struct wrap_matrix4
		{
			static void assign2(matrix4& l, matrix4 const& m)
			{
				l=m;
			}
			static void assign(matrix4& l, object const& ll)
			{
				if(LUAwrapper::arraySize(ll)!=16) throw std::range_error("matrix4_assign");
				l._11=object_cast<double>(ll[1]);
				l._12=object_cast<double>(ll[2]);
				l._13=object_cast<double>(ll[3]);
				l._14=object_cast<double>(ll[4]);
				l._21=object_cast<double>(ll[5]);
				l._22=object_cast<double>(ll[6]);
				l._23=object_cast<double>(ll[7]);
				l._24=object_cast<double>(ll[8]);
				l._31=object_cast<double>(ll[9]);
				l._32=object_cast<double>(ll[10]);
				l._33=object_cast<double>(ll[11]);
				l._34=object_cast<double>(ll[12]);
				l._41=object_cast<double>(ll[13]);
				l._42=object_cast<double>(ll[14]);
				l._43=object_cast<double>(ll[15]);
				l._44=object_cast<double>(ll[16]);
			}
			static std::string out(matrix4& l)
			{
				TString v;
				v.format(" %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n", l._11,l._12,l._13,l._14,l._21,l._22,l._23,l._24,l._31,l._32,l._33,l._34,l._41,l._42,l._43,l._44);
				return std::string(v.ptr());
			}
			static void setRotation(matrix4& l, quater const& q)
			{
				l.setRotation(q);
			}

			static matrix4 inverse(matrix4 const& m)
			{
				matrix4 t;
				t.inverse(m);
				return t;
			}
		};
		struct wrap_matrix3
		{
#ifndef _MSC_VER
			static vector3  __mul_mv(matrix3 const& a, vector3 const& b)
			{
				return a*b;
			}

			static vector3 __mul_vm(vector3 const& b, matrix3 const& a)
			{
				return b*a;
			}

			static matrix3 __mul_mm(matrix3 const& a, matrix3 const& b)
			{
				return a*b;
			}
#endif

			static void assign(matrix3& l, object const& ll)
			{
				if(LUAwrapper::arraySize(ll)!=9) throw std::range_error("matrix4_assign");
				l._11=object_cast<double>(ll[1]);
				l._12=object_cast<double>(ll[2]);
				l._13=object_cast<double>(ll[3]);
				l._21=object_cast<double>(ll[4]);
				l._22=object_cast<double>(ll[5]);
				l._23=object_cast<double>(ll[6]);
				l._31=object_cast<double>(ll[7]);
				l._32=object_cast<double>(ll[8]);
				l._33=object_cast<double>(ll[9]);
			}
			static std::string out(matrix3& l)
			{
				TString v;
				v.format(" %f %f %f\n %f %f %f\n %f %f %f\n", l._11,l._12,l._13,l._21,l._22,l._23,l._31,l._32,l._33);
				return std::string(v.ptr());
			}
		};

		module(L)[
			class_<matrix4>("matrix4")
			.def_readwrite("_11", &matrix4::_11)
			.def_readwrite("_12", &matrix4::_12)
			.def_readwrite("_13", &matrix4::_13)
			.def_readwrite("_14", &matrix4::_14)
			.def_readwrite("_21", &matrix4::_21)
			.def_readwrite("_22", &matrix4::_22)
			.def_readwrite("_23", &matrix4::_23)
			.def_readwrite("_24", &matrix4::_24)
			.def_readwrite("_31", &matrix4::_31)
			.def_readwrite("_32", &matrix4::_32)
			.def_readwrite("_33", &matrix4::_33)
			.def_readwrite("_34", &matrix4::_34)
			.def_readwrite("_41", &matrix4::_41)
			.def_readwrite("_42", &matrix4::_42)
			.def_readwrite("_43", &matrix4::_43)
			.def_readwrite("_44", &matrix4::_44)
			.def(constructor<>())
			.def(constructor<const quater&, const vector3&>())
			.def(constructor<const transf&>())
			.def("assign", &wrap_matrix4::assign)
			.def("assign", &wrap_matrix4::assign2)
			.def("identity", &matrix4::identity)
			.def("__tostring", &wrap_matrix4::out)
			.def("setRotation", &wrap_matrix4::setRotation)
			.def("setRotation", (void (matrix4::*)(const vector3& axis, m_real angle, bool bPreserveCurrentTranslation) )&matrix4::setRotation)
			.def("setScaling", &matrix4::setScaling)
			.def("leftMultRotation", (void (matrix4::*)(const vector3& axis, m_real angle) )&matrix4::leftMultRotation)
			.def("leftMultRotation", (void (matrix4::*)(const quater& q) )&matrix4::leftMultRotation)
			.def("leftMultTranslation", &matrix4::leftMultTranslation)
			.def("leftMult", (void (matrix4::*)(const matrix4&))&matrix4::leftMult)
			.def("rightMult",(void (matrix4::*)(const matrix4&)) &matrix4::rightMult)
			.def("inverse", &matrix4::inverse)
			.def("inverse", &wrap_matrix4::inverse)
			.def(const_self*other<vector3 const&>())
			.def(const_self*other<matrix4 const&>()),
			class_<matrix3>("matrix3")
				.def_readwrite("_11", &matrix3::_11)
				.def_readwrite("_12", &matrix3::_12)
				.def_readwrite("_13", &matrix3::_13)
				.def_readwrite("_21", &matrix3::_21)
				.def_readwrite("_22", &matrix3::_22)
				.def_readwrite("_23", &matrix3::_23)
				.def_readwrite("_31", &matrix3::_31)
				.def_readwrite("_32", &matrix3::_32)
				.def_readwrite("_33", &matrix3::_33)
				.def(constructor<>())
				.def(constructor<matrix3 const&>())
				.def("assign", &wrap_matrix3::assign)
				.def("setValue", (void (matrix3::*)(m_real, m_real, m_real, m_real, m_real, m_real, m_real, m_real, m_real))&matrix3::setValue)
				.def("identity", &matrix3::identity)
				.def("zero", &matrix3::zero)
				.def("inverse", (bool (matrix3::*)(matrix3 const&))&matrix3::inverse)
				.def("transpose", &matrix3::transpose)
				.def("setTilde", (void (matrix3::*)( vector3 const &v ))&matrix3::setTilde)
				.def("__tostring", &wrap_matrix3::out)
				.def("setFromQuaternion", &matrix3::setFromQuaternion)
				.def("mult", (void (matrix3::*)(matrix3 const& a,matrix3 const& b))&matrix3::mult)
				.def("mult", (void (matrix3::*)(matrix3 const& a,m_real b))&matrix3::mult)

#ifdef _MSC_VER
				.def("__mul", (vector3 (*)(matrix3 const& a, vector3 const& b))&operator*)
				.def("__mul", (vector3 (*)(vector3 const& b, matrix3 const& a))&operator*)
				.def("__mul", (matrix3 (*)(matrix3 const& a, matrix3 const& b))&operator*)
#else
				.def("__mul", &wrap_matrix3::__mul_mv)
				.def("__mul", &wrap_matrix3::__mul_vm)
				.def("__mul", &wrap_matrix3::__mul_mm)
#endif
				];

		struct transf_
		{
			static void assign(transf &b, transf const& a)
			{
				b.rotation=a.rotation;
				b.translation=a.translation;
			}

			static vector3 mul_tv(transf const& a, vector3 const& b)
			{
				return a*b;
			}
			static transf mul_tt(transf const& a, transf const& b)
			{
				return a*b;
			}



		};
		module(L)[
			class_<transf>("transf")
			.def(constructor<>())
			.def(constructor<quater const&, vector3 const&>())
			.def(constructor<matrix4 const&>())

			.def_readwrite("rotation", &transf::rotation)
			.def_readwrite("translation",&transf::translation)
			.def("toLocal",&transf::toLocal)
#ifdef _MSC_VER
			.def("__mul", (vector3 (*)(transf const&, vector3 const&))&operator*)
			.def("__mul", (transf (*)(transf const&, transf const&))&operator*)		
#else
			.def("__mul", &transf_::mul_tv)
			.def("__mul", &transf_::mul_tt)
#endif
			.def("toGlobal",&transf::toGlobal)
			.def("inverse", &transf::inverse)
			.def("difference", &transf::difference)
			.def("toLocalRot",&transf::toLocalRot)
			.def("toGlobalRot",&transf::toGlobalRot)
			.def("toLocalDRot",&transf::toLocalDRot)
			.def("toGlobalDRot",&transf::toGlobalDRot)
			.def("toLocalPos",&transf::toLocalPos)
			.def("toGlobalPos",&transf::toGlobalPos)
			.def("toLocalDir",&transf::toLocalDir)
			.def("toGlobalDir",&transf::toGlobalDir)
			.def("assign", &transf_::assign)
			.def("encode2D", &transf::encode2D)
			.def("decode2D", &transf::decode2D)
			];
	}

	addBaselibModule2(L);



}
/*void testLuaBind()
  {
  try
  {
  LUAwrapper l;
  lua_dostring(l.L, "function add(first, second)\nprint(first,second)\nreturn 1\nend\n");

  int a=3;
  l.setVal<int>("a", a);
  vectorn b(4, 1.0, 2.0,3.0,4.0);
  l.setVal<vectorn>("b", b);
  b[2]=5;

  l.setRef<vectorn>("b_ref", b);


  struct test_raw
  {
  static void greet(lua_State* L)
  {
  lua_pushstring(L, "hello");
  }
  };

  module(l.L)
  [
  def("greet", &test_raw::greet, raw(_1))
  ];

  lua_dostring(l.L, "d=3");

  lua_dostring(l.L, "e=vectorn()\ne:assign({1,1,1,1})");

  lua_dostring(l.L, "e[0]=3\ne[1]=e[1]+1\n");


  int d;
  l.getVal<int>("d", d);
  std::cout << d;

  vectorn e;
  l.getVal<vectorn>("e", e);


  vectorn& er=l.getRef<vectorn>("e");

  er[1]=3;

  std::cout << "Result: "
  << luabind::call_function<int>(l.L, "add", "asdf", "3333");


  lua_dostring(l.L, "debug.debug()");
  }
  catch(luabind::error& e)
  {

  printf("lua error %s\n", e.what());
  }
  }


*/
