#include "pch.h"
#include "CGraphic.h"
#include "CTools.h"
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
		this->a = 255;
		this->r = r;
		this->g = g;
		this->b = b;
		//Color(255, r, g, b);		//error?
	}

	Color::Color(unsigned long color)
	{
		memcpy(this, &color, sizeof(color));
	}

	Color::Color()
	{
		a = r = g = b = 0;
	}

	Rect::Rect(int x, int y, int w, int h)
	{
		this->x = x;
		this->y = y;
		this->width = w;
		this->height = h;
	}

	Rect::Rect()
	{
	}

	Rect Rect::Union(Rect const& src1, Rect const& src2)
	{
		Gdiplus::Rect s1 = rect_2_gp_rect(src1);
		Gdiplus::Rect s2 = rect_2_gp_rect(src2);
		Gdiplus::Rect o;
		BOOL bRet = Gdiplus::Rect::Union(o, s1, s2);
		Rect outRect = gp_rect_2_rect(o);
		return outRect;
	}

	CGraphic::CGraphic()
		:m_fillBrush(Gdiplus::Color::Transparent),
		m_pen(Gdiplus::Color::Transparent),
		m_textBrush(Gdiplus::Color::Black)
	{
		m_format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
	}

	CGraphic::~CGraphic()
	{
		detach();
	}

	void CGraphic::attach(HWND wnd)
	{
		m_wnd = wnd;
		HDC hDC = GetDC(m_wnd);
		m_hDCMem = CreateCompatibleDC(hDC);
		{
			m_hBmpMem = CreateCompatibleBitmap(hDC, 2000, 2000);
			SelectObject(m_hDCMem, m_hBmpMem);
		}
		ReleaseDC(wnd, hDC);
	}

	void CGraphic::detach()
	{
		if (m_hBmpMem != NULL)
		{
			DeleteObject(m_hBmpMem);
			m_hBmpMem = NULL;
		}
		if (m_hDCMem != NULL)
		{
			DeleteObject(m_hDCMem);
			m_hDCMem = NULL;
		}
	}

	void CGraphic::begin()
	{
		m_graphic = std::shared_ptr<Gdiplus::Graphics>(new Gdiplus::Graphics(m_hDCMem));
		reset_clip();
	}

	void CGraphic::end()
	{
		HDC hDC = GetDC(m_wnd);
		BitBlt(hDC, 0, 0, 2000, 2000, m_hDCMem, 0, 0, SRCCOPY);
		ReleaseDC(m_wnd, hDC);
		m_graphic = NULL;
	}

	void CGraphic::draw_line(Point const& pt1, Point const& pt2, Color color, int lineWidth)
	{
		m_pen.SetColor(color_2_gp_color(color));
		m_pen.SetWidth((Gdiplus::REAL)lineWidth);
		m_graphic->DrawLine(&m_pen, point_2_gp_point(pt1), point_2_gp_point(pt2));
	}

	void CGraphic::draw_rectangle(int x, int y, int width, int height, Color color, int borderWidth)
	{
		m_pen.SetColor(color_2_gp_color(color));
		m_pen.SetWidth((Gdiplus::REAL)borderWidth);
		m_graphic->DrawRectangle(&m_pen, Gdiplus::Rect{ x, y, width, height });
	}

	void CGraphic::fill_rectangle(int x, int y, int width, int height, Color color)
	{
		m_fillBrush.SetColor(color_2_gp_color(color));
		m_graphic->FillRectangle(&m_fillBrush, Gdiplus::Rect{x, y, width, height});
	}

	void CGraphic::draw_string(std::wstring const& str, Font& font, Rect const& layoutRect, Color color, StringFormat& format)
	{
		m_format.SetAlignment(Gdiplus::StringAlignment(format.alignment));
		m_format.SetLineAlignment(Gdiplus::StringAlignment(format.lineAlignment));
		Gdiplus::Font font2(font.fontName.c_str(), (float)font.fontSize);
		m_textBrush.SetColor(color_2_gp_color(color));
		m_graphic->DrawString(str.c_str(), -1, &font2,
			rect_2_gp_rectf(layoutRect),
			&m_format, &m_textBrush);
	}

	void CGraphic::draw_image(void* rawData, Rect const& dstRect, Rect const& srcRect)
	{
		Gdiplus::Bitmap bmp(srcRect.width, srcRect.height,
			srcRect.width * 3, PixelFormat24bppRGB,
			(BYTE*)rawData);
		m_graphic->DrawImage(&bmp, rect_2_gp_rect(dstRect), 0, 0, srcRect.width, srcRect.height, Gdiplus::UnitPixel);		
	}

	void CGraphic::draw_image(std::string const& fileName, Rect const& dstRect, Rect const& srcRect)
	{
		std::wstring fileName2 = CTools::str_2_wstr(fileName);
		this->draw_image(fileName2, dstRect, srcRect);
	}

	void CGraphic::draw_image(std::wstring const& fileName, Rect const& dstRect, Rect const& srcRect)
	{
		auto img = get_img(fileName);
		if (img)
		{
			m_graphic->DrawImage(img.get(), rect_2_gp_rect(dstRect),
				srcRect.x,
				srcRect.y,
				srcRect.width,
				srcRect.height,
				Gdiplus::UnitPixel);
		}
	}

	void CGraphic::set_clip(Rect const& rect, CombineMode mode)
	{
		Gdiplus::CombineMode m;
		if (mode == CombineModeAnd)
		{
			m = Gdiplus::CombineModeIntersect;
		}
		else if (mode == CombineModeOr)
		{
			m = Gdiplus::CombineModeUnion;
		}
		m_graphic->SetClip(rect_2_gp_rect(rect), m);
	}

	void CGraphic::save_clip()
	{
		m_graphic->GetClip(&m_rgnClipOld);
	}

	void CGraphic::restore_clip()
	{
		m_graphic->SetClip(&m_rgnClipOld);
	}

	void CGraphic::reset_clip()
	{
		m_graphic->ResetClip();
	}

	std::shared_ptr<Gdiplus::Image> CGraphic::get_img(std::wstring const& fileName)
	{
		if (m_imgCache.find(fileName) != m_imgCache.end())
		{
			return m_imgCache[fileName];
		}
		else
		{
			std::shared_ptr<Gdiplus::Image> p(Gdiplus::Image::FromFile(fileName.c_str()));
			if (p)
			{
				m_imgCache[fileName] = p;
				return p;
			}
		}
		return NULL;
	}
	
	Gdiplus::Rect rect_2_gp_rect(Rect const& rect)
	{
		Gdiplus::Rect result{ rect.x,
			rect.y,
			rect.width,
			rect.height
		};
		return result;
	}

	Gdiplus::RectF rect_2_gp_rectf(Rect const& rect)
	{
		Gdiplus::RectF rectf{(Gdiplus::REAL)rect.x,
			(Gdiplus::REAL)rect.y,
			(Gdiplus::REAL)rect.width, 
			(Gdiplus::REAL)rect.height
		};
		return rectf;
	}

	Gdiplus::Color color_2_gp_color(Color color)
	{
		Gdiplus::Color result(
			color.a,
			color.r,
			color.g,
			color.b
		);
		return result;
	}

	Gdiplus::Point point_2_gp_point(Point const& pt)
	{
		Gdiplus::Point result{pt.x, pt.y};
		return result;
	}

	Gdiplus::PointF point_2_gp_pointf(Point const& pt)
	{
		Gdiplus::PointF result{
			(Gdiplus::REAL)pt.x, 
			(Gdiplus::REAL)pt.y
		};
		return result;
	}

	Color gp_color_2_color(Gdiplus::Color color)
	{
		Color result{color.GetA(), color.GetR(), color.GetG(), color.GetB()};
		return result;
	}

	Rect gp_rect_2_rect(Gdiplus::Rect const& rect)
	{
		Rect result{rect.X, rect.Y, rect.Width, rect.Height};
		return result;
	}

	std::tuple<int, int> Image::get_image_size(std::wstring fileName)
	{
		Gdiplus::Image* img = Gdiplus::Image::FromFile(fileName.c_str());
		int w = img->GetWidth();
		int h = img->GetHeight();
		delete img;
		return std::make_tuple(w, h);
	}

}

