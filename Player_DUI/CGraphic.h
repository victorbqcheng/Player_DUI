#pragma once

#include <windows.h>
#include <string>

namespace Corona
{
	class Color
	{
	public:
		Color(BYTE a, BYTE r, BYTE g, BYTE b);
		Color(BYTE r, BYTE g, BYTE b);
		Color(unsigned int color);
		BYTE a;
		BYTE r;
		BYTE g;
		BYTE b;
	};
	class Point
	{
	public:
		int x;
		int y;
	};
	class Rect
	{
	public:
		int x;
		int y;
		int width;
		int height;
	};
	class CGraphic
	{
	public:
		CGraphic();
		virtual ~CGraphic();
		void attach(HWND wnd);
		void detach();
		virtual void draw_line(Point pt1, Point pt2);
		virtual void draw_rectangle(int x, int y, int width, int height);
		virtual void fill_rectangle(int x, int y, int width, int height);
		virtual void draw_string(std::string str);

	private:
		HWND m_wnd;
	};
}



