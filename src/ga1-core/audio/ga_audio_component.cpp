#include "ga_audio_component.h"

#include <cassert>
#include <iostream>

inline void check_error(FMOD_RESULT* result) 
{
	if (*result != FMOD_OK) 
	{
		printf("ERROR: %d %s\n", *result, FMOD_ErrorString(*result));
	}
}

ga_audio_component::ga_audio_component(class ga_entity* ent, int num_channels) : ga_component(ent)
{
	FMOD::System_Create(&_system);
	_system->init(num_channels, FMOD_INIT_NORMAL, 0);

	for (int i = 0; i < num_channels; ++i)
	{
		FMOD::Channel* channel = 0;
		_channels.push_back(channel);
	}
}

ga_audio_component::~ga_audio_component()
{

}

void ga_audio_component::update(struct ga_frame_params* params)
{
	_system->update();
}

void ga_audio_component::add_sound(char* path)
{
	FMOD::Sound* sound;
	FMOD_RESULT result = _system->createSound(path, FMOD_DEFAULT, 0, &sound);
	check_error(&result);

	_sounds.push_back(sound);
}

void ga_audio_component::play_sound(int index)
{
	// check boundaries
	if (index > _sounds.size() || index < 0)
	{
		return;
	}

	// find the first free channel to play the sound
	for (int i = 0; i < _channels.size(); ++i)
	{
		bool playing = true;
		_channels[i]->isPlaying(&playing);
		
		// a channel is free if it is not playing anything at the moment
		if (!playing)
		{
			FMOD_RESULT result = _system->playSound(_sounds[index], NULL, false, &_channels[i]);
			check_error(&result);
			for (int j = 0; j < _channels.size(); ++j) {
				std::cout << _channels[j] << std::endl;
			}

			return;
		}
	}
}

