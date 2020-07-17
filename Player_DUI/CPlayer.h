#pragma once
#include "pch.h"
#include <string>

#include "CPacketReader.h"
#include "CVideoDecoder.h"
#include "CAudioDecoder.h"
#include "CPlayAudioBySDL.h"
#include "condition_wrapper.h"

using RENDER_CALLBACK = std::function<void(char* data, int width, int height)>;

class CPlayer
{
public:
	enum PLAYER_STATE
	{
		PS_PLAYING,
		PS_PAUSING,
		PS_STOPPED
	};
	enum MEDIA_TYPE
	{
		MT_UNKNOWN = -1,
		MT_VIDEO,
		MT_AUDIO
	};
public:
	
	int open(std::string fileName);
	void close();

	int play();
	void stop();

	int pause();
	void continue_play();

	void set_volume(int volume);
	int const get_max_volume();

	void set_speed(double sp);

	//milseconds>0:forward
	//milseconds<0:backward
	void seek_by(int64_t milseconds);
	//��λ���������
	void seek_to(int64_t milseconds);

	int get_width();
	int get_height();
	int64_t get_duration();		//ʱ��, ��λ:΢��
	int64_t get_play_time();	//
	void set_render_callback(std::function<void(char* data, int width, int height)> cb);
private:
	void init_data();
	void update_play_time(int64_t t);	//����ʱ��
	int play_video_thread();
	void audio_callback(Uint8 *stream, int len);
private:
	PLAYER_STATE ps_state = PS_STOPPED;
	AVFormatContext* p_fmt_context = NULL;
	MEDIA_TYPE mt = MT_UNKNOWN;
	CPacketReader m_packet_reader;
	CVideoDecoder m_video_decoder;
	CAudioDecoder m_audio_decoder;
	CPlayAudioBySDL m_play_by_sdl;
	std::chrono::time_point<std::chrono::system_clock> start_time_point;

	std::shared_ptr<std::thread> t_play_video = NULL;
	std::shared_ptr<std::thread> t_play_audio = NULL;
	//��¼����ʱ��
	long long playing_length = 0;
	int volume = 0;
	double play_speed = 1.0;
	int64_t play_time = 0;	//���Ž��ȣ���λ:΢�롣�п��ܲ�����ʵ�Ĳ���ʱ��,�����б��ٲ���

	int audio_buf_index = 0;	//��Ƶ�������α�
	int audio_buf_size = 0;		//��Ƶ��������С
	char* audio_buf = NULL;		//��Ƶ���ݻ�������ʼ��ַ
	RENDER_CALLBACK render_callback;
};

