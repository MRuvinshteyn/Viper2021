#include "ga_audio_component.h"
#include <cstdarg>
#include <stdarg.h>

#include <cassert>

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
	_system->close();
}

void ga_audio_component::update(struct ga_frame_params* params)
{
	_system->update();
}

// add a sound clip to the collection of available sounds
unsigned int ga_audio_component::add_sound(char* path)
{
	FMOD::Sound* sound;
	FMOD_RESULT result = _system->createSound(path, FMOD_DEFAULT, 0, &sound);
	check_error(&result);

	_sounds.push_back(sound);

	return _sounds.size() - 1;
}

// remove a specific sound clip from the component
void ga_audio_component::remove_sound(unsigned int index)
{
	if (index >= _sounds.size())
	{
		return;
	}

	_sounds.erase(_sounds.begin() + index);
}

// play a specific sound clip
int ga_audio_component::play_sound(unsigned int index)
{
	// check boundaries
	if (index >= _sounds.size())
	{
		return -1;
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

			return i;
		}
	}
	return -1;
}

// pause a specific channel
void ga_audio_component::pause_channel(unsigned int index)
{
	if (index >= _channels.size())
	{
		return;
	}

	_channels[index]->setPaused(true);
}

// resume a specific channel
void ga_audio_component::resume_channel(unsigned int index)
{
	if (index >= _channels.size())
	{
		return;
	}

	_channels[index]->setPaused(false);
}

// stop a specific channel
void ga_audio_component::stop_channel(unsigned int index)
{
	if (index >= _channels.size())
	{
		return;
	}

	_channels[index]->stop();
}

// set the time position of a specific channel
void ga_audio_component::set_channel_position(unsigned int index, unsigned int position_ms)
{
	if (index >= _channels.size())
	{
		return;
	}

	_channels[index]->setPosition(position_ms, FMOD_TIMEUNIT_MS);
}

// test the boundaries of parameters passed to an effect
bool ga_audio_component::test_effect_boundaries(unsigned int index, int num_parameters, ...)
{
	// check index boundaries
	if (index >= _channels.size())
	{
		return false;
	}

	// only add effect for channels with a sound in queue
	bool playing;
	_channels[index]->isPlaying(&playing);
	if (!playing)
	{
		return false;
	}

	va_list arguments;
	va_start(arguments, num_parameters);
	
	for (int i = 0; i + 2 < num_parameters * 3; i += 3)
	{
		double effect = va_arg(arguments, double);
		double min = va_arg(arguments, double);
		double max = va_arg(arguments, double);
		if (effect < min || effect > max)
		{
			return false;
		}
	}
	va_end(arguments);
	
	return true;
}

void ga_audio_component::add_effect(unsigned int index, FMOD_DSP_TYPE type, int num_parameters, ...)
{
	FMOD::DSP* dsp;
	_system->createDSPByType(type, &dsp);

	va_list arguments;
	va_start(arguments, num_parameters);

	for (int i = 0; i + 1 < num_parameters * 2; i += 2)
	{
		int parameter = va_arg(arguments, int);

		FMOD_DSP_PARAMETER_DESC* desc;
		dsp->getParameterInfo(parameter, &desc);

		FMOD_DSP_PARAMETER_TYPE parameter_type = desc->type;

		// set parameter according to type
		switch (parameter_type) {
		case FMOD_DSP_PARAMETER_TYPE_FLOAT:
		{
			double level_float = va_arg(arguments, double);

			dsp->setParameterFloat(parameter, level_float);
		}
			break;

		case FMOD_DSP_PARAMETER_TYPE_BOOL:
		{
			bool level_bool = va_arg(arguments, bool);

			dsp->setParameterBool(parameter, level_bool);
		}
			break;

		case FMOD_DSP_PARAMETER_TYPE_INT:
		{
			int level_int = va_arg(arguments, int);

			dsp->setParameterInt(parameter, level_int);
		}
			break;
		}
	}

	// check the number of DSP's already in the channel
	// and place the new one in the next index
	int num_dsps;
	_channels[index]->getNumDSPs(&num_dsps);
	_channels[index]->addDSP(num_dsps, dsp);
}

