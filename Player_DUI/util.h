#pragma once

#include <string>
#include <thread>
class util
{

public:
	static void log(std::string const& str);
	static void thread_sleep(int milsec);
	static void av_packet_releaser(AVPacket* p_packet);
	static void av_frame_releaser(AVFrame* p_frm);
	static std::string wstr_2_str(const std::wstring& ws);
	static std::wstring str_2_wstr(const std::string& s);
};

#define SAFE_DELETE(p)\
if(p!=NULL)\
{\
	delete p;\
	p = NULL;\
}

template<typename type1, typename type2>
class SafeLeave
{
public:
	SafeLeave(std::function<type1> f1, std::function<type2> f2)
	{
		this->f2 = f2;
		f1();
	}
	~SafeLeave()
	{
		f2();
	}
private:
	std::function<void()> f2;
};
