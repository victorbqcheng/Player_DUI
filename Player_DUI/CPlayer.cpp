#include "pch.h"
#include "CPlayer.h"
#include "util.h"

std::mutex mutex_for_seek;

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

void CPlayer::forward(int milseconds)
{
	int64_t new_play_time = play_time + int64_t(milseconds);
	int64_t duration = get_duration();
	if (new_play_time > duration)
	{
		new_play_time = duration - 1;
	}

	m_video_decoder.flush();
	m_audio_decoder.flush();
	m_packet_reader.seek(new_play_time);
}

void CPlayer::backward(int milseconds)
{
	int64_t new_play_time = play_time - int64_t(milseconds);
	if (new_play_time <= 0)
	{
		new_play_time = 0;
	}

	m_video_decoder.flush();
	m_audio_decoder.flush();
	m_packet_reader.seek(new_play_time);
	m_bBackward = true;
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
		return p_fmt_context->duration / 1000;	//微秒 
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

	m_bBackward = false;
}

void CPlayer::update_play_time(int64_t t)
{
	play_time = t;
}

int CPlayer::play_video_thread()
{
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
			int64_t mil_sec_video = m_video_decoder.pts_to_milsecond(p_frame->pts);
			auto current_time_point = std::chrono::system_clock::now();
			auto d_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time_point - start_time_point);

			if (m_bBackward == true)
			{
				m_bBackward = false;
				p_frame.reset();
				continue;
			}

			if (mil_sec_video <= play_time)
			{
				ps_state = PS_PLAYING;
				//记录播放时长
				//playing_length = mil_sec_video;
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
			if (!p_frame)
			{
				util::thread_sleep(10);	//wait to guarantee there is data
			}
		}
	}
	return 0;
}

void CPlayer::audio_callback(Uint8 *stream, int len)
{
	int len1;
	memset(stream, 0, len);
	while (len > 0)
	{
		if (audio_buf_index >= audio_buf_size)	//游标越过了缓冲区，说明需要获取新的PCM数据
		{
			int64_t pts = 0;
			std::tie(audio_buf, audio_buf_size, pts)= m_audio_decoder.get_pcm();
			if (audio_buf == NULL)
			{
				break;
			}
			audio_buf_index = 0;
			audio_pts = pts;
			int64_t pt = m_audio_decoder.pts_to_milsecond(pts);
			update_play_time(pt);
		}

		len1 = audio_buf_size - audio_buf_index;	//缓冲区可以提供len1长的数据
		if (len1 > len)		//供(len1)大于求(len)
		{
			len1 = len;		//需要多少(len)，提供多少len1=len;
		}
		
		m_play_by_sdl.mix_audio(stream, (uint8_t *)audio_buf + audio_buf_index, len1, this->volume);

		len -= len1;
		stream += len1;			//移动指针
		audio_buf_index += len1;//移动指针
	}
}
