#pragma once
#include "Div.h"

class CDUIButton : public CDiv
{
public:
	CDUIButton(std::string id);
	virtual ~CDUIButton()
	{
	}
	void setButtonWidth(int nWidth);
	void setButtonHeight(int nHeight);
	void setBackgroundImage(std::wstring normal, Corona::Rect rcNormal,
		std::wstring hover, Corona::Rect rcHover,
		std::wstring down, Corona::Rect rcDown);
	void mouseMoveCb(CMouseEvent e);
	void mouseLeaveCb(CMouseEvent e);
private:
	CDiv* m_pDivNormal;
	CDiv* m_pDivHover;
	CDiv* m_pDivDown;
};

