array.pushBack(gen_lua.number_types, 'm_real') -- m_real is a type-def of double.
array.pushBack(gen_lua.enum_types, 'bitvectorn::zeroCrossingMode')
array.pushBack(gen_lua.enum_types, 'uchar')
-- this top-level definition file is supposed to contain all the classes to be exported to lua.
-- Or, at least, all inherited classes should be defined in the same file with it's parent class
-- (Though, of couse, you can use "dofile" or "require" in this file. )
-- output can be seperated into multiple cpp files so that C++ compilation time is reduced.
-- ; see "generate" function below.

-- Detailed explanation: Q. what happen if not?
-- A. Lunagen assign a unique integer for 
-- each class, and uses it for type-checking and function-overloading.
-- And the unique type-id might not unique across multiple input files if they don't share the same top-level definition file (= this file).
-- if you want to use multiple input files for luna-gen,
-- consider using "dofile" or "require" in a single main input file.
-- If this constraints is not satisfied, generated code might work incorrectly (though unlikely in most cases).


-- snippets for luabind users:
-- luabind -> lunagen

--[[
luabind::def("createVRMLskin", &RE::createVRMLskin,luabind::adopt(luabind::result))
-> 
PLDPrimVRML* RE::createVRMLskin(VRMLloader*pTgtSkel, bool bDrawSkeleton) @ ;adopt=true; 


		.enum_("constants")
		[
			luabind::value("EULER",OpenHRP::DynamicsSimulator::EULER),
			luabind::value("RUNGE_KUTTA",OpenHRP::DynamicsSimulator::RUNGE_KUTTA)
		]
->

		.scope_[..] -> staticMemberFunctions={...}

		#ifndef AAA
			luabind::def("abc"...)
		#endif
		->
		void abc() @ ;ifndef=AAA;
 
]]--
bindTarget={
	classes={
		{
			name='CPixelRGB8',
			ctors={'(uchar, uchar, uchar)'},
			properties={'uchar R', 'uchar G', 'uchar B'},
		},
		{
			name='intvectorn',
			ctors=  -- constructors 
			{
				'()',
				'(int size)',
			},
			wrapperCode=
			[[
			inline static int get(intvectorn const& a, int i)
			{
				return a[i];
			}
			inline static void set(intvectorn & a, int i, int d)
			{
				a[i]=d;
			}
			inline static void radd(intvectorn & a, int b)	{a+=b;}
			inline static void rsub(intvectorn & a, int b)	{a-=b;}
			inline static void rdiv(intvectorn & a, int b) {a/=b;}
			inline static void rmult(intvectorn & a, int b) {a*=b;}
			inline static void rmult(intvectorn & a, intvectorn const&b) {for(int i=0; i<a.size(); i++) a[i]*=b[i];}
			inline static void radd(intvectorn & a, intvectorn const& b)	{a+=b;}
			inline static void rsub(intvectorn & a, intvectorn const& b)	{a-=b;}
			static int setValues(lua_State* L)
			{
				intvectorn& self=*luna_t::check(L,1);
				int n=lua_gettop(L)-1;
				self.setSize(n);
				for (int i=0;i<n; i++)
					self(i)=lua_tonumber(L,i+2);
				return 0;
			}
			]],
			staticMemberFunctions=
			{
				{'int get(intvectorn const& a, int i);'},
				{'int get(intvectorn const& a, int i);', rename='__call'},
				{'void set(intvectorn & a, int i, m_real d);'},
				[[void radd(intvectorn & a, int b);
				void rsub(intvectorn & a, int b);
				void rdiv(intvectorn & a, int b);
				void rmult(intvectorn & a, int b);
				void rmult(intvectorn & a, intvectorn const&b);
				void radd(intvectorn & a, intvectorn const& b);
				void rsub(intvectorn & a, intvectorn const& b);
				]]
			},
			memberFunctions=
			{
				[[
				TString output() @ __tostring
					int maximum() const;
					int minimum() const;
					int sum() const;
				void pushBack(int x)
				int size()
				void setSize(int)
				void resize(int)
				void setValue(int i, int d) @ set	
				intvectornView range(int start, int end, int step)
				intvectornView range(int start, int end)
				void  colon(int start, int end, int stepSize);
				void setAllValue(int v);
				]]
			},
			custumFunctionsToRegister ={'setValues'},
		},
		{
			name='TRect',
			ctors={"()","(int,int,int,int)"},
			properties={"int left", "int right", "int top", "int bottom"},
		},
		{
			name='CImage',
			ctors={"()"},
			memberFunctions={[[
			int GetWidth() const ;
			int GetHeight() const ;
			bool Load(const char* filename);
			bool Save(const char* filename);
			bool save(const char* filename, int BPP);
			bool Create(int width, int height); @ create
			void CopyFrom(CImage const& other);
			]]},
			staticMemberFunctions={[[
			void Imp::drawBox(CImage& inout, TRect const& t, int R, int G, int B);
			void Imp::sharpen(CImage& inout, double factor, int iterations);
			void Imp::contrast(CImage& inout, double factor);
			void Imp::gammaCorrect(CImage& inout, double factor); @ gamma
			void Imp::dither(CImage& inout, int levels);
			void Imp::resize(CImage& inout, int width, int height);
			void Imp::blit(CImage& out, CImage const& in, TRect const& rect_in, int x, int y);
			void Imp::concatVertical(CImage& out, CImage const& a, CImage const& b);
			void Imp::crop(CImage& out, CImage const& in, int left, int top, int right, int bottom);
			void Imp::rotateRight(CImage& other);
			void Imp::rotateLeft(CImage& other);
			]]}
		},
		{ 
			name='TStrings',
			ctors={'()'},
			wrapperCode=[[
			inline static void set(TStrings& a, int i, const char* b)
			{
				a[i]=b;
			}
			inline static void set(TStrings& a, int i)
			{
				a[i]="";
			}
			]],
			staticMemberFunctions={[[
			void set(TStrings& a, int i, const char* b)
			void set(TStrings& a, int i)
			]]},
			memberFunctions={[[
			TString operator[](int i)	@ __call
			TString data(int i)					
			TString back()						
			void init(int n);
			int size() const						
			void resize(int n)					
			void pushBack(const TString& other)
			TString prefix() const;	// 모든 문자열이 같은 prefix로 시작하는 경우 해당 prefix return.
			void trimSamePrefix(const TStrings& other);	// 모든 문자열이 같은 prefix로 시작하는 경우 잘라준다.
			int find(const char* other) const;
			]]}
		},
		{
			name='intvectornView',
			inheritsFrom='intvectorn'
		},
		{
			name='intmatrixn'
		},
		{
			name='bitvectorn',
			ctors={'()'},
			memberFunctions={[[
			void findLocalOptimum(const vectorn& signal, bitvectorn::zeroCrossingMode mode);
			void findLocalOptimum(const vectorn& signal);
			void setSize(int size);
			int size() const;
			void clearAll()							
			void setAll();
			void setValue(int nIndex, bool bit); @ set
			bool getValue(int nIndex) const	; @ __call
			void save(const char* filename);	
			void load(int size, const char* filename);
			]]},
			enums={
				{"ZC_MIN", "(int)bitvectorn::ZC_MIN"},
				{"ZC_MAX", "(int)bitvectorn::ZC_MAX"},
				{"ZC_ALL", "(int)bitvectorn::ZC_ALL"},
			}
		},
		{
			name='transf',
			wrapperCode=[[
			inline 	static void assign(transf &b, transf const& a)
			{
				b.rotation=a.rotation;
				b.translation=a.translation;
			}
			]],
			ctors={
				[[
				()
				(quater const&, vector3 const&)
				(matrix4 const&)
				]]
			},
			properties={ 'quater rotation', 'vector3 translation'},
			memberFunctions={
				[[
				transf inverse() const;
				transf toLocal(transf const& global) const;
				transf toGlobal(transf const& local) const;

				quater toLocalRot(quater const& ori) const;
				quater toGlobalRot(quater const& ori) const;
				quater toLocalDRot(quater const& ori) const;
				quater toGlobalDRot(quater const& ori) const;

				vector3 toLocalPos(vector3 const& pos) const;
				vector3 toGlobalPos(vector3 const& pos) const;
				vector3 toLocalDir(vector3 const& dir) const;
				vector3 toGlobalDir(vector3 const& dir) const;
				void difference(transf const& f1, transf const& f2);			//!< f2*f1.inv
				void identity();
				void operator=(matrix4 const& a);
				vector3 encode2D() const;
				void decode2D(vector3 const& in);
				void leftMult(const transf& a);	//!< this=a*this;
				void operator*=(const transf& a);	//!< this=this*a;
				]]
			},
			staticMemberFunctions={
				[[
				static void assign(transf &b, transf const& a)
				transf       operator* ( transf const&, transf const& );
				vector3       operator* ( transf const& , vector3 const&);
				]]
			},
		},
		{
			name='vector3N',
			ctors=
			{
				'()',
				'(int)',
			},
			memberFunctions=
			{
				[[
						vector3& row(int)
						int rows() const
						int size() const
						void setSize(int)
						void resize(int)
						vector3NView range(int,int)
						vector3NView range(int,int,int)
						void assign(vector3N const&)
						vector3& value(int) @ at
						vector3& value(int) @ __call
						void transition(vector3 const&, vector3 const&, int duration) 
						void setAllValue(vector3)
						vectornView x()
						vectornView y()
						vectornView z()
				]]
				-- ,add,sub,mul
			},
			wrapperCode=[[
					static TString __tostring(vector3N const& a)
						{
							return TString("vector3N");
						}
						static vector3 sampleRow(vector3N const& in, m_real criticalTime)
						{
							vector3 out;
							//!< 0 <=criticalTime<= numFrames()-1
							// float 0 이 정확하게 integer 0에 mapping된다.
							int a;
							float t;

							a=(int)floor(criticalTime);
							t=criticalTime-(float)a;

							if(t<0.005)
								out=in.row(a);
							else if(t>0.995)
								out=in.row(a+1);
							else
							{
								if(a<0)
									out.interpolate(t-1.0, in.row(a+1), in.row(a+2));
								else if(a+1>=in.rows())
									out.interpolate(t+1.0, in.row(a-1), in.row(a));
								else
									out.interpolate(t, in.row(a), in.row(a+1));
							}
							return out;
						}
				]],
			staticMemberFunctions=
			{
				[[
				vector3 sampleRow(vector3N const& in, m_real criticalTime)
				TString __tostring(vector3N const& a)
				]]
			},
		},
		{
			name='vector3NView', inheritsFrom='vector3N',
		},
		{
			name='quaterN',
			ctors=
			{
				'()',
				'(int)',
			},
			memberFunctions=
			{
				[[
						quater& row(int)
						int rows() const
						int size() const
						void setSize(int)
						void resize(int)
						quaterNView range(int,int)
						quaterNView range(int,int,int)
						void assign(quaterN const&)
						quater& value(int) @ at
						quater& value(int) @ __call
						void transition(quater const&, quater const&, int duration) 
						void setAllValue(quater)
				]]
				-- sampleRow
			},
			wrapperCode=[[
					static TString __tostring(quaterN const& a)
						{
							return TString("quaterN");
						}
						static quater sampleRow(quaterN const& in, m_real criticalTime)
						{
							quater out;
							//!< 0 <=criticalTime<= numFrames()-1
							// float 0 이 정확하게 integer 0에 mapping된다.
							int a;
							float t;

							a=(int)floor(criticalTime);
							t=criticalTime-(float)a;

							if(t<0.005)
								out=in.row(a);
							else if(t>0.995)
								out=in.row(a+1);
							else
							{
								if(a<0)
									out.safeSlerp(in.row(a+1), in.row(a+2), t-1.0);
								else if(a+1>=in.rows())
									out.safeSlerp(in.row(a-1), in.row(a), t+1.0);
								else
									out.safeSlerp( in.row(a), in.row(a+1),t);
							}
							return out;
						}
				]],
			staticMemberFunctions=
			{
				[[
				static quater sampleRow(quaterN const& in, m_real criticalTime)
					static TString __tostring(quaterN const& a)
				]]
			},
		},
		{
			name='quaterNView', inheritsFrom='quaterN',
		},
		{
			name='vectorn', --necessary
			ctors=  -- constructors 
			{
				'()',
				'(int size)',
			},
			wrapperCode=
			[[
			inline static m_real get(vectorn const& a, int i)
			{
				return a[i];
			}
			inline static void set(vectorn & a, int i, m_real d)
			{
				a[i]=d;
			}
			inline static void radd(vectorn & a, m_real b)	{a+=b;}
			inline static void rsub(vectorn & a, m_real b)	{a-=b;}
			inline static void rdiv(vectorn & a, m_real b) {a/=b;}
			inline static void rmult(vectorn & a, m_real b) {a*=b;}
			inline static void rmult(vectorn & a, vectorn const&b) {for(int i=0; i<a.size(); i++) a[i]*=b[i];}
			inline static void setAllValue(vectorn & a, m_real b) {a.setAllValue(b);}
			inline static void radd(vectorn & a, vectorn const& b)	{a+=b;}
			inline static void rsub(vectorn & a, vectorn const& b)	{a-=b;}
			inline static void smoothTransition(vectorn &c, m_real s, m_real e, int size)
			{
				c.setSize(size);
				for(int i=0; i<size; i++)
					{
						m_real sv=sop::map(i, 0, size-1, 0,1);
						c[i]=sop::map(sop::smoothTransition(sv), 0,1, s, e);
					}
				}
			static int setValues(lua_State* L)
			{
				vectorn& self=*luna_t::check(L,1);
				int n=lua_gettop(L)-1;
				self.setSize(n);
				for (int i=0;i<n; i++)
					self(i)=lua_tonumber(L,i+2);
				return 0;
			}
			inline static vectorn mult(m_real b, vectorn const& a)
				{
					return a*b;
				}
			]],
			staticMemberFunctions=
			{
				{'m_real get(vectorn const& a, int i);'},
				{'m_real get(vectorn const& a, int i);', rename='__call'},
				{'void set(vectorn & a, int i, m_real d);'},
				[[void radd(vectorn & a, m_real b);
				void rsub(vectorn & a, m_real b);
				void rdiv(vectorn & a, m_real b);
				void rmult(vectorn & a, m_real b);
				void rmult(vectorn & a, vectorn const&b);
				void setAllValue(vectorn & a, m_real b) ;
				void radd(vectorn & a, vectorn const& b);
				void rsub(vectorn & a, vectorn const& b);
				static void smoothTransition(vectorn &c, m_real s, m_real e, int size)
				m_real v::sample(vectorn const& in, m_real criticalTime)
				void v::interpolate(vectorn & out, m_real t, vectorn const& a, vectorn const& b) @ interpolate
				vectorn operator+( vectorn const& a, vectorn const& b);
				vectorn operator-( vectorn const& a, vectorn const& b);
				vectorn operator*( vectorn const& a, vectorn const& b );
				vectorn operator+( vectorn const& a, m_real b);
				vectorn operator-( vectorn const& a, m_real b);
				vectorn operator*( vectorn const& a, m_real b);
				vectorn operator/( vectorn const& a, m_real b);
				vectorn operator*( matrixn const& a, vectorn const& b );
				vectorn mult(m_real b, vectorn const& a) @ __mul
				]]
			},
			memberFunctions=
			{
				[[
				void sub(vectorn const& a, vectorn const& b);
				void add(vectorn const& a, vectorn const& b);
				void assign(const vector3& other);
				void assign(const quater& other);
				void assign(const vectorn &other);
				m_real minimum();
				m_real maximum();
				TString output() @ __tostring
				vector3 toVector3()	const	
				vector3 toVector3(int startIndex)	const	
				quater toQuater() const	
				quater toQuater(int startIndex) const	
				void setVec3( int start, const vector3& src)
				void setQuater( int start, const quater& src)
				inline void pushBack(double x)
				int size()
				void setSize(int)
				void resize(int)
				void setValue(int i, double d) @ set	
				vectornView range(int start, int end, int step)
				vectornView range(int start, int end)
				m_real length() const ;
				m_real minimum() const;
				m_real maximum()	const;
				m_real sum()	const;
				m_real squareSum() const;
				m_real avg() const;
				int argMin() const
				int argMax() const
				int argNearest(double) const;
				void colon(m_real start, m_real stepSize,int nSize);
				void colon(m_real start, m_real stepSize);
				void linspace(m_real x1, m_real x2, int nSize);
				void linspace(m_real x1, m_real x2);
				void uniform(m_real x1, m_real x2, int nSize);
				void uniform(m_real x1, m_real x2);
				matrixnView column() const;	// return n by 1 matrix, which can be used as L-value (reference matrix)
				matrixnView row() const;	// return 1 by n matrix, which can be used as L-value (reference matrix)
				
				]]
			},
			custumFunctionsToRegister ={'setValues'},

		},
		{
			name='vectornView', inheritsFrom='vectorn',
		},
		{
			name='matrix3',
			wrapperCode=[[
			static TString out(matrix3& l)
			{
				TString v;
				v.format(" %f %f %f\n %f %f %f\n %f %f %f\n", l._11,l._12,l._13,l._21,l._22,l._23,l._31,l._32,l._33);
				return v;
			}
			]],
			ctors=
			{
				[[
				()
				(matrix3 const&)
				]]
			},	
			properties={'double _11','double _12','double _13',
			'double _21','double _22','double _23',
			'double _31','double _32','double _33',
					},
			staticMemberFunctions=
			{
				[[
				vector3 operator*(matrix3 const& a, vector3 const& b)
				vector3 operator*(vector3 const& b, matrix3 const& a)
				matrix3 operator*(matrix3 const& a, matrix3 const& b)
				TString out(matrix3 &l) @ __tostring
				]]
			},
			memberFunctions=
			{
				[[
					void setValue( m_real a00, m_real a01, m_real a02, m_real a10, m_real a11, m_real a12, m_real a20, m_real a21, m_real a22 );
					void zero();
					void identity();
					void transpose( void );
					void negate( void );
					bool inverse(matrix3 const& a);
					void setTilde( vector3 const &v );
					void setFromQuaternion(quater const& q);
					void mult(matrix3 const& a,matrix3 const& b);
					void mult(matrix3 const& a, m_real b);
					]]
			},
		},
		{
			name='matrix4', --necessary
			ctors=  -- constructors 
			{
				[[
				()
				(const quater&, const vector3&)
				(const transf&)
				]]
			},
			properties={'double _11','double _12','double _13','double _14',
			'double _21','double _22','double _23','double _24',
			'double _31','double _32','double _33','double _34',
			'double _41','double _42','double _43','double _44',},
			wrapperCode=[[
			inline static void assign(matrix4& l, matrix4 const& m)
			{
				l=m;
			}
			static TString out(matrix4& l)
			{
				TString v;
				v.format(" %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n", l._11,l._12,l._13,l._14,l._21,l._22,l._23,l._24,l._31,l._32,l._33,l._34,l._41,l._42,l._43,l._44);
				return v;
			}
			static matrix4 inverse(matrix4 const& m)
			{
				matrix4 t;
				t.inverse(m);
				return t;
			}
			]],
			staticMemberFunctions={
				[[
				inline static void assign(matrix4& l, matrix4 const& m)
				static TString out(matrix4& l) @__tostring
static matrix4 inverse(matrix4 const& m)
				]]
			},
			memberFunctions={
				[[
				void identity()	
				void setRotation(const quater& q);
				void setRotation(const matrix3& m);
				void setRotation(const vector3& axis, m_real angle);
				void setScaling(m_real sx, m_real sy, m_real sz);
				void leftMultRotation(const quater& b);
				void leftMultRotation(const vector3& axis, m_real angle);
				void leftMultTranslation(const vector3& vec);
				void leftMultScaling(m_real sx, m_real sy, m_real sz);
				void inverse(const matrix4& a);
				matrix4 operator*(matrix4 const& a) const
				vector3 operator*(vector3 const& a) const;
				]]
			},
		},
		{
			name='matrixn', --necessary
			ctors=  -- constructors 
			{
				'()',
				'(int rows, int cols)',
			},
			custumFunctionsToRegister ={'setValues'},
			wrapperCode=
			[[
			static int setValues(lua_State* L)
			{
				matrixn& self=*luna_t::check(L,1);
				int n=lua_gettop(L)-1;
				if (n!=self.cols()*self.rows()) luaL_error(L, "setValues(nrows, ncols, values...)");
				int c=2;
				for (int i=0;i<self.rows(); i++)
					for (int j=0;j<self.cols(); j++)
						self(i,j)=lua_tonumber(L,c++);
				return 0;
			}
			inline static matrixn __mul(matrixn const& a, m_real b)
			{matrixn c;c.mult(a,b);return c;}
			inline static matrixn __mul(m_real b, matrixn const& a)
			{matrixn c;c.mult(a,b);return c;}
			inline static matrixn __mul(matrixn const& a, matrixn const& b)
			{matrixn c;c.mult(a,b);return c;}

			inline static matrixn __add(matrixn const& a, m_real b)
			{matrixn c;c.add(a,b);return c;}
			inline static matrixn __add(m_real b, matrixn const& a)
			{matrixn c;c.add(a,b);return c;}

			inline static matrixn __div(matrixn const& a, m_real b)
			{
				matrixn c;
				c.mult(a,1.0/b);
				return c;
			}

			//static void radd(matrixn & a, m_real b)	{a+=b;}
			//static void rsub(matrixn & a, m_real b)	{a-=b;}
			inline static void rdiv(matrixn & a, m_real b) {a/=b;}
			inline static void rmult(matrixn & a, m_real b) {a*=b;}
			inline static void setAllValue(matrixn & a, m_real b) {a.setAllValue(b);}
			inline static void radd(matrixn & a, matrixn const& b)	{a+=b;}
			inline static void rsub(matrixn & a, matrixn const& b)	{a-=b;}

			inline static void draw(matrixn& mat, const char* filename) {mat.op0(m0::draw(filename));}
			inline static void draw2(matrixn& mat, const char* filename, float fMin, float fMax)
			{mat.op0(m0::draw(filename, fMin, fMax));}
			inline static void pushBack(matrixn& mat, const vectorn& v) { mat.pushBack(v);}

      static void drawSignals(matrixn & in, const char* filename,  bool useTheSameMinMax)
      {
	m0::drawSignals(filename,useTheSameMinMax).calc(in);
      }
			inline static void transpose(matrixn& a)
			{
				matrixn c;
				c.transpose(a);
				a=c;
			}


			inline static void sampleRow(matrixn const& in, m_real criticalTime, vectorn& out){
				out.setSize(in.cols());
				//!< 0 <=criticalTime<= numFrames()-1
				// float 0 이 정확하게 integer 0에 mapping된다.
				int a;
				float t;

				a=(int)floor(criticalTime);
				t=criticalTime-(float)a;

				if(t<0.005)
					out=in.row(a);
				else if(t>0.995)
					out=in.row(a+1);
				else {
					if(a<0)
						v2::interpolate(t-1.0).calc(out, in.row(a+1), in.row(a+2));
					else if(a+1>=in.rows())
						v2::interpolate(t+1.0).calc(out, in.row(a-1), in.row(a));
					else
						v2::interpolate(t).calc(out, in.row(a), in.row(a+1));
					}
				}
				]]
				,staticMemberFunctions=
				{
					[[
							matrixn operator-( matrixn const& a, matrixn const& b);
							matrixn operator+( matrixn const& a, matrixn const& b);
					static matrixn __mul(matrixn const& a, m_real b)
					static matrixn __mul(m_real b, matrixn const& a)
					static matrixn __mul(matrixn const& a, matrixn const& b)
					static matrixn __add(matrixn const& a, m_real b)
					static matrixn __add(m_real b, matrixn const& a)
					static matrixn __div(matrixn const& a, m_real b)
					static void rdiv(matrixn & a, m_real b) 
					static void rmult(matrixn & a, m_real b) 
					static void setAllValue(matrixn & a, m_real b) 
					static void radd(matrixn & a, matrixn const& b)	
					static void rsub(matrixn & a, matrixn const& b)	
					static void draw(matrixn& mat, const char* filename) 
					static void draw2(matrixn& mat, const char* filename, float fMin, float fMax) @ draw
					static void pushBack(matrixn& mat, const vectorn& v) 
					static void transpose(matrixn& a)
					static void sampleRow(matrixn const& in, m_real criticalTime, vectorn& out)
					static void drawSignals(matrixn & in, const char* filename,  bool useTheSameMinMax)
					void sampleRow(matrixn const& in, m_real criticalTime, vectorn& out)
					]]
				},
				memberFunctions=
				{
					[[
							double getValue(int,int) @ __call
							double getValue(int,int) @ get
							void setValue(int,int,double) @ set
							int rows()
							int cols()
							vectornView		row(int i)const	
							vectornView		column(int i)const
							vectornView		diag() const		
							void transpose(matrixn const& o)
							void assign(matrixn const& o)
							void pseudoInverse(matrixn const&)
							void setSize(int, int)
							void resize(int, int)
							TString output() @ __tostring
							void setAllValue(double)
							matrixnView range(int, int, int, int)
							double minimum()
							double maximum()
							double sum()
							void pushBack(vectorn const& o)
							void inverse(matrixn const& )
							void mult( matrixn const&, matrixn const& );
							void multABt(matrixn const& a, matrixn const& b); //!< a*b^T
							void multAtB(matrixn const& a, matrixn const& b); //!< a^T*b
							void multAtBt(matrixn const& a, matrixn const& b); //!< a^T*b^T
							void mult( matrixn const& a, double b );
					]]
				},
			},
		{
			name='matrixnView', inheritsFrom='matrixn',
			ctors={
				[[
						(const matrixn& )
						(const matrixnView&)
				]]
			},
		},
			{
				name='vector3', --necessary
				ctors=  -- constructors 
				{
					'()',
					'(double xyz)',
					'(double x, double y, double z)',
				},
				read_properties=
				{
					-- property name, lua function name
					{'x', 'getX'},
					{'y', 'getY'},
					{'z', 'getZ'},
				},
				write_properties=
				{
					{'x', 'setX'},
					{'y', 'setY'},
					{'z', 'setZ'},
				},
				wrapperCode=
				[[
				inline static double getX(vector3 const& a) { return a.x;}
				inline static double getY(vector3 const& a) { return a.y;}
				inline static double getZ(vector3 const& a) { return a.z;}
				inline static void setX(vector3 & a, m_real b) { a.x=b;}
				inline static void setY(vector3 & a, m_real b) { a.y=b;}
				inline static void setZ(vector3 & a, m_real b) { a.z=b;}
				inline static void set(vector3 & a, m_real x, m_real y, m_real z) {a.x=x; a.y=y; a.z=z;}
				inline static vector3 __mul(vector3 const& a, m_real b)
				{vector3 c;c.mult(a,b);return c;}
				inline static vector3 __mul(m_real b, vector3 const& a)
				{vector3 c;c.mult(a,b);return c;}
				inline static vector3 __div(vector3 const& a, m_real b)
				{
					vector3 c;
					c.mult(a,1.0/b);
					return c;
				}
				inline static vector3 __add(vector3 const& a, vector3 const& b)
				{vector3 c;c.add(a,b);return c;}
				inline static vector3 __sub(vector3 const& a, vector3 const& b)
				{vector3 c;c.sub(a,b);return c;}
				inline static double dotProduct(vector3 const& a, vector3 const& b)
				{
					return a%b;
				}
				// you can implement custum interface function too. (see custumFunctionsToRegister below)
					static int __tostring(lua_State* L)
					{
						vector3& self=*luna_t::check(L,1);
						lua_pushstring(L, self.output().ptr());
						return 1;
					}
					static vector3 __unm(vector3 const& a,vector3 const& a2)
					{ return a*-1;}
					]],
					staticMemberFunctions=
					{
						[[
						vector3 __mul(vector3 const& a, double b);
						vector3 __mul(double b, vector3 const& a);
						vector3 __div(vector3 const& a, double b);
						vector3 __add(vector3 const& a, vector3 const& b);
						vector3 __sub(vector3 const& a, vector3 const& b);
						double dotProduct(vector3 const& a, vector3 const& b);
						double getX(vector3 const& a);
						double getY(vector3 const& a);
						double getZ(vector3 const& a);
						void setX(vector3 & a, m_real b);
						void setY(vector3 & a, m_real b);
						void setZ(vector3 & a, m_real b);
						void set(vector3 & a, m_real ,m_real, m_real);
						void set(vector3 & a, m_real ,m_real, m_real); @ setValue
							vector3 __unm(vector3 const& a,vector3 const & a2)
						]]
					},
					custumFunctionsToRegister={'__tostring'},
					memberFunctions = -- list of strings of c++ function declarations.
					{
						-- you can enter multiline texts that looks like a cleaned header file
						[[
						void add(const vector3&, const vector3&);
						void sub(const vector3&, const vector3&);
						void zero();
						void operator*=(double);
						void operator*=(double); @ scale
						void cross(const vector3&, const vector3&);
						vector3 cross(const vector3&) const;
						m_real distance(const vector3& other) const;
						void normalize();
						void multadd(const vector3&, m_real);	
						m_real length() const;
						void interpolate( m_real, vector3 const&, vector3 const& );
						void difference(vector3 const& v1, vector3 const& v2);
						void rotate( const quater& q);
						void rotate( const quater& q, vector3 const& in);
						void rotationVector(const quater& in);
						]],
						{'void add(const vector3&);', rename='radd'},
						{'void sub(const vector3&);', rename='rsub'},
						{'void operator=(const vector3&);', rename='assign'},
					},
				},
				{
					name='quater', --necessary
					properties={'double x', 'double y', 'double z', 'double w'},
					ctors=
					{
						[[
								()
								(double,double,double,double)
								(double, vector3 const&)
						]]
					},
					wrapperCode=
					[[
								static vector3 rotate(quater const& q, vector3 const& v)
								{
									vector3 out;
									out.rotate(q,v);
									return out;
								}
								static void setRotation(quater &q, const char* aChannel, vector3 & euler)
								{
									q.setRotation(aChannel, euler);
								}
								static void getRotation(quater const&q, const char* aChannel, vector3 & euler)
								{
									q.getRotation(aChannel, euler);
								}
								static vector3 getRotation(quater const&q, const char* aChannel)
								{
									vector3 euler;
									q.getRotation(aChannel, euler);
									return euler;
								}
								]],
					staticMemberFunctions={
						[[
								static vector3 rotate(quater const& q, vector3 const& v)
								static void setRotation(quater &q, const char* aChannel, vector3 & euler)
								static void getRotation(quater const&q, const char* aChannel, vector3 & euler)
								static vector3 getRotation(quater const&q, const char* aChannel)
						]]
					},
					memberFunctions={
						[[
								void slerp( quater const&, quater const&, m_real );
								void safeSlerp( quater const&, quater const&, m_real );// align이 안되어 있어도 동작
								void interpolate( m_real, quater const&, quater const& );
								void setAxisRotation(const vector3& vecAxis, const vector3& front, const vector3& vecTarget);
								void identity()
								quater   inverse() const
								void decompose(quater& rotAxis_y, quater& offset) const;
								void decomposeTwistTimesNoTwist (const vector3& rkAxis, quater& rkTwist, quater& rkNoTwist) const;
								void decomposeNoTwistTimesTwist (const vector3& rkAxis, quater& rkNoTwist, quater& rkTwist) const;
								void difference(quater const& q1, quater const& q2);			//!< quaternion representation of "angular velocity w" : q2*inv(q1);
								void scale(m_real s);
								void mult(quater const& a, quater const& b);// cross product
								quater    operator+( quater const& b) const		
								quater    operator-( quater const& b) const		
								quater    operator-() const						
								quater    operator*( quater const& b) const		
								quater    operator*( m_real b) const			
								vector3	  operator*(vector3 const& b) const		
								quater    operator/( m_real b) const			
								m_real  length() const;
								m_real rotationAngle() const	
								m_real rotationAngleAboutAxis(const vector3& axis) const;
								vector3 rotationVector() const
								void operator=(quater const& a)
								void operator=(quater const& a) @set
								void setValue( m_real ww,m_real xx, m_real yy, m_real zz )
								void axisToAxis( const vector3& vFrom, const vector3& vTo);
								void leftMult(quater const& a)			
								void rightMult(quater const& a)			
								void setRotation(const vector3&, m_real)
								void setRotation(const vector3& )
								void setRotation(const matrix4& a)
								void normalize();
								void align(const quater& other)
								TString output(); @ __tostring
						]]
					},
				},
			},
			modules={
				{
					namespace='_G' -- global
				},
				{
					namespace='math',
					wrapperCode=
					[[
					static int frexp(lua_State* L)
					{
						double d=lua_tonumber(L,1);
						double f;
						int e;
						f=::frexp(d,&e);
						lua_pushnumber(L,f);
						lua_pushnumber(L,e);
						return 2;
					}

			static void drawSignals(const char* filename, matrixn & in)
			{
				intvectorn temp;
				m0::drawSignals a(filename,0,0,true, temp);
				a.calc(in);
			}
			static void changeChartPrecision(int precision)
			{
				Imp::ChangeChartPrecision(precision);
			}

			static void defaultPrecision()
			{
				Imp::DefaultPrecision();
			}


			static void _throwTest()
			{
				throw std::runtime_error("_throwTest");

			}
					]],
					custumFunctionsToRegister ={'frexp'},
					functions=
					{
						[[
			static void drawSignals(const char* filename, matrixn & in)
			static void changeChartPrecision(int precision)
			static void defaultPrecision()
			static void _throwTest()

	void m::LUsolve(matrixn const & A, vectorn const& b, vectorn& x) @ LUsolve
	void m::PIsolve(matrixn const & A, vectorn const& b, vectorn& x) @ PIsolve
						]]
					}

					--[[module(L , "math")
					[
					def("Hash", &Hash),
					class_<FuncWrapper>("Function")
					.def(constructor<>())
					.def("getInout", &FuncWrapper::getInout)
					.def("func", &FuncWrapper::func)
					.def("func", &FuncWrapper::func_dfunc)
					.def("info", &FuncWrapper::info),
					class_<GSLsolver>("GSLsolver")
					.def(constructor<FuncWrapper*, const char*>())
					.def("solve", &GSLsolver::solve),
					class_<CMAwrap>("CMAwrap")
					.def(constructor<vectorn const&, vectorn const&,int,int>())
					.def("testForTermination",&CMAwrap::testForTermination)
					.def("samplePopulation",&CMAwrap::samplePopulation)
					.def("numPopulation",&CMAwrap::numPopulation)
					.def("dim",&CMAwrap::dim)
					.def("getPopulation",&CMAwrap::getPopulation)
					.def("setVal",&CMAwrap::setVal)
					.def("resampleSingle",&CMAwrap::resampleSingle)
					.def("update",&CMAwrap::update)
					.def("getMean",&CMAwrap::getMean)
					.def("getBest",&CMAwrap::getBest)
					];
					]]
				}
			},
		}

		require('luna_mainlib')
	-- namespaces can be defined here or using nested name (name='test.vector3') 
	namespaces={
		util={},
		math={},
		
		-- test={
		-- 	-- these are cpp class name, not lua class name, because lua name may not be unique. 
		-- 	'TRect','CImage','intvectorn', 'intmatrixn','bitvectorn','TStrings', 'binaryFile', 'boolN','vector3','quater','vector3N','vector3NView','quaterN', 'quaterNView', 'vectorn','vectornView', 'matrixn', 'matrixnView','matrix3','matrix4','transf',
		-- 	-- nested namespaces or namespaces containing any class have to be declared here too.
		-- 	math={},
		-- },
		Ogre={}
	}

	-- definitions should be in a single lua file (Though, of couse, you can use "dofile" or "require" in this file. )
	-- output cpp files can be seperated by modules or classes so that C++ compilation time is reduced.

	function generateBaseLib()
		-- declaration
		write([[
					  class boolNView;
					  class boolN;
					  class NonuniformSpline;
					  struct CPixelRGB8;
					  struct TRect;
					  class CImage;
]])
		writeHeader(bindTarget)
		flushWritten(	'../luna_baselib.h')
		write(
		[[
		#include "stdafx.h"
		#include "image/Image.h"
		#include "image/ImageProcessor.h"
		#include "math/Operator.h"
		#include "math/matrix3.h"
		#include "utility/operatorString.h"
		]]
		)
		writeIncludeBlock(true)
		write('#include "luna_baselib.h"')
		writeDefinitions(bindTarget, 'Register_baselib') -- input bindTarget can be non-overlapping subset of entire bindTarget 
		flushWritten('../luna_baselib.cpp') -- write to cpp file only when there exist modifications -> no-recompile.
	end
	function generate() 
		buildDefinitionDB(bindTarget, bindTargetMainLib) -- input has to contain all classes to be exported.
		-- write function can be used liberally.
		generateBaseLib()
		generateMainLib()
	end
