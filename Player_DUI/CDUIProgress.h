#pragma once
#include "Div.h"
class CDUIProgress : public CDiv
{
public:
	CDUIProgress(std::string id);
	virtual ~CDUIProgress();
	void setPercentage(double percentage);
	void setPercentageColor(Corona::Color clr);
	virtual void setHeight(int nHeight, DIV_STATE state=STATE_NORMAL) override;
	void setHandleLen(int n);
private:
	std::shared_ptr<CDiv> m_pDivCurrentProgress = NULL;
	std::shared_ptr<CDiv> m_pDivHandle = NULL;
	double m_percentage = 0.0;
	int m_nDefaultHeight = 5;
	int m_handleLen = m_nDefaultHeight;
};

