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

#pragma region Basic Control

	/// <summary>
	/// Add a sound clip to the component
	/// </summary>
	/// <param name="path">File path for the sound clip</param>
	/// <returns>Index of the sound clip</returns>
	unsigned int add_sound(char* path);
	/// <summary>
	/// Remove a sound clip from the component
	/// </summary>
	/// <param name="index">Index of the sound clip to remove</param>
	void remove_sound(unsigned int index);
	/// <summary>
	/// Play a specific sound clip
	/// </summary>
	/// <param name="index">Index of the sound clip to play</param>
	/// <returns>Index of the channel that is playing the sound</returns>
	int play_sound(unsigned int index);
	/// <summary>
	/// Pause a channel
	/// </summary>
	/// <param name="index">Index of the channel to pause</param>
	void pause_channel(unsigned int index);
	/// <summary>
	/// Un-pause a channel
	/// </summary>
	/// <param name="index">Index of the channel to resume</param>
	void resume_channel(unsigned int index);
	/// <summary>
	/// Stop a channel
	/// </summary>
	/// <param name="index">Index of the channel to stop</param>
	void stop_channel(unsigned int index);

	/// <summary>
	/// Change the timestamp of a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="position_ms">New position of the channel in milliseconds</param>
	void set_channel_position(unsigned int index, unsigned int position_ms);
	/// <summary>
	/// Change the volume of a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="volume">New volume of the channel</param>
	void set_channel_volume(unsigned int index, float volume);
	/// <summary>
	/// Change the pan of a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="pan">New pan of the channel [-1, 1]</param>
	void set_channel_pan(unsigned int index, float pan);
	/// <summary>
	/// Change the frequency of a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="frequency">New frequency of the channel in Hz</param>
	void set_channel_frequency(unsigned int index, float frequency);

	/// <summary>
	/// Get the current timestamp of a channel
	/// </summary>
	/// <param name="index">Index of the channel</param>
	/// <returns>Timestamp of the channel in milliseconds</returns>
	int get_channel_position(unsigned int index);
	/// <summary>
	/// Get the current volume of a channel
	/// </summary>
	/// <param name="index">Index of the channel</param>
	/// <returns>Volume of the channel</returns>
	float get_channel_volume(unsigned int index);
	/// <summary>
	/// Get the frequency of a channel
	/// </summary>
	/// <param name="index">Index of the channel</param>
	/// <returns>Frequency of the channel in Hz</returns>
	float get_channel_frequency(unsigned int index);

#pragma endregion