// change the volume of a specific channel
void ga_audio_component::set_channel_volume(unsigned int index, float volume)
{
	if (!test_effect_boundaries(index, 1, volume, 0, std::numeric_limits<float>::max()))
	{
		return;
	}

	_channels[index]->setVolume(volume);
}

// change the pan of a specific channel
void ga_audio_component::set_channel_pan(unsigned int index, float pan)
{
	if (!test_effect_boundaries(index, 1, pan, -1.0f, 1.0f))
	{
		return;
	}

	_channels[index]->setPan(pan);
}

// change the frequency of a specific channel
void ga_audio_component::set_channel_frequency(unsigned int index, float frequency)
{
	if (index >= _channels.size())
	{
		return;
	}

	_channels[index]->setFrequency(frequency);
}

// get the timestamp of a specific channel
int ga_audio_component::get_channel_position(unsigned int index)
{
	if (index >= _channels.size())
	{
		return -1;
	}

	unsigned int position;
	_channels[index]->getPosition(&position, FMOD_TIMEUNIT_MS);
	return position;
}

// get the volume of a specific channel
float ga_audio_component::get_channel_volume(unsigned int index)
{
	if (index >= _channels.size())
	{
		return -1.0f;
	}

	float volume;
	_channels[index]->getVolume(&volume);
	return volume;
}

// get the frequency of a specific channel
float ga_audio_component::get_channel_frequency(unsigned int index)
{
	if (index >= _channels.size())
	{
		return -1.0f;
	}

	float frequency;
	_channels[index]->getFrequency(&frequency);
	return frequency;
}

#pragma region DSP Effects

