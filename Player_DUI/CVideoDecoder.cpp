#include "pch.h"
#include "CVideoDecoder.h"
#include "util.h"

#define SAFE_CONTINEU(ret)\
	if (ret != 0)\
	{\
		return -1;\
	}

CVideoDecoder::CVideoDecoder()
{
	this->p_video_stream = NULL;
	this->stream_index = -1;
	this->p_packet_reader = NULL;
}

CVideoDecoder::~CVideoDecoder()
{
// 	stop_decode();
// 	close();
// 	clear_data();
}

int CVideoDecoder::open(AVStream* p_stream, int index, CPacketReader* p_packet_reader)
{
	int ret = -1;

	this->p_video_stream = p_stream;
	this->stream_index = index;
	this->p_packet_reader = p_packet_reader;

	AVCodecParameters* p_codecpar_video = NULL;
	p_codecpar_video = p_video_stream->codecpar;

	AVCodec* p_codec_video = NULL;
	if (p_codec_video == NULL)
	{
		p_codec_video = avcodec_find_decoder(p_codecpar_video->codec_id);
	}
	p_codec_ctx_video = avcodec_alloc_context3(p_codec_video);
	if (p_codec_ctx_video == NULL)
	{
		return ret;
	}
	ret = avcodec_parameters_to_context(p_codec_ctx_video, p_codecpar_video);
	SAFE_CONTINEU(ret);
	ret = avcodec_open2(p_codec_ctx_video, p_codec_video, NULL);
	SAFE_CONTINEU(ret);

	//
	int width = p_codec_ctx_video->width;
	int height = p_codec_ctx_video->height;
	AVPixelFormat fmt = p_codec_ctx_video->pix_fmt;
	sws_ctx = sws_getContext(width,
		height,
		fmt,
		width,
		height,
		AV_PIX_FMT_BGR24,
		SWS_BICUBIC,
		NULL,
		NULL,
		NULL);

	int buf_size_rgb = av_image_get_buffer_size(AV_PIX_FMT_BGR24,
		width,
		height,
		1);

	p_frm_given =std::shared_ptr<AVFrame>(av_frame_alloc(), &util::av_frame_releaser);		//可以直接播放的RGB数据
	buffer_rgb = (uint8_t*)av_malloc(buf_size_rgb);	//需要释放
	av_image_fill_arrays(p_frm_given->data,
		p_frm_given->linesize,
		buffer_rgb,
		AV_PIX_FMT_BGR24,
		//AV_PIX_FMT_RGB24,
		width,
		height,
		1);
	return 0;
}

void CVideoDecoder::close()
{
	if (p_codec_ctx_video != NULL)
	{
		avcodec_free_context(&p_codec_ctx_video);
		p_codec_ctx_video = NULL;
	}
	if (p_frm_given != NULL)
	{
		p_frm_given = NULL;
	}
	if (buffer_rgb != NULL)
	{
		av_free(buffer_rgb);
		buffer_rgb = NULL;
	}
	this->p_video_stream = NULL;
	this->stream_index = -1;
	this->p_packet_reader = NULL;
}

int CVideoDecoder::start_decode()
{
	b_stop = false;
	t_decode_video = new std::thread(&CVideoDecoder::decode_video_thread, this);
	return 0;
}

void CVideoDecoder::stop_decode()
{
	b_stop = true;
	if (t_decode_video != NULL)
	{
		t_decode_video->join();
		SAFE_DELETE(t_decode_video);
	}
	clear_data();
}

int CVideoDecoder::get_width()
{
	if (p_codec_ctx_video != NULL)
	{
		return p_codec_ctx_video->width;
	}
	else
	{
		return 0;
	}
}

int CVideoDecoder::get_height()
{
	if (p_codec_ctx_video != NULL)
	{
		return p_codec_ctx_video->height;
	}
	else
	{
		return 0;
	}
}

void CVideoDecoder::clear_data()
{
	AVFrame* p_frm = NULL;
	for (; queue_video_frames.size() > 0;)
	{
		std::shared_ptr<AVFrame> p;
		queue_video_frames.pop(p);
	}
	queue_video_frames.clear();
}
bool CVideoDecoder::is_no_packet_to_decode()
{
	if (p_packet_reader->is_eof() && p_packet_reader->video_packets_num() == 0)
	{
		return true;
	}
	return false;
}
bool CVideoDecoder::is_no_frame_to_render()
{
	if(is_no_packet_to_decode() && queue_video_frames.size() == 0)
		return true;
	return false;
}

std::shared_ptr<AVFrame> CVideoDecoder::get_frame()
{
	if (queue_video_frames.size() > 0)
	{
		std::shared_ptr<AVFrame> p;
		queue_video_frames.pop(p);
		return p;
	}
	else
	{
		return NULL;
	}
}

std::shared_ptr<AVFrame> CVideoDecoder::convert_frame_to_given(std::shared_ptr<AVFrame> p_frm_raw)
{
	if (p_frm_raw)
	{
		int height = p_codec_ctx_video->height;
		int ret = sws_scale(sws_ctx,
			p_frm_raw->data,
			p_frm_raw->linesize,
			0,
			height,
			p_frm_given->data,
			p_frm_given->linesize);
		return p_frm_given;
	}
	else
	{
		return NULL;
	}
}

int CVideoDecoder::get_stream_index()
{
	return stream_index;
}

int CVideoDecoder::decode_video_thread()
{
	int ret = 0;
	while (true)
	{
		if (b_stop == true || is_no_packet_to_decode() == true)
		{
			break;
		}
		if (queue_video_frames.size() > 10)		//最多缓存10帧
		{
			util::thread_sleep(5);
			continue;
		}
		if (p_packet_reader->video_packets_num() == 0)	//解码太快，等待读取线程
		{
			util::thread_sleep(10);
			continue;
		}
		//VideoPacketsInQueue() > 0
		std::shared_ptr<AVPacket> p_packet = p_packet_reader->get_video_packet();
		ret = avcodec_send_packet(p_codec_ctx_video, p_packet.get());
		if (ret != 0)
		{
			break;
		}
		AVFrame* p_frm = av_frame_alloc();
		std::shared_ptr<AVFrame> p_frame(p_frm, &util::av_frame_releaser);
		ret = avcodec_receive_frame(p_codec_ctx_video, p_frm);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		{
			continue;
		}
		if (ret != 0)
		{
			break;
		}
		queue_video_frames.push(p_frame);
// 		char buf[128] = { 0 };
// 		sprintf(buf, "queue_video_frames size:%d\r\n", queue_video_frames.size());
// 		util::log(buf);
	}
	return ret;
}