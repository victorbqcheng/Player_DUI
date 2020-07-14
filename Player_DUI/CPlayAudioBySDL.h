#pragma once

#include <sdl.h>

#include <functional>

class CPlayAudioBySDL
{
	friend void audio_callback(void *userdata, Uint8 *stream, int len);
public:
	enum MODE
	{
		CB,
		REGULAR,
	};
	enum STATE
	{
		STATE_PLAYING,
		STATE_PAUSING,
		STATE_STOPPED,
	};
	CPlayAudioBySDL();
	~CPlayAudioBySDL();

	virtual int open(int freq, int out_channels);
	virtual void close();

	virtual int play();
	virtual void stop();

	virtual void pause();
	virtual void continue_play();

	virtual MODE get_mode()
	{
		return mode;
	}
	virtual int const get_max_volume();
	virtual int set_play_speed(double sp);
	virtual void set_callback(std::function<void(Uint8 *stream, int len)> cb);
	virtual void queue_audio(const void* data, unsigned int len);
	virtual void mix_audio(Uint8 * dst,const Uint8 * src, Uint32 len, int volume);
private:
	MODE const mode = CB;
	std::function<void(Uint8 *stream, int len)> callback_function;
	double play_speed = 1.0;
	int freq = -1;
	int out_channels = -1;
	STATE state = STATE_STOPPED;
};

