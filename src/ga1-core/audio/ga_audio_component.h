#pragma once

#include "entity/ga_component.h"
#include <fmod.hpp>
#include <fmod_errors.h>

class ga_audio_component : public ga_component
{
public:
	ga_audio_component(class ga_entity* ent, int num_channels=100);
	virtual ~ga_audio_component();
	virtual void update(struct ga_frame_params* params) override;

	void add_sound(char* path);
	void play_sound(int index);

private:
	FMOD::System* _system;
	std::vector<FMOD::Channel*> _channels;
	std::vector<FMOD::Sound*> _sounds;
};