#pragma once

#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "CQueue.h"

//using PacketQueue = CQueue<std::shared_ptr<AVPacket>>;
using PacketQueue = CDeque<std::shared_ptr<AVPacket>>;

class CPacketReader
{
public:
	static char const* FLUSH;
	CPacketReader();
	~CPacketReader();

	void open(AVFormatContext* p_fmt_ctx,int video_stream_index,int audio_stream_index, std::vector<int>& subtitle_indexs);
	void close();

	void start_read();
	//void pause_read();
	void stop_read();
	
	void seek(int64_t milseconds);
	
	bool is_eof();

	//队列中视频packet的数量
	int video_packets_num();
	//队列中音频packet的数量
	int audio_packets_num();
	int subtitle_packets_num();
	
	std::shared_ptr<AVPacket> get_video_packet();
	std::shared_ptr<AVPacket> get_audio_packet();
	std::shared_ptr<AVPacket> get_subtitle_packet();
private:
	void flush();
	void clear_data();
	bool is_subtitle_packet(int idx);
	int read_thread();
private:
	std::thread* t_read = NULL;

	std::mutex mutex_for_fmt_ctx;
	AVFormatContext* p_fmt_ctx = NULL;
	int video_stream_index = -1;
	int audio_stream_index = -1;
	std::vector<int> subtitle_indexs;

	bool b_stop = false;
	bool b_eof = false;

	PacketQueue queue_video_packets;
	PacketQueue queue_audio_packets;
	std::map<int, std::shared_ptr<PacketQueue>> subtitle_queues;	//index--queue


	bool m_b_seek = false;
	int64_t m_seek_time = 0;
};

