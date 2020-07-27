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
#include "CParseXml.h"

//class CDiv;
class CUIMgr
{
private:
	//CUIMgr(void);
public:
	CUIMgr(int nID);
	~CUIMgr(void);
	//static CUIMgr& Instance();
	
public:
	static DivPtr buildDiv(std::string const& id);
	template<typename Element>
	static auto buildElement(std::string const& id);

	void setRoot(DivPtr pRoot);
	DivPtr getElementByID(std::string strID);
	HWND getHwndContainer();
	bool hookWndMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool parseResource2(std::string fileName);
	void registerClickHandler(std::string strHandlerName, EVENT_CALLBACK_WRAPPER ck);
private:
	void attach(HWND hWnd);
	void detach();
	void onLButtonDown(int x, int y);
	void onLButtonUp(int x, int y);
	void onLButtonDbClick(int x, int y);
	void onMouseMove(int x, int y);
	void onMouseLeave();
	void render();
private:
	HWND m_hWndContainer;
	int m_nTimerID;
	std::map<int, std::deque<std::shared_ptr<CDiv>> > m_elements2;	//all children in z-index
	std::vector<PARSE_ATTR_METHOD> m_vecParseAttrMethods;
	std::map<std::string, EVENT_CALLBACK_WRAPPER> m_mapClickHandler;

	Corona::CGraphic m_graphic;
	DivPtr m_rootDiv;
	CParseXml m_parseXML;
};

template<typename Element>
auto CUIMgr::buildElement(std::string const& id)
{
	auto p = std::make_shared<Element>(id);
	return p;
}



