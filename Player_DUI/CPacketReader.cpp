#include "pch.h"
#include "CPacketReader.h"
#include "util.h"

CPacketReader::CPacketReader()
{

}

CPacketReader::~CPacketReader()
{
// 	stop_read();
// 	clear_data();
}

void CPacketReader::open(AVFormatContext* p_fmt_ctx,int video_stream_index,int audio_stream_index)
{
	this->p_fmt_ctx = p_fmt_ctx;
	this->video_stream_index = video_stream_index;
	this->audio_stream_index = audio_stream_index;
}

void CPacketReader::close()
{
	this->p_fmt_ctx = NULL;
	this->video_stream_index = -1;
	this->audio_stream_index = -1;
}

void CPacketReader::start_read()
{
	b_stop = false;
	t_read = new std::thread(&CPacketReader::read_thread, this);
}

void CPacketReader::stop_read()
{
	b_stop = true;
	if (t_read != NULL)
	{
		t_read->join();
		SAFE_DELETE(t_read);
	}
	clear_data();
}
bool CPacketReader::is_eof()
{
	return b_eof;
}
CQueue<AVPacket*>& CPacketReader::get_video_queue()
{
	throw "deprecated";
	//return queue_video_packets;
}
CQueue<AVPacket*>& CPacketReader::get_audio_queue()
{
	throw "deprecated";
	//return queue_audio_packets;
}
int CPacketReader::video_packets_num()
{
	return queue_video_packets.size();
}
std::shared_ptr<AVPacket> CPacketReader::get_video_packet()
{
	if (queue_video_packets.size() > 0)
	{
		std::shared_ptr<AVPacket> p;
		queue_video_packets.pop(p);
		return p;
	}
	else
	{
		std::shared_ptr<AVPacket> p(NULL);
		return p;
	}
}
std::shared_ptr<AVPacket> CPacketReader::get_audio_packet()
{
	if (queue_audio_packets.size() > 0)
	{
		std::shared_ptr<AVPacket> p;
		queue_audio_packets.pop(p);
		return p;
	}
	else
	{
		std::shared_ptr<AVPacket> p(NULL);
		return p;
	}
}
int CPacketReader::audio_packets_num()
{
	return queue_audio_packets.size();
}
void CPacketReader::clear_data()
{
	AVPacket* p_packet = NULL;
	for (;queue_video_packets.size()>0;)
	{
		std::shared_ptr<AVPacket> p;
		queue_video_packets.pop(p);
	}
	queue_video_packets.clear();

	for (;queue_audio_packets.size()>0;)
	{
		std::shared_ptr<AVPacket> p;
		queue_audio_packets.pop(p);
	}
	queue_audio_packets.clear();
}
//
int CPacketReader::read_thread()
{
	while (true)
	{
		if (b_stop == true)
		{
			break;
		}	
		if ( queue_video_packets.size()>2000)	//最多缓存2000个packet
		{
			util::thread_sleep(10);
			continue;
		}
		if (queue_audio_packets.size() > 2000)
		{
			util::thread_sleep(10);
			continue;
		}
		AVPacket* p_packet = (AVPacket*)malloc(sizeof(AVPacket));
		if (av_read_frame(p_fmt_ctx, p_packet) != 0)
		{
			b_eof = true;	//文件已读完
			b_stop = true;	
			free(p_packet);
			break;
		}
		std::shared_ptr<AVPacket> p(p_packet, &util::av_packet_releaser);
		if (p_packet->stream_index == video_stream_index)
		{
			//queue_video_packets.push(p_packet);
			queue_video_packets.push(p);
		}
		else if (p_packet->stream_index == audio_stream_index)
		{
			//queue_audio_packets.push(p_packet);
			queue_audio_packets.push(p);
		}
// 		char buf[128] = {0};
// 		sprintf(buf, "queue_video_packets size:%d\r\n", queue_video_packets.size());
// 		util::log(buf);
// 
// 		memset(buf, sizeof(buf), 0);
// 		sprintf(buf, "queue_audio_packets size:%d\r\n", queue_audio_packets.size());
// 		util::log(buf);
	}
	return 0;
}
