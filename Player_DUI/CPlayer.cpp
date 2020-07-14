#include "pch.h"
#include "CPlayer.h"
#include "util.h"

#define SAFE_CONTINEU(ret)\
	if (ret != 0)\
	{\
		return -1;\
	}

int CPlayer::open(std::string fileName)
{
	init_data();

	int ret = avformat_open_input(&p_fmt_context, fileName.c_str(), NULL, NULL);
	SAFE_CONTINEU(ret);
	ret = avformat_find_stream_info(p_fmt_context, NULL);
	SAFE_CONTINEU(ret);

	int v_index = -1, a_index = -1;
	for (unsigned int i = 0; i < p_fmt_context->nb_streams; i++)
	{
		if (p_fmt_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			v_index = i;
		}
		if (p_fmt_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			a_index = i;
		}
	}
	if (v_index != -1)
	{
		mt = MT_VIDEO;
	}
	else if (a_index != -1)
	{
		mt = MT_AUDIO;
	}
	if (mt == MT_UNKNOWN)
	{
		return -1;
	}

	m_packet_reader.open(p_fmt_context, v_index, a_index);

	if (v_index != -1)
	{		
		m_video_decoder.open(p_fmt_context->streams[v_index], v_index, &m_packet_reader);
	}
	if (a_index != -1)
	{
		m_audio_decoder.open(p_fmt_context->streams[a_index], a_index, &m_packet_reader);
		
		m_play_by_sdl.open(m_audio_decoder.get_freq(),
			m_audio_decoder.get_channels());
	}
	return 0;
}

void CPlayer::close()
{
	if (p_fmt_context != NULL)
	{
		avformat_close_input(&p_fmt_context);
	}

	m_packet_reader.close();
	m_video_decoder.close();
	m_audio_decoder.close();
	m_play_by_sdl.close();
}

int CPlayer::play()
{	
	ps_state = PS_PLAYING;
	if (mt == MT_VIDEO || mt == MT_AUDIO)
	{
		m_packet_reader.start_read();
	}
	if (m_video_decoder.get_stream_index() >= 0)
	{
		m_video_decoder.start_decode();
		t_play_video = std::shared_ptr<std::thread>(new std::thread(&CPlayer::play_video_thread, this));
	}
	if (m_audio_decoder.get_stream_index() >= 0)
	{
		m_audio_decoder.start_decode();
		m_play_by_sdl.play();
		if (m_play_by_sdl.get_mode() == CPlayAudioBySDL::CB)
		{
			m_play_by_sdl.set_callback(std::bind(&CPlayer::audio_callback, this, std::placeholders::_1, std::placeholders::_2));
		}
		else
		{
			//t_play_audio = std::shared_ptr<std::thread>(new std::thread(&CPlayer::play_audio_thread, this));
		}
	}
	
	start_time_point = std::chrono::system_clock::now();
	return 0;
}

void CPlayer::stop()
{
	ps_state = PS_STOPPED;

	m_play_by_sdl.stop();
	if (t_play_video)
	{
		t_play_video->join();
		t_play_video.reset();
	}
	if (t_play_audio)
	{
		t_play_audio->join();
		t_play_audio.reset();
	}
	
	m_video_decoder.stop_decode();
	m_audio_decoder.stop_decode();
	m_packet_reader.stop_read();
}

int CPlayer::pause()
{
	m_play_by_sdl.pause();
	ps_state = PS_PAUSING;
	return 0;
}

void CPlayer::continue_play()
{
	start_time_point = std::chrono::system_clock::now();
	start_time_point -= std::chrono::milliseconds(playing_length);
	m_play_by_sdl.continue_play();
	ps_state = PS_PLAYING;
}

void CPlayer::set_volume(int volume)
{
	this->volume = volume;
}

int const CPlayer::get_max_volume()
{
	return m_play_by_sdl.get_max_volume();
}

void CPlayer::set_speed(double sp)
{
	this->play_speed = sp;
	m_play_by_sdl.set_play_speed(play_speed);
}

