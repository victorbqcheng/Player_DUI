#include "pch.h"
#include "CPlayAudioBySDL2.h"

int CPlayAudioBySDL2::open(int freq, int out_channels)
{
	int flags = SDL_INIT_AUDIO | SDL_INIT_TIMER;
	SDL_Init(flags);

	SDL_AudioSpec wanted_spec = { 0 };
	wanted_spec.freq = freq * 1;// p_codec_ctx->sample_rate;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = out_channels; //out_channels;
	wanted_spec.silence = 0;
	wanted_spec.samples = 1024; //p_codec_ctx->frame_size;
	wanted_spec.callback = NULL;
	wanted_spec.userdata = NULL;

	//if (SDL_OpenAudio(&wanted_spec, NULL) < 0)
	devID = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if ( devID <= 0)
	{
		return -1;
	}
	return 0;
}

void CPlayAudioBySDL2::close()
{
	SDL_CloseAudioDevice(devID);
}

int CPlayAudioBySDL2::play()
{
	SDL_PauseAudioDevice(devID, 0);
	return 0;
}

void CPlayAudioBySDL2::stop()
{
	SDL_PauseAudioDevice(devID, 1);
}

void CPlayAudioBySDL2::pause()
{
	SDL_PauseAudioDevice(devID, 1);
}

void CPlayAudioBySDL2::continue_play()
{
	SDL_PauseAudioDevice(devID, 0);
}

void CPlayAudioBySDL2::set_callback(std::function<void(Uint8 *stream, int len)> cb)
{
	throw "regular mode, no callback";
}

void CPlayAudioBySDL2::queue_audio(const void* data, unsigned int len)
{
	int ret = SDL_QueueAudio(devID, data, len);
	//SDL_MixAudio(NULL, NULL, 0, 128);
	SDL_GetQueuedAudioSize(devID);
}

//

