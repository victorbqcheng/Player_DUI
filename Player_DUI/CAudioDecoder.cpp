#include "pch.h"
#include "CTools.h"
#include "CAudioDecoder.h"
#include "util.h"

#define SAFE_CONTINEU(ret)\
	if (ret != 0)\
	{\
		return -1;\
	}

CAudioDecoder::CAudioDecoder()
{
}

CAudioDecoder::~CAudioDecoder()
{
}

int CAudioDecoder::open(AVStream* p_stream, int index, CPacketReader* p_packet_reader)
{
	int ret = -1;

	this->p_audio_stream = p_stream;
	this->stream_index = index;
	this->p_packet_reader = p_packet_reader;

	AVCodecParameters* p_codec_param = p_audio_stream->codecpar;
	AVCodec* p_codec_audio = avcodec_find_decoder(p_codec_param->codec_id);

	p_codec_ctx_audio = avcodec_alloc_context3(p_codec_audio);
	ret = avcodec_parameters_to_context(p_codec_ctx_audio, p_codec_param);
	SAFE_CONTINEU(ret);

	ret = avcodec_open2(p_codec_ctx_audio, p_codec_audio, NULL);
	SAFE_CONTINEU(ret);

	p_codec_ctx_audio->channel_layout = av_get_default_channel_layout(p_codec_ctx_audio->channels);

	pcm_out_buffer = (char*)malloc(pcm_out_buffer_size);

	au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx,
		av_get_default_channel_layout(p_codec_ctx_audio->channels),
		AV_SAMPLE_FMT_S16,
		p_codec_ctx_audio->sample_rate,			//输出的采样率
		p_codec_ctx_audio->channel_layout,
		p_codec_ctx_audio->sample_fmt,
		p_codec_ctx_audio->sample_rate,	//输入的采样率
		0,
		NULL);
	swr_init(au_convert_ctx);

	return 0;
}

void CAudioDecoder::close()
{
	if (p_codec_ctx_audio != NULL)
	{
		avcodec_free_context(&p_codec_ctx_audio);
		p_codec_ctx_audio = NULL;
	}
	if (pcm_out_buffer != NULL)
	{
		free(pcm_out_buffer);
		pcm_out_buffer = NULL;
	}
	if (au_convert_ctx != NULL)
	{
		swr_free(&au_convert_ctx);
		au_convert_ctx = NULL;
	}
	this->p_audio_stream = NULL;
	this->stream_index = -1;
	this->p_packet_reader = NULL;
}

int CAudioDecoder::start_decode()
{
	b_stop = false;
	t_decode_audio = std::make_shared<std::thread>(&CAudioDecoder::decode_audio_thread, this);
	return 0;
}

void CAudioDecoder::stop_decode()
{
	b_stop = true;
	if (t_decode_audio)
	{
		t_decode_audio->join();
		t_decode_audio = NULL;
	}
	clear_data();
}

void CAudioDecoder::flush()
{
	clear_data();
	std::shared_ptr<AVFrame> p_frame(av_frame_alloc(), &util::av_frame_releaser);
	p_frame->opaque = (char*)CPacketReader::FLUSH;
	queue_audio_frames.push_back(p_frame);
}

bool CAudioDecoder::has_flush_flag()
{
	std::shared_ptr<AVFrame> p_frame(av_frame_alloc(), &util::av_frame_releaser);
	p_frame->opaque = (char*)CPacketReader::FLUSH;

	auto pred = [p_frame](std::shared_ptr<AVFrame> ele)->bool
	{
		if (p_frame->opaque == ele->opaque)
			return true;
		return false;
	};

	bool ret = queue_audio_frames.find(p_frame, pred);
	return ret;
}

std::shared_ptr<AVFrame> CAudioDecoder::get_frame()
{
	std::shared_ptr<AVFrame> p;
	if (queue_audio_frames.pop_front(p))
	{
		return p;
	}
	else
	{
		return NULL;
	}
}

int CAudioDecoder::get_stream_index()
{
	return stream_index;
}

