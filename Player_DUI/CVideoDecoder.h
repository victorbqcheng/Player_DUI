#pragma once

#include "CPacketReader.h"
#include "util.h"

class CVideoDecoder
{
	//using FrameQueue = CQueue<std::shared_ptr<AVFrame>>;
	using FrameQueue = CDeque<std::shared_ptr<AVFrame>>;
public:
	CVideoDecoder();
	~CVideoDecoder();
	int open(AVStream* p_stream, int index, CPacketReader* p_packet_reader);
	void close();

	int start_decode();
	void stop_decode();

	int get_width();
	int get_height();

	void flush();
	bool has_flush_flag();

	bool is_no_frame_to_render();
	std::shared_ptr<AVFrame> get_frame();

	std::shared_ptr<AVFrame> front_frame();
	std::shared_ptr<AVFrame> pop_front_frame();

	std::shared_ptr<AVFrame> convert_frame_to_given(std::shared_ptr<AVFrame> p_frm_raw);		//���ؿ���ֱ�Ӳ��ŵ�����p_frm_given
	int get_stream_index();

	int64_t pts_to_milsecond(int64_t pts);
	int64_t milsecond_to_pts(int64_t milsecond);
private:
	void clear_data();
	bool is_no_packet_to_decode();
	int decode_video_thread();
private:
	AVCodecContext* p_codec_ctx_video = NULL;
	AVStream* p_video_stream = NULL;	//��Ƶ��
	METADATA_P metadata;
	int stream_index = -1;	//��Ƶ������
	CPacketReader* p_packet_reader = NULL;
	bool b_stop = false;

	SwsContext* sws_ctx = NULL;

	std::shared_ptr<AVFrame> p_frm_given = NULL;		//����ֱ�Ӳ��ŵ�RGB����
	uint8_t* buffer_rgb = NULL;
	
	std::thread* t_decode_video = NULL;
	FrameQueue queue_video_frames;
};

