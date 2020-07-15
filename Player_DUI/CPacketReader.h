#pragma once


#include "CQueue.h"
#include <memory>

using PacketQueue = CQueue<std::shared_ptr<AVPacket>>;

class CPacketReader
{
public:
	static char const* FLUSH;
	CPacketReader();
	~CPacketReader();

	void open(AVFormatContext* p_fmt_ctx,int video_stream_index,int audio_stream_index);
	void close();

	void start_read();
	//void pause_read();
	void stop_read();

	bool is_eof();

	CQueue<AVPacket*>& get_video_queue();
	CQueue<AVPacket*>& get_audio_queue();

	//队列中视频packet的数量
	int video_packets_num();
	//队列中音频packet的数量
	int audio_packets_num();
	
	std::shared_ptr<AVPacket> get_video_packet();
	std::shared_ptr<AVPacket> get_audio_packet();

	void flush();
private:
	void clear_data();
	int read_thread();
private:
	std::thread* t_read = NULL;

	AVFormatContext* p_fmt_ctx = NULL;
	int video_stream_index = -1;
	int audio_stream_index = -1;

	bool b_stop = false;
	bool b_eof = false;

	PacketQueue queue_video_packets;
	PacketQueue queue_audio_packets;
};

