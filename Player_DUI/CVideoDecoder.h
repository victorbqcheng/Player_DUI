#pragma once

#include "CPacketReader.h"

class CVideoDecoder
{
public:
	CVideoDecoder();
	~CVideoDecoder();
	int open(AVStream* p_stream, int index, CPacketReader* p_packet_reader);
	void close();

	int start_decode();
	void stop_decode();

	int get_width();
	int get_height();

	bool is_no_frame_to_render();
	std::shared_ptr<AVFrame> get_frame();
	std::shared_ptr<AVFrame> convert_frame_to_given(std::shared_ptr<AVFrame> p_frm_raw);		//���ؿ���ֱ�Ӳ��ŵ�����p_frm_given
	int get_stream_index();
private:
	void clear_data();
	bool is_no_packet_to_decode();
	int decode_video_thread();
private:
	AVCodecContext* p_codec_ctx_video = NULL;
	AVStream* p_video_stream = NULL;	//��Ƶ��
	int stream_index = -1;	//��Ƶ������
	CPacketReader* p_packet_reader = NULL;
	SwsContext* sws_ctx = NULL;
	std::shared_ptr<AVFrame> p_frm_given = NULL;		//����ֱ�Ӳ��ŵ�RGB����
	uint8_t* buffer_rgb = NULL;
	bool b_stop = false;
	std::thread* t_decode_video = NULL;
	CQueue<std::shared_ptr<AVFrame>> queue_video_frames;
};

