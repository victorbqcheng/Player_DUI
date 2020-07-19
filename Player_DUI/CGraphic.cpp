#include "pch.h"
#include "CGraphic.h"
namespace Corona
{
	Color::Color(BYTE a, BYTE r, BYTE g, BYTE b)
	{
		this->a = a;
		this->r = r;
		this->g = g;
		this->b = b;
	}

	Color::Color(BYTE r, BYTE g, BYTE b)
	{
		Color(255, r, g, b);
	}

	Color::Color(unsigned int color)
	{
		memcpy(this, &color, sizeof(color));
	}

	CGraphic::CGraphic()
	{
	}

	CGraphic::~CGraphic()
	{
	}

	void CGraphic::attach(HWND wnd)
	{
		m_wnd = wnd;
	}

	void CGraphic::detach()
	{

	}

	void CGraphic::draw_line(Point pt1, Point pt2)
	{

	}

	void CGraphic::draw_rectangle(int x, int y, int width, int height)
	{

	}

	void CGraphic::fill_rectangle(int x, int y, int width, int height)
	{

	}

	void CGraphic::draw_string(std::string str)
	{

	}

}

