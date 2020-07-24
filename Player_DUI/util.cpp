#include "pch.h"
#include "util.h"
#include "CTools.h"

void util::log(std::string const& str)
{
#ifdef DEBUG

#ifdef WIN32
	OutputDebugStringA(str.c_str());
#else
	std::cout << str << std::endl;
#endif

#endif
}

void util::av_packet_releaser(AVPacket* p_packet)
{
	av_packet_unref(p_packet);
	free(p_packet);
	p_packet = NULL;
}

void util::av_frame_releaser(AVFrame* p_frm)
{
	av_frame_unref(p_frm);
	av_frame_free(&p_frm);
}

void util::av_dict_2_map(AVDictionary* dict, METADATA_P& m)
{
	AVDictionaryEntry* e = NULL;
	std::string key, value = "";
	while ((e = av_dict_get(dict, "", e, AV_DICT_IGNORE_SUFFIX)))
	{
		key = CTools::wstr_2_str(CTools::utf8_2_unicode(e->key));
		value = CTools::wstr_2_str(CTools::utf8_2_unicode(e->value));
		m[key] = value;
	}
}
