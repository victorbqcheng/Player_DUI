#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <map>
#include <memory>

namespace Corona
{
	enum ALIGNMENT
	{
		ALIGNMENT_NEAR,
		ALIGNMENT_CENTER,
		ALIGNMENT_FAR,
	};
	class Color
	{
	public:
		Color(BYTE a, BYTE r, BYTE g, BYTE b);
		Color(BYTE r, BYTE g, BYTE b);
		Color(unsigned int color);
		Color();
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
		Rect(int x, int y, int w, int h);
		Rect();
		
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
	};
	enum CombineMode
	{
		CombineModeAnd,
		CombineModeOr,
	};
	class ImageSprite
	{
	public:
		std::wstring fileName;
		Rect srcRect;
	};
	class StringFormat
	{
	public:
		ALIGNMENT alignment=ALIGNMENT_NEAR;
		ALIGNMENT lineAlignment = ALIGNMENT_NEAR;
	};
	class Font
	{
	public:
		std::wstring fontName = L"Î¢ÈíÑÅºÚ";
		int fontSize = 12;
	};
	class CGraphic
	{
	public:
		CGraphic();
		virtual ~CGraphic();
		virtual void attach(HWND wnd);
		virtual void detach();
		virtual void begin();
		virtual void end();
		virtual void draw_line(Point const& pt1, Point const& pt2, Color color, int lineWidth=0);
		virtual void draw_rectangle(int x, int y, int width, int height, Color color, int borderWidth=0);
		virtual void fill_rectangle(int x, int y, int width, int height, Color color);
		//virtual void draw_string(std::string const& str, std::string const& fontName, int fontSize, Rect const& layoutRect, Color color);
		virtual void draw_string(std::wstring const& str, Font& font, Rect const& layoutRect, Color color, StringFormat& format);
		virtual void draw_image(void* rawData, Rect const& dstRect, Rect const& srcRect);
		virtual void draw_image(std::string const& fileName, Rect const& dstRect, Rect const& srcRect);
		virtual void draw_image(std::wstring const& fileName, Rect const& dstRect, Rect const& srcRect);
		virtual void set_clip(Rect const& rect, CombineMode mode=CombineModeAnd);
		virtual void save_clip();
		virtual void restore_clip();
	private:
		void reset_clip();
		std::shared_ptr<Gdiplus::Image> get_img(std::wstring const& fileName);
	private:
		HWND m_wnd;
		HDC m_hDCMem = NULL;
		HBITMAP m_hBmpMem = NULL;
		std::shared_ptr<Gdiplus::Graphics> m_graphic;
		Gdiplus::SolidBrush m_fillBrush;
		Gdiplus::Pen m_pen;
		Gdiplus::Region m_rgnClipOld;
		Gdiplus::StringFormat m_format;
		Gdiplus::SolidBrush m_textBrush;

		std::map<std::wstring, std::shared_ptr<Gdiplus::Image>> m_imgCache;
	};
	Gdiplus::Rect rect_2_gp_rect(Rect const& rect);
	Gdiplus::RectF rect_2_gp_rectf(Rect const& rect);
	Gdiplus::Color color_2_gp_color(Color color);
	Gdiplus::Point point_2_gp_point(Point const& pt);
	Gdiplus::PointF point_2_gp_pointf(Point const& pt);

	Color gp_color_2_color(Gdiplus::Color color);
}