#pragma region DSP

	/*
	* DSP effects not included:
	*   - Channel Mix
	*	- Convolution Reverb
	*   - Delay
	*   - FFT
	*   - Multiband Equalizer
	*   - Object Panner
	*   - Pan
	*   - Return
	*   - Send
	*/

	/// <summary>
	/// Add a chorus effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="depth">Modulation depth [0, 100]</param>
	/// <param name="rate">Modulation rate [0, 20]</param>
	/// <param name="mix">Wet signal in mix [0, 100]</param>
	void set_channel_chorus(unsigned int index, float depth = 3.0f, float rate = 0.8f, float mix = 100.0f);
	/// <summary>
	/// Add a compressor effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="threshold">Threshold level [-60, 0]</param>
	/// <param name="ratio">Compression ratio [1, 50]</param>
	/// <param name="attack">Attack time [0.1, 500]</param>
	/// <param name="release">Release time [10, 5000]</param>
	/// <param name="gain_makeup">Make-up gain applied after limiting [-30, 30]</param>
	void set_channel_compressor(unsigned int index, float threshold = 0.0f, float ratio = 2.5f, float attack = 20.0f,
		float release = 100.0f, float gain_makeup = 0.0f);
	/// <summary>
	/// Add a distortion effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="level">Distortion value [0, 1]</param>
	void set_channel_distortion(unsigned int index, float level = 0.5f);
	/// <summary>
	/// Add an echo effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="delay">Echo delay [1, 5000]</param>
	/// <param name="feedback">Echo decay per delay (100 = no decay, 0 = full decay) [0, 100]</param>
	/// <param name="wet_mix">Volume of echo signal to pass to output [-80, 10]</param>
	/// <param name="dry_mix">Original sound volume [-80, 10]</param>
	void set_channel_echo(unsigned int index, float delay = 500.0f, float feedback = 50.0f, float wet_mix = 0.0f, 
		float dry_mix = 0.0f);
	/// <summary>
	/// Add a fader effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="gain">Signal gain [-80, 10]</param>
	void set_channel_fader(unsigned int index, float gain = 0.0f);
	/// <summary>
	/// Add a flange effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="depth">Flange depth [0.01, 1]</param>
	/// <param name="rate">Flange speed [0, 20]</param>
	/// <param name="mix">Percentage of wet signal in mix [0, 100]</param>
	void set_channel_flange(unsigned int index, float depth = 1.0f, float rate = 0.1f, float mix = 50.0f);
	/// <summary>
	/// Add a limiter effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="ceiling">Maximum level of the output signal [-12, 0]</param>
	/// <param name="release">Time to return the gain reduction to full in ms [1, 1000]</param>
	/// <param name="maximizer_gain">Maximum amplification allowed [0, 12]</param>
	void set_channel_limiter(unsigned int index, float ceiling = 0.0f, float release = 10.0f, float maximizer_gain = 0.0f);
	/// <summary>
	/// Add an IT lowpass effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="cutoff">Lowpass cutoff frequency [1, 22000]</param>
	/// <param name="resonance">Lowpass resonance Q value [0, 127]</param>
	void set_channel_lowpass(unsigned int index, float cutoff = 5000.0f, float resonance = 1.0f);
	/// <summary>
	/// Add a normalization effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="threshold">Lower volume range threshold to ignore [0, 1]</param>
	/// <param name="fade_time">Time to ramp the silence to full [0, 20000]</param>
	/// <param name="max_amp">Maximum amplification allowed [1, 100000]</param>
	void set_channel_normalize(unsigned int index, float threshold = 0.1f, float fade_time = 5000.0f, float max_amp = 20.0f);
	/// <summary>
	/// Add an oscillator effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="type">Waveform type (0 = sine, 1 = square, 2 = sawup, 3 = sawdown, 4 = triangle, 5 = noise)</param>
	/// <param name="rate">Frequency of the tone [0, 22000]</param>
	void set_channel_oscillator(unsigned int index, float type = 0.0f, float rate = 220.0f);
	/// <summary>
	/// Add a pitch shift effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="semitones">Number of semitones to shift [-12, 12]</param>
	/// <param name="fftsize">FFT window size, increase to reduce 'smearing'</param>
	void set_channel_pitch(unsigned int index, float semitones = 0.0f, float fftsize = 1024.0f);
	/// <summary>
	/// Add an SFX reverb effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="decay_time">Reverberation decay time at low-frequencies [100, 20000]</param>
	/// <param name="early_delay">Delay time of first reflection [0, 300]</param>
	/// <param name="late_delay">Late reverberation delay time relative to first reflection in milliseconds [0, 100]</param>
	/// <param name="hf_reference">Reference frequency for high-frequency decay [20, 20000]</param>
	/// <param name="hf_ratio">High-frequency decay time relative to decay time [10, 100]</param>
	/// <param name="diffusion">Reverberation diffusion [10, 100]</param>
	/// <param name="density">Reverberation density [10, 100]</param>
	/// <param name="low_shelf_frequency">Transition frequency of low-shelf filter [20, 1000]</param>
	/// <param name="low_shelf_gain">Gain of low-shelf filter [-36, 12]</param>
	/// <param name="high_cut">Cutoff frequency of low-pass filter [20, 20000]</param>
	/// <param name="early_late_mix">Blend ratio of late reverb to early reflections [0, 100]</param>
	/// <param name="wet_level">Reverb signal level [-80, 20]</param>
	/// <param name="dry_level">Dry signal level [-80, 20]</param>
	void set_channel_reverb(unsigned int index, float decay_time = 1500.0f, float early_delay = 20.0f, 
		float late_delay = 40.0f, float hf_reference = 5000.0f, float hf_ratio = 50.0f, float diffusion = 50.0f, 
		float density = 50.0f, float low_shelf_frequency = 250.0f, float low_shelf_gain = 0.0f, float high_cut = 20000.0f, 
		float early_late_mix = 50.0f, float wet_level = -6.0f, float dry_level = 0.0f);
	/// <summary>
	/// Add a three EQ effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="low_gain">Low frequency gain [-80, 10]</param>
	/// <param name="mid_gain">Mid frequency gain [-80, 10]</param>
	/// <param name="high_gain">High frequency gain [-80, 10]</param>
	/// <param name="low_crossover">Low-to-mid crossover frequency [10, 22000]</param>
	/// <param name="high_crossover">Mid-to-high crossover frequency [10, 22000]</param>
	/// <param name="slope">Crossover slope type</param>
	void set_channel_three_eq(unsigned int index, float low_gain = 0.0f, float mid_gain = 0.0f, float high_gain = 0.0f,
		float low_crossover = 400.0f, float high_crossover = 4000.0f, 
		FMOD_DSP_THREE_EQ_CROSSOVERSLOPE_TYPE slope = FMOD_DSP_THREE_EQ_CROSSOVERSLOPE_24DB);
	/// <summary>
	/// Add a tremolo effect to a channel
	/// </summary>
	/// <param name="index">Index of the channel to modify</param>
	/// <param name="depth">Tremolo depth [0, 1]</param>
	/// <param name="frequency">LFO frequency [0.1, 20]</param>
	/// <param name="shape">LFO shape morph between triangle and sine [0, 1]</param>
	/// <param name="skew">Time-skewing of LFO cycle [-1, 1]</param>
	/// <param name="duty">LFO on-time [0, 1]</param>
	/// <param name="phase">Instantaneous LFO phase [0, 1]</param>
	/// <param name="square">Flatness of the LFO shape [0, 1]</param>
	/// <param name="spread">Rotation/auto-pan effect [-1, 1]</param>
	void set_channel_tremolo(unsigned int index, float depth = 1.0f, float frequency = 5.0f, float shape = 0.0f, 
		float skew = 0.0f, float duty = 0.5f, float phase = 0.0f, float square = 0.0f, float spread = 0.0f);

#pragma endregion

private:
	bool test_effect_boundaries(unsigned int index, int num_parameters, ...);
	void add_effect(unsigned int index, FMOD_DSP_TYPE type, int num_parameters, ...);

	FMOD::System* _system;
	std::vector<FMOD::Channel*> _channels;
	std::vector<FMOD::Sound*> _sounds;
};