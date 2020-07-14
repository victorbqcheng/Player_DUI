#pragma once
#include "Div.h"



class CDUIButton : public CDiv
{
public:
	CDUIButton(std::string id);
	~CDUIButton()
	{
	}
	void setButtonWidth(int nWidth);
	void setButtonHeight(int nHeight);
	void setBackgroundImage(std::wstring normal, Gdiplus::Rect rcNormal, 
		std::wstring hover, Gdiplus::Rect rcHover,
		std::wstring down, Gdiplus::Rect rcDown);
	void mouseMoveCb(CMouseEvent e);
	void mouseLeaveCb(CMouseEvent e);
private:
	CDiv* m_pDivNormal;
	CDiv* m_pDivHover;
	CDiv* m_pDivDown;
};

