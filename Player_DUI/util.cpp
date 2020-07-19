#include "pch.h"
#include "util.h"



void util::log(std::string str)
{
#ifdef DEBUG

#ifdef WIN32
	OutputDebugStringA(str.c_str());
#else
	std::cout << str << std::endl;
#endif

#endif
}

void util::thread_sleep(int milsec)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milsec));
}

void util::av_packet_releaser(AVPacket* p_packet)
{
// 	static int count = 0;
// 	char buf[32] = {};
// 	sprintf(buf, "count=%d\r\n", count);
// 	util::log(buf);
// 	count++;

	av_packet_unref(p_packet);
	free(p_packet);
	p_packet = NULL;
}

void util::av_frame_releaser(AVFrame* p_frm)
{
	av_frame_unref(p_frm);
	av_frame_free(&p_frm);
}

std::string util::wstr_2_str(const std::wstring& ws)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const wchar_t* wchSrc = ws.c_str();
	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	char *chDest = new char[nDestSize];
	memset(chDest, 0, nDestSize);
	wcstombs(chDest, wchSrc, nDestSize);
	std::string strResult = chDest;
	delete[]chDest;
	setlocale(LC_ALL, strLocale.c_str());
	return strResult;
}

std::wstring util::str_2_wstr(const std::string& s)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const char* chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest, chSrc, nDestSize);
	std::wstring wstrResult = wchDest;
	delete[]wchDest;
	setlocale(LC_ALL, strLocale.c_str());
	return wstrResult;
}

