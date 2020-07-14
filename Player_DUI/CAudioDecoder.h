#pragma once

#include <tuple>

#include "CPacketReader.h"

class CAudioDecoder
{
public:
	CAudioDecoder();
	~CAudioDecoder();

	int open(AVStream* p_stream, int index, CPacketReader* p_packet_reader);
	void close();

	int start_decode();
	void stop_decode();

	std::shared_ptr<AVFrame> get_frame();
	std::tuple<char*, int, int64_t> get_pcm();		//pcm音频可以直接播放
	std::tuple<char*, int> get_pcm(std::shared_ptr<AVFrame> p_frame);		//pcm音频可以直接播放
	int get_stream_index();
	int get_freq();
	int get_channels();
private:
	void clear_data();
	bool is_no_packet_to_decode();
	int set_swr_opts(std::shared_ptr<AVFrame> frame);
	int decode_audio_thread();
private:
	AVCodecContext* p_codec_ctx_audio = NULL;
	AVStream* p_audio_stream = NULL;
	int stream_index = -1;	//视频流索引
	CPacketReader* p_packet_reader = NULL;
	bool b_stop = false;

	SwrContext* au_convert_ctx = NULL;

	char* pcm_out_buffer = NULL;
	int const pcm_out_buffer_size = 1024 * 32;

	std::shared_ptr<std::thread> t_decode_audio = NULL;
	CQueue<std::shared_ptr<AVFrame>> queue_audio_frames;
};

