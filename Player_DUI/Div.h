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

#include "Types.h"

class CDiv;
class CUIMgr;

enum ALIGNMENT
{
	ALIGNMENT_NEAR,
	ALIGNMENT_CENTER,
	ALIGNMENT_FAR,
};

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


class CDiv
{
	friend class CUIMgr;
public:
	//遍历方向：DIR_DOWN-遍历子元素; DIR_UP-遍历父元素
	enum DIRECTION
	{
		DIR_DOWN,
		DIR_UP,
	};
	typedef std::function<void(CMouseEvent e)> EVENT_CALLBACK_WRAPPER;
	typedef EVENT_CALLBACK_WRAPPER MOUSEDOWN_CALLBACK_WRAPPER;
	typedef EVENT_CALLBACK_WRAPPER MOUSEUP_CALLBACK_WRAPPER;
	typedef EVENT_CALLBACK_WRAPPER LBUTTON_DB_CLICK_CALLBACK_WRAPPER;
	typedef EVENT_CALLBACK_WRAPPER CLICK_CALLBACK_WRAPPER;
	typedef EVENT_CALLBACK_WRAPPER MOUSEMOVE_CALLBACK_WRAPPER;
	typedef EVENT_CALLBACK_WRAPPER MOUSELEAVE_CALLBACK_WRAPPER;
	
public:
	CDiv(std::string strID);
public:
	virtual ~CDiv(void);
	std::string const& getID();
	CUIMgr* getUIMgr();

	void setPosition(int nLeft, int nTop);
	POINT getPosition();
	POINT getAbsPosition();

	void setWidth(int nWidth);
	int getWidth();
	
	virtual void setHeight(int nHeight);
	int getHeight();
	
	void setVisible(bool bVisible);
	bool isVisible();
	
	void setZIndex(int nZIndex);
	int getZIndex();
	
	void setClip(bool bClip);
	bool isClip();

	void setTransparent(bool bTrans);
	bool isTransparent();
	
	void addChild(CDiv* pDivChild);
	CDiv* getChildByID(std::string const& strID);
	void setParent(CDiv* pDivParent);

	void setDraggable(bool b);
	bool isDraggable();

	void setShowFrame(bool bShowFrame);

	void setText(std::string const& strText);
	void setFontName(std::string strFontName);
	void setFontSize(int nFontSize);
	void setAlignment(ALIGNMENT h_align=ALIGNMENT_NEAR, ALIGNMENT v_align=ALIGNMENT_NEAR);
	void setTextColor(Gdiplus::Color color);

	void setBorderColor(Gdiplus::Color color);
	void setBorderWidth(int nWidth);
	//
	void setBackgroundColor(Gdiplus::Color color);
	
	void setBackgroundImage(std::wstring fileName, Gdiplus::Rect srcRc);
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
	virtual void onPaint(Gdiplus::Graphics& g);
	//return value:true--msg has been handled, do not need to proceed anymore 
	virtual bool onLButtonDown(int x, int y);
	virtual bool onLButtonUp(int x, int y);
	virtual bool onLButtonDbClick(int x, int y);
	virtual bool onMouseMove(int x, int y);
	virtual bool onMouseLeave();
protected:
	bool hitTest(int x, int y);
	void setUIMgr(CUIMgr* pUIMgr);
	HFONT myCreateFont(std::string strFontName, int nFontSize);
protected:
	std::string m_strID;
	CUIMgr* m_pUIMgr;
	CDiv* m_pDivParent;
	//位置
	int m_nLeftRelative;
	int m_nTopRelative;
	//大小
	int m_nWidth;
	int m_nHeight;
	//
	int m_nZIndex;
	//可见
	bool m_bVisible = true;
	//剪裁
	bool m_bClip;
	//背景
	Gdiplus::Color m_backgroundColor2;
	Gdiplus::SolidBrush m_backgroundBrush;
	ImgSprite m_bkImgSprite;
	char* m_bkImgData = NULL;
	int m_nBkImgDataWidth = 0;
	int m_nBkImgDataHeight = 0;
	//边框
	Gdiplus::Pen m_pen;
	//文字
	Gdiplus::StringFormat m_strFormat;
	Gdiplus::SolidBrush m_textBrush;
	std::string m_strFontName;
	int m_nFontSize;
	std::string m_strText;

	bool m_bDraggable = false;
	//
	bool m_bTransparent = false;	//true:鼠标消息可以穿透
	//
	bool m_bFocus;
	//
	bool m_bMouseMove;
	//子元素
	std::map<int, std::deque<CDiv*> > m_children;	//z-index -- children
	//
	bool m_bShowFrame = true;		//是否显示区域边框
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
