/***************************************************************************
Author: victor cheng
**************************************************************************/
#pragma once

#include <Windows.h>
#include <WinGDI.h>

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <gdiplus.h>
#include <functional>

#include "CGraphic.h"

class CDiv;
class CUIMgr;

using DivPtr = std::shared_ptr<CDiv>;

class CMouseEvent
{
public:
	CDiv* pTarget;
	int nMouseX;
	int nMouseY;
};

typedef void(*EVENT_CALLBACK)(CMouseEvent e);
typedef EVENT_CALLBACK MOUSEDOWN;
typedef EVENT_CALLBACK MOUSEUP;
typedef EVENT_CALLBACK CLICK;
typedef EVENT_CALLBACK MOUSEMOVE;
typedef EVENT_CALLBACK MOUSELEAVE;

typedef std::function<void(CMouseEvent e)> EVENT_CALLBACK_WRAPPER;
typedef EVENT_CALLBACK_WRAPPER MOUSEDOWN_CALLBACK_WRAPPER;
typedef EVENT_CALLBACK_WRAPPER MOUSEUP_CALLBACK_WRAPPER;
typedef EVENT_CALLBACK_WRAPPER LBUTTON_DB_CLICK_CALLBACK_WRAPPER;
typedef EVENT_CALLBACK_WRAPPER CLICK_CALLBACK_WRAPPER;
typedef EVENT_CALLBACK_WRAPPER MOUSEMOVE_CALLBACK_WRAPPER;
typedef EVENT_CALLBACK_WRAPPER MOUSELEAVE_CALLBACK_WRAPPER;

struct Style{ 
	//位置
	int m_nLeftRelative = 0;
	int m_nTopRelative = 0;
	//大小
	int m_nWidth = -1;
	int m_nHeight = -1;
	//可见
	bool m_bVisible = true;
	//剪裁
	bool m_bClip = false;
	//背景
	Corona::Color m_backgroundColor;
	Corona::ImageSprite m_bkImgSprite;
	char* m_bkImgData = NULL;
	int m_nBkImgDataWidth = 0;
	int m_nBkImgDataHeight = 0;
	//边框
	Corona::Color m_borderColor;
	int m_borderWidth = 1;
	//文字
	Corona::StringFormat m_strFormat;
	Corona::Color m_textColor;
	Corona::Font m_font;
	std::wstring m_strText;
};

enum DIV_STATE
{
	STATE_NORMAL,
	STATE_HOVER
};

class CDiv
{
	friend class CUIMgr;
public:
	CDiv(std::string strID);
	virtual ~CDiv(void);
public:
	static DivPtr build(std::string const& id);
	void setID(std::string const& id);
	DivPtr clone(DivPtr pParent);
	std::string const& getID();
	CUIMgr* getUIMgr();
	void setLeft(int left, DIV_STATE=STATE_NORMAL);
	void setTop(int top, DIV_STATE = STATE_NORMAL);
	void setPosition(int nLeft, int nTop, DIV_STATE = STATE_NORMAL);
	POINT getPosition();
	POINT getAbsPosition();

	void setWidth(int nWidth, DIV_STATE = STATE_NORMAL);
	int getWidth();
	
	virtual void setHeight(int nHeight, DIV_STATE = STATE_NORMAL);
	int getHeight();
	
	void setVisible(bool bVisible);
	bool isVisible();
	
	void setZIndex(int nZIndex);
	int getZIndex();
	
	void setClip(bool bClip, DIV_STATE = STATE_NORMAL);
	bool isClip();

	void setTransparent(bool bTrans);
	bool isTransparent();
	
	void addChild(DivPtr pDiv);
	DivPtr getChildByID(std::string const& strID);
	void setParent(CDiv* pDivParent);

	void setDraggable(bool b);
	bool isDraggable();

	void setText(std::wstring const& strText, DIV_STATE state = STATE_NORMAL);
	void setFontName(std::wstring const& strFontName, DIV_STATE state = STATE_NORMAL);
	void setFontSize(int nFontSize, DIV_STATE state = STATE_NORMAL);
	void setAlignment(Corona::ALIGNMENT h_align= Corona::ALIGNMENT_NEAR, Corona::ALIGNMENT v_align= Corona::ALIGNMENT_NEAR, DIV_STATE state = STATE_NORMAL);
	void setTextColor(Corona::Color color, DIV_STATE state = STATE_NORMAL);

	void setBorderColor(Corona::Color color, DIV_STATE state = STATE_NORMAL);
	void setBorderWidth(int nWidth, DIV_STATE state = STATE_NORMAL);
	//
	void setBackgroundColor(Corona::Color color, DIV_STATE state = STATE_NORMAL);
	void setBackgroundImage(std::wstring const& fileName, Corona::Rect const& srcRc, DIV_STATE state = STATE_NORMAL);
	void setBackgroundImage(char* data, int width, int height);	//data:RGB数据.播放器使用
	
	//message callback
	void setMouseDownCb(MOUSEDOWN md);
	void setMouseDownCb(MOUSEDOWN_CALLBACK_WRAPPER cb);
	void setMouseUpCb(MOUSEUP mu);
	void setMouseUpCb(MOUSEUP_CALLBACK_WRAPPER cb);
	void setClickCb(CLICK click);
	void setClickCb(CLICK_CALLBACK_WRAPPER cb);
	void setLButtonDbClickCb(LBUTTON_DB_CLICK_CALLBACK_WRAPPER cb);
	void setMouseMoveCb(MOUSEMOVE mm);
	void setMouseMoveCb(MOUSEMOVE_CALLBACK_WRAPPER cb);
	void setMouseLeaveCb(MOUSELEAVE ml);
	void setMouseLeaveCb(MOUSELEAVE_CALLBACK_WRAPPER cb);
private:
	//message handler
	virtual void onPaint(Corona::CGraphic& g);
	//return value:true--msg has been handled, do not need to proceed anymore 
	virtual bool onLButtonDown(int x, int y);
	virtual bool onLButtonUp(int x, int y);
	virtual bool onLButtonDbClick(int x, int y);
	virtual bool onMouseMove(int x, int y);
	virtual bool onMouseLeave();
protected:
	bool hitTest(int x, int y);
	void setUIMgr(CUIMgr* pUIMgr);
	Corona::Rect calcChldrenBounds();
	HFONT myCreateFont(std::string strFontName, int nFontSize);
protected:
	std::string m_strID;
	CUIMgr* m_pUIMgr;
	CDiv* m_pDivParent;
	int m_nZIndex;
	bool m_bDraggable = false;
	//
	bool m_bTransparent = false;	//true:鼠标消息可以穿透
	//
	Style m_styleNormal;
	Style m_styleHover;
	//
	bool m_bFocus;
	//
	bool m_bMouseMove;
	//子元素
	std::map<int, std::deque<DivPtr>> m_children;	//z-index -- children
	//callback functions
	MOUSEDOWN m_mousedown = NULL;
	EVENT_CALLBACK_WRAPPER m_mdcbWrapper;
	MOUSEUP m_mouseup = NULL;
	EVENT_CALLBACK_WRAPPER m_mucbWrapper;
	CLICK m_click = NULL;
	CLICK_CALLBACK_WRAPPER m_clickWrapper;
	LBUTTON_DB_CLICK_CALLBACK_WRAPPER m_lbuttonDbClickWrapper;
	MOUSEMOVE m_mm = NULL;
	MOUSEMOVE_CALLBACK_WRAPPER m_mmcbWrapper;
	MOUSELEAVE m_ml = NULL;
	MOUSELEAVE_CALLBACK_WRAPPER m_mlcbWrapper;
};



