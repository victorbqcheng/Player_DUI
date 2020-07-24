#include "pch.h"
#include "CSubtitleDecoder.h"
#include "util.h"
#include "CTools.h"

int CSubtitleDecoder::open(AVStream* p_stream, int index, CPacketReader* p_packet_reader)
{
	int ret = -1;
	this->p_subtitle_stream = p_stream;
	this->stream_index = index;
	this->p_packet_reader = p_packet_reader;
	util::av_dict_2_map(p_stream->metadata, metadata);

	AVCodecParameters* p_codec_param = p_subtitle_stream->codecpar;
	AVCodec* p_codec_subtitle = avcodec_find_decoder(p_codec_param->codec_id);

	p_codec_ctx_subtitle = avcodec_alloc_context3(p_codec_subtitle);
	ret = avcodec_parameters_to_context(p_codec_ctx_subtitle, p_codec_param);
	SAFE_CONTINEU(ret);

	ret = avcodec_open2(p_codec_ctx_subtitle, p_codec_subtitle, NULL);
	SAFE_CONTINEU(ret);

	return 0;
}

void CSubtitleDecoder::close()
{
	if (p_codec_ctx_subtitle != NULL)
	{
		avcodec_free_context(&p_codec_ctx_subtitle);
		p_codec_ctx_subtitle = NULL;
	}
	this->p_subtitle_stream = NULL;
	this->stream_index = -1;
	this->p_packet_reader = NULL;
}

int CSubtitleDecoder::start_decode()
{
	b_stop = false;
	t_decode_subtitle = std::make_shared<std::thread>(&CSubtitleDecoder::decode_subtitle_thread, this);
	return 0;
}

void CSubtitleDecoder::stop_decode()
{
	b_stop = true;
	if (t_decode_subtitle)
	{
		t_decode_subtitle->join();
		t_decode_subtitle = NULL;
	}
	clear_data();
}

void CSubtitleDecoder::flush()
{
	clear_data();
// 	std::shared_ptr<AVSubtitle> p_frame(, &util::av_frame_releaser);
// 	p_frame->opaque = (char*)CPacketReader::FLUSH;
// 	queue_audio_frames.push_back(p_frame);
}

bool CSubtitleDecoder::has_flush_flag()
{
	return false;
}

int CSubtitleDecoder::get_stream_index()
{
	return stream_index;
}

std::shared_ptr<AVSubtitle> CSubtitleDecoder::get_subtitle()
{
	std::shared_ptr<AVSubtitle> p(NULL);
	queue_subtitle_frames.pop_front(p);
	return p;
}

std::shared_ptr<AVSubtitle> CSubtitleDecoder::front_subtitle()
{
	std::shared_ptr<AVSubtitle> p(NULL);
	queue_subtitle_frames.front(p);
	return p;
}

std::shared_ptr<AVSubtitle> CSubtitleDecoder::pop_front_subtitle()
{
	std::shared_ptr<AVSubtitle> p(NULL);
	queue_subtitle_frames.pop_front(p);
	return p;
}

void CSubtitleDecoder::clear_data()
{
	queue_subtitle_frames.clear();
}

int64_t CSubtitleDecoder::pts_to_milsecond(int64_t pts)
{
	double d = av_q2d(p_subtitle_stream->time_base);
	int64_t milsecond = int64_t(pts *1.0 * d * 1000);
	return milsecond;
}

int CSubtitleDecoder::decode_subtitle_thread()
{
	int ret = 0;
	while (true)
	{
		if (b_stop == true)
		{
			break;
		}
		if (p_packet_reader->subtitle_packets_num() == 0)
		{
			CTools::thread_sleep(50);
			continue;
		}
		std::shared_ptr<AVPacket> p_packet = p_packet_reader->get_subtitle_packet();
		if (p_packet)
		{
			if ((char*)p_packet->data == CPacketReader::FLUSH)
			{
				avcodec_flush_buffers(p_codec_ctx_subtitle);
				this->flush();
				continue;
			}
			else
			{
				std::shared_ptr<AVSubtitle> sub = std::make_shared<AVSubtitle>();
				int got = 0;
				ret = avcodec_decode_subtitle2(p_codec_ctx_subtitle, sub.get(), &got, p_packet.get());
				if (ret >= 0)
				{
					if (sub->pts == AV_NOPTS_VALUE)
					{
						sub->pts = p_packet->pts;
					}
					queue_subtitle_frames.push_back(sub);
				}
			}
		}
	}
	return 0;
}
