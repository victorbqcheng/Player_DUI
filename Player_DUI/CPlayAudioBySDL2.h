#pragma once
#include "CPlayAudioBySDL.h"
class CPlayAudioBySDL2 : public CPlayAudioBySDL
{
public:
	virtual int open(int freq, int out_channels) override;
	virtual void close() override;

	virtual int play() override;
	virtual void stop() override;

	virtual void pause() override;
	virtual void continue_play() override;

	virtual MODE get_mode()
	{
		return mode;
	}

	virtual void set_callback(std::function<void(Uint8 *stream, int len)> cb) override;
	virtual void queue_audio(const void* data, unsigned int len) override;
private:
	MODE const mode = REGULAR;
	unsigned int devID;
};

