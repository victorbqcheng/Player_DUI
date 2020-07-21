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
	std::shared_ptr<CDiv> m_pDivNormal;
	std::shared_ptr<CDiv> m_pDivHover;
	std::shared_ptr<CDiv> m_pDivDown;
};

