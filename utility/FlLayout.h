#ifndef FL_LAYOUT_H_
#define FL_LAYOUT_H_

#pragma once

#include "../WrapperLua/OR_LUA_Stack.h"
#include "luabindWorker.h"

class FlLayout;
class FlLayoutGroup : public Fl_Group
{
	std::vector<FlLayout*> layouts;
	int mCurrLayout;
public:
	int numLayouts();
	FlLayout* layout(int ilayout=0);
	void showLayout(int ilayout);

	FlLayoutGroup (int x, int y, int w, int h, const char* l=0);
	FlLayoutGroup (int x, int y, int w, int h, FlLayout* layout, const char* l=0);
	FlLayoutGroup (int x, int y, int w, int h, std::vector<FlLayout*> const& layouts, const char* l=0);

	virtual void resize(int x,int y,int w,int h);

	void updateLayout();
	int minimumHeight();
};

class Fl_Scrollbar;
class FlChoice;
#include <map>
#include "namedmapsupport.h"
class FlLayout  : public Fl_Double_Window, public OR::LUAWrap::Worker
{
public:

	// buttontype, sliderType -> deprecated
	enum buttonType {BUTTON, CHECK_BUTTON, LIGHT_BUTTON};
	enum sliderType {SLIDER, VALUE_SLIDER, VALUE_VERT_SLIDER};

	FlLayout ();
	FlLayout (int w, int h, const char* title=NULL);
	FlLayout (int x, int y, int w, int h, const char* title=NULL);
	~FlLayout (void);

	/***********************************************************************
	usage

	TwoWin::TwoWin(int x, int y, int w, int h)
	: FlLayout  (x,y,w,h) ,
	{
		create("Button", "btn:attack1", "Attack 1");
		createSlider("Slider", "slider1", "Attack 1");
		slider(0).value(0,1);
		updateLayout();
	}

	TwoWin::onCallback(FlLayout::Widget const& w, Fl_Widget * pWidget, int userData)
	{
		if(w.mId=="btn:attack1")
			doSomething();
	}
	***********************************************************************/


	/*********************************************************************
		LUA��ũ��Ʈ �ȿ��� ���Ǵ� ����� ����.- deprecated. (luabind������� �籸�� ��)
		c++�ڵ�:
			OR::LUAWrap w;
			w.setWorker("ui", *this);	// ui��� �ƹ��̸��̳� ����� �� ����.
			w.dofile("changeui.lua");
		lua�ڵ�:
			ui:work("checkButton", "Attack 1", true)	// ������ ����� �̸�:work(...)
			ui:work("menu", "method", 3)
			...
	**********************************************************************/
	virtual int work(TString const& workname, OR::LUAStack& L);
	virtual void __call(const char* wn, luabind::adl::object const& table);

	// type-> FL_�� ������ Ŭ���� �̸�. ex) Input, Box, Adjuster, Check_Button, Menu, Choice, Slider, Frame_Adjuster, Layout...
	Fl_Widget* create(const char* type, const char* id, const char* title=0);

	// height�� 0�̸� widget�� default height�� ����Ѵ�.
	Fl_Widget* create(const char* type, const char* id, const char* title, int startSlot, int endSlot=INT_MAX, int height=0);

	void removeWidgets(int startWidgetIndex);

	void embedLayout(FlLayout* childLayout, const char* id, const char* title=0);
	void embedLayouts(std::vector<FlLayout*> const& childLayouts,const char* id, const char* title=0);

	void newLine(); // �����ٷ� �ű��. (����Ʈ widgetPosition������ �ڵ����� �Ѿ�� call�����־ ��.)

	// setState  (���Ŀ� �����Ǵ� ��� ������ ���� �ֱ��� setState�� ������ �޴´�.)
	void setLineSpace(int l=5);
	// space between two horizontally consecutive widgets
	void setHorizSpace(int h=5);

	void setWidgetHeight(int h=20);
	void setUniformGuidelines(int totalSlot); // ���η� totalSlot����Ѵ�.
	void setWidgetPos(int startSlot, int endSlot=INT_MAX); // guideline ���� ���������ִ� �������� ��ŭ ��������.
	void setWidgetPosUniform(int totalSlot, int slot);	// ���η� totalSlot����� �ؼ�, ���°�� �־��.
	void resetToDefault()	{setUniformGuidelines(3); setWidgetPos(0); setWidgetHeight(20); setLineSpace(5); }
	vectorn& guideLines();

	// updateLayout�� �������� �ݵ�� ���־���Ѵ�.
	void updateLayout();

	// scrollbar�� ������ �ʴ� minimum�� height�� return�Ѵ�.
	int minimumHeight();