// add chorus effect to a specific channel
void ga_audio_component::set_channel_chorus(unsigned int index, float depth, float rate, float mix)
{
	if (!test_effect_boundaries(index, 3,
		depth, 0.0f, 100.0f,
		rate, 0.0f, 20.0f,
		mix, 0.0f, 100.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_CHORUS, 3,
		FMOD_DSP_CHORUS_DEPTH, depth,
		FMOD_DSP_CHORUS_RATE, rate,
		FMOD_DSP_CHORUS_MIX, mix);
}

// add compressor effect to specific channel
void ga_audio_component::set_channel_compressor(unsigned int index, float threshold, float ratio, float attack, float release,
	float gain_makeup)
{
	if (!test_effect_boundaries(index, 5,
		threshold, -60.0f, 0.0f,
		ratio, 1.0f, 50.0f,
		attack, 0.1f, 500.0f,
		release, 10.0f, 5000.0f,
		gain_makeup, -30.0f, 30.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_COMPRESSOR, 5,
		FMOD_DSP_COMPRESSOR_THRESHOLD, threshold,
		FMOD_DSP_COMPRESSOR_RATIO, ratio,
		FMOD_DSP_COMPRESSOR_ATTACK, attack,
		FMOD_DSP_COMPRESSOR_RELEASE, release,
		FMOD_DSP_COMPRESSOR_GAINMAKEUP, gain_makeup);
}

// add distortion effect to a specific channel
void ga_audio_component::set_channel_distortion(unsigned int index, float level)
{
	if (!test_effect_boundaries(index, 1, 
		level, 0.0f, 1.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_DISTORTION, 1, 
		FMOD_DSP_DISTORTION_LEVEL, level);
}

// add echo effect to a specific channel
void ga_audio_component::set_channel_echo(unsigned int index, float delay, float feedback, float wet_mix, float dry_mix)
{
	if (!test_effect_boundaries(index, 4, 
		delay, 10.0f, 5000.0f,
		feedback, 0.0f, 100.0f,
		wet_mix, -80.0f, 10.0f,
		dry_mix, -80.0f, 10.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_ECHO, 4, 
		FMOD_DSP_ECHO_DELAY, delay,
		FMOD_DSP_ECHO_FEEDBACK, feedback,
		FMOD_DSP_ECHO_WETLEVEL, wet_mix,
		FMOD_DSP_ECHO_DRYLEVEL, dry_mix);
}

// change gain of a specific channel
void ga_audio_component::set_channel_fader(unsigned int index, float gain)
{
	if (!test_effect_boundaries(index, 1,
		gain, -80.0f, 10.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_FADER, 1,
		FMOD_DSP_FADER_GAIN, gain);
}

// add flange effect to a specific channel
void ga_audio_component::set_channel_flange(unsigned int index, float depth, float rate, float mix)
{
	if (!test_effect_boundaries(index, 3,
		depth, 0.01f, 1.0f,
		rate, 0.0f, 20.0f,
		mix, 0.0f, 100.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_FLANGE, 3,
		FMOD_DSP_FLANGE_DEPTH, depth,
		FMOD_DSP_FLANGE_RATE, rate,
		FMOD_DSP_FLANGE_MIX, mix);
}

// add limiter effect to a specific channel
void ga_audio_component::set_channel_limiter(unsigned int index, float ceiling, float release, float maximizer_gain)
{
	if (!test_effect_boundaries(index, 3,
		ceiling, -12.0f, 0.0f,
		release, 1.0f, 1000.0f,
		maximizer_gain, 0.0f, 12.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_LIMITER, 3,
		FMOD_DSP_LIMITER_CEILING, ceiling,
		FMOD_DSP_LIMITER_RELEASETIME, release,
		FMOD_DSP_LIMITER_MAXIMIZERGAIN, maximizer_gain);
}

// add IT lowpass effect to a specific channel
void ga_audio_component::set_channel_lowpass(unsigned int index, float cutoff, float resonance)
{
	if (!test_effect_boundaries(index, 2, 
		cutoff, 1000.0f, 22000.0f,
		resonance, 0.0f, 127.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_ITLOWPASS, 2, 
		FMOD_DSP_ITLOWPASS_CUTOFF, cutoff,
		FMOD_DSP_ITLOWPASS_RESONANCE, resonance);
}

// normalize the sound of a channel
void ga_audio_component::set_channel_normalize(unsigned int index, float threshold, float fade_time, float max_amp)
{
	if (!test_effect_boundaries(index, 3,
		threshold, 0.0f, 1.0f,
		fade_time, 0.0f, 20000.0f,
		max_amp, 1.0f, 100000.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_NORMALIZE, 3,
		FMOD_DSP_NORMALIZE_THRESHOLD, threshold,
		FMOD_DSP_NORMALIZE_FADETIME, fade_time,
		FMOD_DSP_NORMALIZE_MAXAMP, max_amp);
}

// add oscillator effect to a specific channel
void ga_audio_component::set_channel_oscillator(unsigned int index, float type, float rate)
{
	if (!test_effect_boundaries(index, 2,
		(int)type, 0.0f, 6.0f,
		rate, 0.0f, 22000.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_OSCILLATOR, 2,
		FMOD_DSP_OSCILLATOR_TYPE, (int)type,
		FMOD_DSP_OSCILLATOR_RATE, rate);
}

// change the pitch of a specific channel
void ga_audio_component::set_channel_pitch(unsigned int index, float semitones, float fftsize)
{
	if (!test_effect_boundaries(index, 2,
		semitones, -12.0f, 12.0f,
		fftsize, 256.0f, 4096.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_PITCHSHIFT, 2,
		FMOD_DSP_PITCHSHIFT_PITCH, powf(2.0f, semitones / 12.0f),
		FMOD_DSP_PITCHSHIFT_FFTSIZE, fftsize);
}

// add SFX reverb effect to a specific channel
void ga_audio_component::set_channel_reverb(unsigned int index, float decay_time, float early_delay, float late_delay, 
	float hf_reference, float hf_ratio, float diffusion, float density, float low_shelf_frequency, float low_shelf_gain, 
	float high_cut, float early_late_mix, float wet_level, float dry_level)
{
	if (!test_effect_boundaries(index, 13,
		decay_time, 100.0f, 20000.0f,
		early_delay, 0.0f, 300.0f,
		late_delay, 0.0f, 100.0f,
		hf_reference, 20.0f, 20000.0f,
		hf_ratio, 10.0f, 100.0f,
		diffusion, 10.0f, 100.0f,
		density, 10.0f, 100.0f,
		low_shelf_frequency, 20.0f, 1000.0f,
		low_shelf_gain, -36.0f, 12.0f,
		high_cut, 20.0f, 20000.0f,
		early_late_mix, 0.0f, 100.0f,
		wet_level, -80.0f, 20.0f,
		dry_level, -80.0f, 20.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_SFXREVERB, 13,
		FMOD_DSP_SFXREVERB_DECAYTIME, decay_time,
		FMOD_DSP_SFXREVERB_EARLYDELAY, early_delay,
		FMOD_DSP_SFXREVERB_LATEDELAY, late_delay,
		FMOD_DSP_SFXREVERB_HFREFERENCE, hf_reference,
		FMOD_DSP_SFXREVERB_HFDECAYRATIO, hf_ratio,
		FMOD_DSP_SFXREVERB_DIFFUSION, diffusion,
		FMOD_DSP_SFXREVERB_DENSITY, density,
		FMOD_DSP_SFXREVERB_LOWSHELFFREQUENCY, low_shelf_frequency,
		FMOD_DSP_SFXREVERB_LOWSHELFGAIN, low_shelf_gain,
		FMOD_DSP_SFXREVERB_HIGHCUT, high_cut,
		FMOD_DSP_SFXREVERB_EARLYLATEMIX, early_late_mix,
		FMOD_DSP_SFXREVERB_WETLEVEL, wet_level,
		FMOD_DSP_SFXREVERB_DRYLEVEL);
}

// add 3-way equalizer effect to a specific channel
void ga_audio_component::set_channel_three_eq(unsigned int index, float low_gain, float mid_gain, float high_gain, 
	float low_crossover, float high_crossover, FMOD_DSP_THREE_EQ_CROSSOVERSLOPE_TYPE slope)
{
	if (!test_effect_boundaries(index, 5,
		low_gain, -80.0f, 10.0f,
		mid_gain, -80.0f, 10.0f,
		high_gain, -80.0f, 10.0f,
		low_crossover, 10.0f, 22000.0f,
		high_crossover, 10.0f, 22000.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_THREE_EQ, 5,
		FMOD_DSP_THREE_EQ_LOWGAIN, low_gain,
		FMOD_DSP_THREE_EQ_MIDGAIN, mid_gain,
		FMOD_DSP_THREE_EQ_HIGHGAIN, high_gain,
		FMOD_DSP_THREE_EQ_LOWCROSSOVER, low_crossover,
		FMOD_DSP_THREE_EQ_HIGHCROSSOVER, high_crossover,
		FMOD_DSP_THREE_EQ_CROSSOVERSLOPE, slope);
}

// add tremolo effect to a specific channel
void ga_audio_component::set_channel_tremolo(unsigned int index, float depth, float frequency, float shape, float skew,
	float duty, float phase, float square, float spread)
{
	if (!test_effect_boundaries(index, 8, 
		depth, 0.0f, 1.0f, 
		frequency, 0.1f, 20.0f,
		shape, 0.0f, 1.0f,
		skew, -1.0f, 1.0f,
		duty, 0.0f, 1.0f,
		phase, 0.0f, 1.0f,
		square, 0.0f, 1.0f,
		spread, -1.0f, 1.0f))
	{
		return;
	}

	add_effect(index, FMOD_DSP_TYPE_TREMOLO, 8, 
		FMOD_DSP_TREMOLO_DEPTH, depth, 
		FMOD_DSP_TREMOLO_FREQUENCY, frequency,
		FMOD_DSP_TREMOLO_SHAPE, shape,
		FMOD_DSP_TREMOLO_SKEW, skew,
		FMOD_DSP_TREMOLO_DUTY, duty,
		FMOD_DSP_TREMOLO_PHASE, phase,
		FMOD_DSP_TREMOLO_SQUARE, square,
		FMOD_DSP_TREMOLO_SPREAD, spread);
}

#pragma endregion