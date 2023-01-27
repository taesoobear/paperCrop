
#include "stdafx.h"
#include "BaselibLUA.h"
#include "math/Operator.h"

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

using namespace luabind;



void addBaselibModule2(lua_State* L)
{

	// matrixn
	{
		module(L)[			
			class_<matrixn>("matrixn")
				.def(constructor<>())
				.def("value", (double (matrixn::*)( int, int))&matrixn::getValue)
				.def("setValue", (void (matrixn::*)( int, int, double))&matrixn::setValue)
				.def("rows", &matrixn::rows)
				.def("setSize", &matrixn::setSize)
				.def("range", (matrixnView (matrixn::*)(int, int, int, int))&matrixn::range)
				,class_<matrixnView, matrixn>("matrixnView")
		];

	}

	// intvectorn 
	{
		struct intvectorn_
		{
			static void assign(intvectorn & l, object const& ll) 
			{
				int count=LUAwrapper::arraySize(ll);

				l.setSize(count);
				for(int i=0; i<count; i++)
					l[i]=object_cast<int>(ll[i+1]);
			}
		};
		void (intvectorn::*setValue1)( int i, int d )=&intvectorn::setValue;
		int (intvectorn::*getValue1)( int i ) const=&intvectorn::getValue;
		intvectornView (intvectorn::*range)(int start, int end, int step)=&intvectorn::range;
		module(L)[
			class_<intvectorn>("intvectorn")
				.def("assign", &intvectorn_::assign)
				.def("value", getValue1)
				.def("get", getValue1)
				.def("set", setValue1)
				.def("size", &intvectorn::size)
				.def("setSize", &intvectorn::setSize)
				.def("resize", &intvectorn::resize)
				.def("range", range)
				.def("colon", &intvectorn::colon)
		];
	}

	// namespace sop
	module(L, "sop")[	
		def("map", &sop::map)
	];


	// vectorno
	{
		struct vectorn_wrap
		{
			static void assign(vectorn& l, object const& ll) 
			{
				int count=LUAwrapper::arraySize(ll);

				l.setSize(count);
				for(int i=0; i<count; i++)
					l[i]=object_cast<double>(ll[i+1]);	// lua indexing starts from 1.
			}

			static double __index(vectorn const& a, int i)
			{
				return a[i];
			}
			static vectorn __mul(vectorn const& a, double b)
			{
				vectorn c;
				c.mult(a,b);
				return c;
			}

			static vectorn __div(vectorn const& a, double b)
			{
				vectorn c;
				c.mult(a,1.0/b);
				return c;
			}
		};

		vectorn& (vectorn::*assignv)(const vector3& other)=&vectorn::assign;
		vectorn& (vectorn::*assignq)(const quater& other)=&vectorn::assign;
		void (vectorn::*setValue1)( int i, double d )=&vectorn::setValue;
		double (vectorn::*getValue1)( int i ) const=&vectorn::getValue;
		vectornView (vectorn::*range)(int start, int end, int step)=&vectorn::range;
		module(L)[
			class_<vectorn>("vectorn")
				.def(constructor<>())
				.def("assign", &vectorn_wrap::assign)
				.def("assignv", assignv, discard_result)
				.def("assignq", assignq, discard_result)
				.def("value", getValue1)
				.def("get", &vectorn_wrap::__index)
			.def("set", setValue1)
			.def("size", &vectorn::size)
			.def("setSize", &vectorn::setSize)
			.def("resize", &vectorn::resize)
			.def("range", range)
			.def("length", &vectorn::length)
			.def("colon", &vectorn::colon)
			.def("uniform", &vectorn::uniform)
			.def("linspace", &vectorn::linspace)
			//.def("normalize", &vectorn::normalize, return_value_policy<reference_existing_object>())
			.def("negate", &vectorn::negate)
			.def(-self) // neg (unary minus)
			.def(self + self) // add (homogeneous)
			.def(self * self) // mul
			.def("__mul", &vectorn_wrap::__mul)
			.def("__div", &vectorn_wrap::__div)
		];
	}


}

