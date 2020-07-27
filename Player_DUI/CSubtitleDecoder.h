#pragma once

#include "CPacketReader.h"
#include "util.h"

class CSubtitleDecoder
{
	using FrameQueue = CDeque<std::shared_ptr<AVSubtitle>>;
	struct SubtitleStream
	{
		AVStream* p_stream;
		int index;
		AVCodecContext* p_codec_ctx;
		AVCodec* p_codec;
	};
public:
	CSubtitleDecoder()
	{
	}
	~CSubtitleDecoder()
	{
	}
	int open(AVStream* p_stream, int index, CPacketReader* p_packet_reader);
	void close();

	int start_decode();
	void stop_decode();

	void flush();
	bool has_flush_flag();

	int get_stream_index();

	std::shared_ptr<AVSubtitle> get_subtitle();
	std::shared_ptr<AVSubtitle> front_subtitle();
	std::shared_ptr<AVSubtitle> pop_front_subtitle();
	
	int64_t pts_to_milsecond(int64_t pts);
private:
	void clear_data();
	//bool is_no_packet_to_decode();
	int decode_subtitle_thread();
private:

	std::vector<SubtitleStream> m_subtitleStreams;

	AVCodecContext* p_codec_ctx_subtitle = NULL;
	AVStream* p_subtitle_stream = NULL;	//视频流
	METADATA_P metadata;
	int stream_index = -1;	//视频流索引
	CPacketReader* p_packet_reader = NULL;
	bool b_stop = false;

	std::shared_ptr<std::thread> t_decode_subtitle = NULL;
	FrameQueue queue_subtitle_frames;
}; 

