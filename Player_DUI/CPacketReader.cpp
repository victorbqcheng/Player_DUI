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
	m_b_seek = true;
	m_seek_time = milseconds;
}

bool CPacketReader::is_eof()
{
	return b_eof;
}

int CPacketReader::video_packets_num()
{
	return queue_video_packets.size();
}
std::shared_ptr<AVPacket> CPacketReader::get_video_packet()
{
	std::shared_ptr<AVPacket> p;
	if (queue_video_packets.pop_front(p))
	{
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
	std::shared_ptr<AVPacket> p;
	if(queue_audio_packets.pop_front(p))
	{
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

	queue_audio_packets.push_back(p);
	queue_video_packets.push_back(p);
}

int CPacketReader::audio_packets_num()
{
	return queue_audio_packets.size();
}
void CPacketReader::clear_data()
{
	AVPacket* p_packet = NULL;
	queue_video_packets.clear();
	queue_audio_packets.clear();
}
//
int CPacketReader::read_thread()
{
	auto is_cache_full = [this]()->bool
	{
		if (queue_video_packets.size() > 50 || queue_audio_packets.size() > 20)
			return true;

		return false;
	};

	while (true)
	{
		if (b_stop == true)
		{
			break;
		}
		if (is_cache_full() == true)	//最多缓存2000个packet
		{
			util::thread_sleep(10);
			continue;
		}

		if (m_b_seek)
		{
			int n1 = av_seek_frame(p_fmt_ctx, -1, m_seek_time * 1000, AVSEEK_FLAG_BACKWARD);
			flush();
			m_b_seek = false;
		}

		AVPacket* p_packet = (AVPacket*)malloc(sizeof(AVPacket));
		std::shared_ptr<AVPacket> p(p_packet, &util::av_packet_releaser);
		{
			int n = av_read_frame(p_fmt_ctx, p.get());
			if (n < 0)
			{
				if (n == AVERROR_EOF)
				{
					b_eof = true;	//文件已读完
					util::thread_sleep(10);
					continue;
				}
				else
				{
					break;
				}
			}
		}
		if (p_packet->stream_index == video_stream_index)
		{
			queue_video_packets.push_back(p);
		}
		else if (p_packet->stream_index == audio_stream_index)
		{
			queue_audio_packets.push_back(p);
		}
	}
	return 0;
}
