#pragma once

#include "pch.h"
#include "CPlayAudioBySDL.h"
#include "CTools.h"

class CSDLInit
{
public:
	CSDLInit()
	{
		int flags = SDL_INIT_AUDIO;
		SDL_Init(flags);
	}
};
CSDLInit ssssssssssssssssssss;

CPlayAudioBySDL::CPlayAudioBySDL()
{

}

CPlayAudioBySDL::~CPlayAudioBySDL()
{

}
static void audio_callback(void *userdata, Uint8 *stream, int len)
{
	CPlayAudioBySDL* pThis = (CPlayAudioBySDL*)userdata;
	if (pThis->callback_function)
	{
		pThis->callback_function(stream, len);
	}
}
int CPlayAudioBySDL::open(int freq, int out_channels)
{
	this->freq = freq;
	this->out_channels = out_channels;
	
	SDL_AudioSpec wanted_spec = { 0 };
	wanted_spec.freq = int(freq * play_speed);// p_codec_ctx->sample_rate;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = out_channels; //out_channels;
	wanted_spec.samples = 1024; 
	wanted_spec.callback = audio_callback;
	wanted_spec.userdata = this;

	if (SDL_OpenAudio(&wanted_spec, NULL) < 0)
	{
		return -1;
	}
	return 0;
}

void CPlayAudioBySDL::close()
{
	SDL_CloseAudio();
	this->freq = -1;
	this->out_channels = -1;
}

int CPlayAudioBySDL::play()
{
	state = STATE_PLAYING;
	SDL_PauseAudio(0);
	return 0;
}

void CPlayAudioBySDL::stop()
{
	state = STATE_STOPPED;
	SDL_PauseAudio(1);
	SDL_CloseAudio();
}

void CPlayAudioBySDL::pause()
{
	state = STATE_PAUSING;
	SDL_PauseAudio(1);
}

void CPlayAudioBySDL::continue_play()
{
	state = STATE_PLAYING;
	SDL_PauseAudio(0);
}

int const CPlayAudioBySDL::get_max_volume()
{
	return SDL_MIX_MAXVOLUME;
}

int CPlayAudioBySDL::set_play_speed(double sp)
{
	play_speed = sp;
	if (state == STATE_PLAYING)
	{
		SDL_CloseAudio();
		if (this->open(this->freq, this->out_channels) < 0)
			return -1;
		this->play();
	}
	return 0;
}

void CPlayAudioBySDL::set_callback(std::function<void(Uint8 *stream, int len)> cb)
{
	callback_function = cb;
}

void CPlayAudioBySDL::queue_audio(const void* data, unsigned int len)
{
	throw "callback mode, no implement";
}

void CPlayAudioBySDL::mix_audio(Uint8 * dst, const Uint8 * src, Uint32 len, int volume)
{
	SDL_MixAudioFormat(dst, src, AUDIO_S16SYS, len, volume);
}