int CPlayer::get_width()
{
	return m_video_decoder.get_width();
}

int CPlayer::get_height()
{
	return m_video_decoder.get_height();
}

int64_t CPlayer::get_duration()
{
	if (p_fmt_context)
	{
		return p_fmt_context->duration / 1000;	//΢�� 
	}
	else
	{
		return 0;
	}
}

int64_t CPlayer::get_play_time()
{
	return play_time;
}

void CPlayer::set_render_callback(std::function<void(char*, int, int)> cb)
{
	render_callback = cb;
}

void CPlayer::init_data()
{
	PLAYER_STATE ps_state = PS_STOPPED;
	MEDIA_TYPE mt = MT_UNKNOWN;
	long long playing_length = 0;
	int volume = 0;
	double play_speed = 1.0;
	int64_t audio_pts = 0;
	int64_t play_time = 0;

	int audio_buf_index = 0;
	int audio_buf_size = 0;
	char* audio_buf = NULL;
}

void CPlayer::update_play_time(int64_t t)
{
	play_time = t;
}

int CPlayer::play_video_thread()
{
	util::thread_sleep(10);	//wait to guarantee there is data

	int stream_video_index = m_video_decoder.get_stream_index();
	int num = p_fmt_context->streams[stream_video_index]->time_base.num;
	int den = p_fmt_context->streams[stream_video_index]->time_base.den;

	std::shared_ptr<AVFrame> p_frame;
	while (true)
	{
		if (ps_state == PS_PAUSING)
		{
			util::thread_sleep(2);
			continue;
		}
		if (ps_state == PS_STOPPED || m_video_decoder.is_no_frame_to_render())
		{
			break;
		}
		if (p_frame)
		{
			long long mil_sec = (long long)(p_frame->pts * 1.0 * num / den * 1000);
			auto current_time_point = std::chrono::system_clock::now();

			auto d_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time_point - start_time_point);

			//if (mil_sec <= d_time.count())
			if (p_frame->pts <= audio_pts)
			{
				ps_state = PS_PLAYING;
				//��¼����ʱ��
				playing_length = mil_sec;
				std::shared_ptr<AVFrame> p = m_video_decoder.convert_frame_to_given(p_frame);
				
				if (render_callback)
				{
					render_callback((char*)(p->data[0]), p_frame->width, p_frame->height);
				}
				p_frame.reset();
			}
			else
			{
				util::thread_sleep(5);
			}
		}
		else
		{
			p_frame = m_video_decoder.get_frame();
		}
	}
	return 0;
}


void CPlayer::audio_callback(Uint8 *stream, int len)
{
	int stream_audio_index = m_audio_decoder.get_stream_index();
	int num = p_fmt_context->streams[stream_audio_index]->time_base.num;
	int den = p_fmt_context->streams[stream_audio_index]->time_base.den;

	int len1;
	memset(stream, 0, len);
	while (len > 0)
	{
		if (audio_buf_index >= audio_buf_size)	//�α�Խ���˻�������˵����Ҫ��ȡ�µ�PCM����
		{
			int64_t pts = 0;
			std::tie(audio_buf, audio_buf_size, pts)= m_audio_decoder.get_pcm();
			if (audio_buf == NULL)
			{
				break;
			}
			audio_buf_index = 0;
			audio_pts = pts;
			int64_t pt = (int64_t)(double(pts) * num / den * 1000);
			update_play_time(pt);
		}

		len1 = audio_buf_size - audio_buf_index;	//�����������ṩlen1��������
		if (len1 > len)		//��(len1)������(len)
		{
			len1 = len;		//��Ҫ����(len)���ṩ����len1=len;
		}
		
		m_play_by_sdl.mix_audio(stream, (uint8_t *)audio_buf + audio_buf_index, len1, this->volume);

		len -= len1;
		stream += len1;			//�ƶ�ָ��
		audio_buf_index += len1;//�ƶ�ָ��
	}
}