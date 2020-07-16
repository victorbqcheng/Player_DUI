#include "pch.h"
#include "CPacketReader.h"
#include "util.h"

char const* CPacketReader::FLUSH = "CODEC_FLUSH";	//解码器清空标志,快进/快退时使用

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
void CPacketReader::seek(int64_t milseconds)
{
	//std::lock_guard<std::mutex> lock(mutex_for_fmt_ctx);
	extern std::mutex mutex_for_seek;
	std::lock_guard<std::mutex> lock(mutex_for_seek);
	flush();
	if (video_stream_index >= 0)
	{
		double d = av_q2d(p_fmt_ctx->streams[video_stream_index] ->time_base);
		int64_t pts_video = int64_t(milseconds / 1000.0 / d);
		int n1 = av_seek_frame(p_fmt_ctx, video_stream_index, pts_video, AVSEEK_FLAG_ANY);
	}
	else if (audio_stream_index >= 0)
	{
		double d = av_q2d(p_fmt_ctx->streams[audio_stream_index]->time_base);
		int64_t pts_audio = int64_t(milseconds / 1000.0 / d);
		int n1 = av_seek_frame(p_fmt_ctx, audio_stream_index, pts_audio, AVSEEK_FLAG_BACKWARD);
	}
}

bool CPacketReader::is_eof()
{
	return b_eof;
}
CQueue<AVPacket*>& CPacketReader::get_video_queue()
{
	throw "deprecated";
}
CQueue<AVPacket*>& CPacketReader::get_audio_queue()
{
	throw "deprecated";
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

void CPacketReader::flush()
{
	clear_data();
	AVPacket* p_packet = (AVPacket*)malloc(sizeof(AVPacket));
	av_init_packet(p_packet);
	p_packet->data = (uint8_t *)FLUSH;
	std::shared_ptr<AVPacket> p(p_packet, &util::av_packet_releaser);

	queue_audio_packets.push(p);
	queue_video_packets.push(p);
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
		
		if ( queue_video_packets.size()>50 && queue_audio_packets.size()>20)	//最多缓存2000个packet
		{
			util::thread_sleep(10);
			continue;
		}
		
// 		if (queue_audio_packets.size() > 20)
// 		{
// 			util::thread_sleep(10);
// 			continue;
// 		}
		AVPacket* p_packet = (AVPacket*)malloc(sizeof(AVPacket));
		std::shared_ptr<AVPacket> p(p_packet, &util::av_packet_releaser);
		extern std::mutex mutex_for_seek;
		std::lock_guard<std::mutex> lock(mutex_for_seek);
		{
			if (av_read_frame(p_fmt_ctx, p_packet) != 0)
			{
				b_eof = true;	//文件已读完
				b_stop = true;
				break;
			}
		}
		if (p_packet->stream_index == video_stream_index)
		{
			queue_video_packets.push(p);
		}
		else if (p_packet->stream_index == audio_stream_index)
		{
			queue_audio_packets.push(p);
		}
	}
	return 0;
}