	class Widget
	{
	public:
		Widget();
		TString mId;
		TString mType;

		Fl_Widget* widgetRaw() const {return mWidget;}
		template <class Fl_Widget_Type> Fl_Widget_Type* widget() const {return dynamic_cast<Fl_Widget_Type*>(widgetRaw());}

		// type casting utility
		Fl_Button* button() const;				//!< ��� ��ư�� ���.
		Fl_Valuator* valuator() const;			//!< slider�� adjuster�� ���.
		Fl_Slider* slider() const;
		Fl_Light_Button* checkButton() const;	//!< lightButton�̳� checkButton�� ���
		FlChoice * menu() const;				//!< choice�� menu�� ���
		FlLayout* layout() const;
		FlLayoutGroup*layoutGroup() const;
	private:
		Fl_Widget* mWidget;

		// additional data
		union
		{
			int mValue;
		};

		struct State
		{
			// state variable
			buttonType mButtonType;	// �� widget�� type
			sliderType mSliderType;	// �� widget�� type

			vectorn mGuideLines; // 0���� 1����. ���õ� ������. ���̵���� [0,1], [1,2], [2,3].. ���� widget�� ������ �����̵ȴ�.
			int mStartSlot, mEndSlot;	// ���� slot�� �� slot �ѹ�. mRight==�ѽ�����(mGuideLines.size()-1)�̸� �ٹٲٱ��.
			int mWidgetHeight;
			int mLineSpace;	// �� widget���� ����� (default=5).
			int mHorizSpace;
		};

		State mState;
		friend class FlLayout;
	};

	/// you can distinguish the caller based on w.mId, pWidget, pWidget->label() or userData.
	virtual void onCallback(FlLayout::Widget const& w, Fl_Widget * pWidget, int userData);

	// n �� 0�϶� ���������� ������ ����. 1�϶� ������ ������ ����, -1�϶� ������ �ι�° ����, ...
	Widget& widgetRaw(int n);
	int widgetIndex(const char* id);	//!< ��ư 10���� �������� ù��° ��ư�� index�� �޾ƿ���, �� ������ ��ư�� index+1, index+2.������ ���� �����ϴ�.
	Widget& findWidget(const char* id);

	template <class Fl_Widget> Fl_Widget* widget(int n)
	{
		return dynamic_cast<Fl_Widget*>(widgetRaw(n).widgetRaw());
	}

	template <class Fl_Widget> Fl_Widget* find(const char* id)
	{
		return dynamic_cast<Fl_Widget*>(findWidget(id).widgetRaw());
	}

	FlLayout* layout(int n);
	FlLayoutGroup* layoutGroup(int n);
	FlLayout* findLayout(const char* id);
	FlLayoutGroup* findLayoutGroup(const char* id);

	FlChoice * menu(int n);
	FlChoice * findMenu(const char* id);

	// utility functions :  findSlider(id)==find<Fl_Slider> (id), slider(0)==widgetT<Fl_Slider>(0)
	Fl_Slider* slider(int n);
	Fl_Slider* findSlider(const char* id);

	Fl_Valuator* valuator(int n);
	Fl_Valuator* findValuator(const char* id);

	Fl_Button* button(int n);
	Fl_Button* findButton(const char* id);

	// check button�̳� light button�� ã���ش�.
	Fl_Light_Button* checkButton(int n);
	Fl_Light_Button* findCheckButton(const char* id);

	void callCallbackFunction(Widget& w);

	virtual void resize(int x,int y,int w,int h);

	///////////////////////////////////////////////////////////////////
	//  �Ʒ� �Լ����� ��� deprecated
	//
	//  use create(..) instead of createWidget(..)
	///////////////////////////////////////////////////////////////////
	Fl_Widget* createWidget(const char* type, const char* id, const char* title=0) { return create(type, id, title);}
	FlChoice * createMenu(const char* id, const char* title=0);
	Fl_Button* createButton(const char* id, const char* title=0);
	Fl_Slider* createSlider(const char* id, const char* title=0);

	// createButton, createSlider�� default class�� �����ش�.
	void setButtonClass(buttonType b);
	void setSliderClass(sliderType s);

private:
	int m_minimumHeight;
	Fl_Window* mWindow;
	Fl_Scrollbar* mScrollbar;
	std::vector<Widget> mWidgets;
	std::map<TString, int, cmpTString> mNamedmapWidgets;
	FlLayout* _callbackRouter;
//	friend class FlLayout;

	static void cbFunc(Fl_Widget * pWidget, void *data);
	Fl_Widget* _createWidget(const char* id, Fl_Widget* o);
	Widget& _findWidget(Fl_Widget* o);
};
#endif
