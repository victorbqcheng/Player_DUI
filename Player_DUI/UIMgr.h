/***************************************************************************
Author: victor cheng 2019
**************************************************************************/
#pragma once

#include <vector>
#include <map>
#include <deque>
#include <string>
#include <gdiplus.h>
#include "Div.h"
#include "CGraphic.h"

class CDiv;
class CUIMgr;

#import <msxml3.dll> rename("value","value_xml")
#define DOMNode MSXML2::IXMLDOMNodePtr
#define DOMNodes MSXML2::IXMLDOMNodeListPtr
#define DOMNodeAttrs MSXML2::IXMLDOMNamedNodeMapPtr

class CUIMgr
{
private:
	//CUIMgr(void);
public:
	typedef void (CUIMgr::*PARSE_ATTR_METHOD)(std::shared_ptr<CDiv> pDiv, DOMNodeAttrs const& attrs);
	CUIMgr(int nID);
	~CUIMgr(void);
	//static CUIMgr& Instance();
	
public:
	static DivPtr buildDiv(std::string const& id);
	template<typename Element>
	static auto buildElement(std::string const& id);
	void addElement(DivPtr pElement);
	CDiv* getElementByID(std::string strID);
	HWND getHwndContainer();
	bool hookWndMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool parseResource(std::string strFileName);
	bool parseResource(char const* buf);
	bool parseResource(int nResID);
	void registerClickHandler(std::string strHandlerName, CLICK ck);

private:
	void attach(HWND hWnd);
	void detach();
	DivPtr parseDiv(DOMNode);
	void onLButtonDown(int x, int y);
	void onLButtonUp(int x, int y);
	void onLButtonDbClick(int x, int y);
	void onMouseMove(int x, int y);
	void onMouseLeave();
	void render();

	void parseAttr_left(DivPtr pDiv, DOMNodeAttrs const& attrs);
	void parseAttr_width(DivPtr pDiv, DOMNodeAttrs const& attrs);
	void parseAttr_height(DivPtr pDiv, DOMNodeAttrs const& attrs);
	void parseAttr_bkcolor(DivPtr pDiv, DOMNodeAttrs const& attrs);
	void parseAttr_borderWidth(DivPtr pDiv, DOMNodeAttrs const& attrs);
	void parseAttr_borderColor(DivPtr pDiv, DOMNodeAttrs const& attrs);
	void parseAttr_fontName(DivPtr pDiv, DOMNodeAttrs const& attrs);
	void parseAttr_fontSize(DivPtr pDiv, DOMNodeAttrs const& attrs);
	void parseAttr_text(DivPtr pDiv, DOMNodeAttrs const& attrs);
	void parseAttr_textColor(DivPtr pDiv, DOMNodeAttrs const& attrs);
	void parseAttr_onclick(DivPtr pDiv, DOMNodeAttrs const& attrs);
	//TODO:
	void parseAttr_textFormat(DivPtr pDiv, DOMNodeAttrs const& attrs);
private:
	HWND m_hWndContainer;
	HDC m_hDCMem = NULL;
	HBITMAP m_hBmpMem = NULL;
	int m_nTimerID;
	std::map<std::string, std::shared_ptr<CDiv>> m_elements;	//all children:mapped by id
	std::map<int, std::deque<std::shared_ptr<CDiv>> > m_elements2;	//all children in z-index
	std::vector<PARSE_ATTR_METHOD> m_vecParseAttrMethods;
	std::map<std::string, CLICK> m_mapClickHandler;

	Corona::CGraphic m_graphic;
};

template<typename Element>
auto CUIMgr::buildElement(std::string const& id)
{
	auto p = std::make_shared<Element>(id);
	return p;
}