std::tuple<char*, int, int64_t> CAudioDecoder::get_pcm()
{
	std::shared_ptr<AVFrame> frame;
	if(queue_audio_frames.pop_front(frame))
	{
		//set_swr_opts(frame);

		int ret_samples = swr_convert(au_convert_ctx, (uint8_t**)&pcm_out_buffer, pcm_out_buffer_size,
			(const uint8_t**)frame->data, frame->nb_samples);

		int ret_buffer_size = av_samples_get_buffer_size(NULL,
			p_codec_ctx_audio->channels,
			ret_samples,
			AV_SAMPLE_FMT_S16,
			1);

		return std::make_tuple(pcm_out_buffer, ret_buffer_size, frame->pts);
	}
	else
	{
		return std::make_tuple((char*)NULL, 0, 0);
	}
}

std::tuple<char*, int> CAudioDecoder::get_pcm(std::shared_ptr<AVFrame> p_frame)
{
	if (p_frame)
	{
		int ret_samples = swr_convert(au_convert_ctx, (uint8_t**)&pcm_out_buffer, pcm_out_buffer_size,
			(const uint8_t**)p_frame->data, p_frame->nb_samples);

		int ret_buffer_size = av_samples_get_buffer_size(NULL,
			p_codec_ctx_audio->channels,
			ret_samples,
			AV_SAMPLE_FMT_S16,
			1);

		return std::make_tuple(pcm_out_buffer, ret_buffer_size);
	}
	else
	{
		return std::make_tuple((char*)NULL, 0);
	}	
}

int CAudioDecoder::get_freq()
{
	if (p_codec_ctx_audio)
	{
		return p_codec_ctx_audio->sample_rate;
	}
	return 0;
}

int CAudioDecoder::get_channels()
{
	if (p_codec_ctx_audio)
	{
		return p_codec_ctx_audio->channels;
	}
	return 0;
}

int64_t CAudioDecoder::pts_to_milsecond(int64_t pts)
{
	double d = av_q2d(p_audio_stream->time_base);
	int64_t milsecond = int64_t(pts *1.0 * d * 1000);
	return milsecond;
}

int64_t CAudioDecoder::milsecond_to_pts(int64_t milsecond)
{
	double d = av_q2d(p_audio_stream->time_base);
	int64_t pts = int64_t(milsecond / 1000.0 /d );
	return pts;
}

void CAudioDecoder::clear_data()
{
	queue_audio_frames.clear();
}

bool CAudioDecoder::is_no_packet_to_decode()
{
	if (p_packet_reader->is_eof() && p_packet_reader->audio_packets_num() == 0)
	{
		return true;
	}
	return false;
}

int CAudioDecoder::set_swr_opts(std::shared_ptr<AVFrame> frame)
{
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_sample_rate = p_codec_ctx_audio->sample_rate;

	int out_channels = p_codec_ctx_audio->channels;
	uint64_t out_channel_layout = av_get_default_channel_layout(out_channels);

	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx,
		out_channel_layout,
		out_sample_fmt,
		out_sample_rate,			//输出的采样率
		p_codec_ctx_audio->channel_layout,
		p_codec_ctx_audio->sample_fmt,
		p_codec_ctx_audio->sample_rate,	//输入的采样率
		0,
		NULL);
	swr_init(au_convert_ctx);
	return 0;
}

int CAudioDecoder::decode_audio_thread()
{
	int ret = 0;
	while (true)
	{
		if (b_stop == true /*|| is_no_packet_to_decode() == true*/)
		{
			break;
		}
		if (queue_audio_frames.size() > 100)
		{
			CTools::thread_sleep(10);
			continue;
		}
		if (p_packet_reader->audio_packets_num() == 0)	//解码太快，等待读取线程
		{
			CTools::thread_sleep(10);
			continue;
		}

		std::shared_ptr<AVPacket> p_packet = p_packet_reader->get_audio_packet();
		if ((char*)p_packet->data == CPacketReader::FLUSH)
		{
			avcodec_flush_buffers(p_codec_ctx_audio);
			this->flush();
			continue;
		}

		ret = avcodec_send_packet(p_codec_ctx_audio, p_packet.get());
		if (ret != 0)
		{
			if (ret == AVERROR_EOF) 
				continue;
			//快进时可能会出现该错误,但是直接继续可以正常播放
			if (ret == AVERROR_INVALIDDATA)	
				continue;
		}

		std::shared_ptr<AVFrame> p_frame(av_frame_alloc(), &util::av_frame_releaser);
		ret = avcodec_receive_frame(p_codec_ctx_audio, p_frame.get());
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		{
			continue;
		}
		if (ret != 0) 
			break;
		queue_audio_frames.push_back(p_frame);
	}
	return 0;
}
