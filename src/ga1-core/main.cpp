/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/ga_camera.h"
#include "framework/ga_compiler_defines.h"
#include "framework/ga_input.h"
#include "framework/ga_sim.h"
#include "framework/ga_output.h"
#include "gui/ga_button.h"
#include "gui/ga_checkbox.h"
#include "gui/ga_label.h"
#include "jobs/ga_job.h"

#include "entity/ga_entity.h"
#include "entity/ga_lua_component.h"

#include "graphics/ga_cube_component.h"
#include "graphics/ga_program.h"

#include "audio/ga_audio_component.h"

#include "gui/ga_font.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#if defined(GA_MINGW)
#include <unistd.h>
#endif

ga_font* g_font = nullptr, *g_font_alt = nullptr;
static void setup_piano(ga_frame_params* params, ga_audio_component* audio);
static void set_root_path(const char* exepath);

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	ga_job::startup(0xffff, 256, 256);

	// Create objects for three phases of the frame: input, sim and output.
	ga_input* input = new ga_input();
	ga_sim* sim = new ga_sim();
	ga_output* output = new ga_output(input->get_window());

	// Create the default fonts:
	g_font = new ga_font("VeraMono.ttf", 64.0f, 512, 512);
	g_font_alt = new ga_font("VeraMono.ttf", 32.0f, 512, 512);

	// Create camera.
	ga_camera* camera = new ga_camera({ 0.0f, 7.0f, 20.0f });
	ga_quatf rotation;
	rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(15.0f));
	camera->rotate(rotation);

	// Create an entity whose movement is driven by Lua script.
	ga_entity demo;
	demo.translate({ 0.0f, 2.0f, 1.0f });
	ga_cube_component demo_model(&demo, "data/textures/speaker.png");

	// create audio component
	ga_audio_component demo_audio(&demo);

	// add the note to be used by the piano
#ifdef _DEBUG
	demo_audio.add_sound("Debug/data/sounds/c4.wav");
	demo_audio.add_sound("Debug/data/sounds/guitar_c4.wav");
#else
	lua_audio.add_sound("Release/data/sounds/c4.wav");
	lua_audio.add_sound("Release/data/sounds/guitar_c4.wav");
#endif
		
	sim->add_entity(&demo);

	// Main loop:
	while (true)
	{
		// We pass frame state through the 3 phases using a params object.
		ga_frame_params params;

		// Gather user input and current time.
		if (!input->update(&params))
		{
			break;
		}

		// Update the camera.
		camera->update(&params);

		// Run gameplay.
		sim->update(&params);

		// create GUI for audio component
		setup_piano(&params, &demo_audio);

		// Perform the late update.
		sim->late_update(&params);

		// Draw to screen.
		output->update(&params);
	}

	delete output;
	delete sim;
	delete input;
	delete camera;

	ga_job::shutdown();

	return 0;
}

// piano demo globals
bool use_guitar, octave_up, distort, echo, tremolo, chorus;
float pan = 0.0f, volume = 1.0f, low_gain = 0.0f, mid_gain = 0.0f, high_gain = 0.0f;
int curr_index = 0;

static void setup_piano(ga_frame_params *params, ga_audio_component* audio)
{
	std::vector<ga_button> keys;

	keys.push_back(ga_button("C", 390, 600, params));
	keys.push_back(ga_button("C#", 420, 540, params, true));
	keys.push_back(ga_button("D", 450, 600, params));
	keys.push_back(ga_button("D#", 480, 540, params, true));
	keys.push_back(ga_button("E", 510, 600, params));
	keys.push_back(ga_button("F", 570, 600, params));
	keys.push_back(ga_button("F#", 600, 540, params, true));
	keys.push_back(ga_button("G", 630, 600, params));
	keys.push_back(ga_button("G#", 660, 540, params, true));
	keys.push_back(ga_button("A", 690, 600, params));
	keys.push_back(ga_button("A#", 720, 540, params, true));
	keys.push_back(ga_button("B", 750, 600, params));
	keys.push_back(ga_button("C", 810, 600, params));

	for (int i = 0; i < keys.size(); ++i)
	{
		if (keys[i].get_clicked(params))
		{
			int index = audio->play_sound(use_guitar ? 1 : 0);
			audio->set_channel_volume(index, volume);
			audio->set_channel_pan(index, pan);
			audio->set_channel_pitch(index, -12 + i + (octave_up ? 12 : 0));
			audio->set_channel_distortion(index, distort ? 1 : 0);
			audio->set_channel_echo(index, echo ? 400 : 0);
			audio->set_channel_tremolo(index, tremolo ? 1 : 0, 5);
			audio->set_channel_chorus(index, chorus ? 50 : 0);
			audio->set_channel_three_eq(index, low_gain, mid_gain, high_gain);
		}
	}

	if (ga_checkbox(use_guitar, "Use Guitar", 800, 680, params).get_clicked(params))
	{
		use_guitar = !use_guitar;
	}

	if (ga_checkbox(octave_up, "Up One Octave", 80, 80, params).get_clicked(params))
	{
		octave_up = !octave_up;
	}

	if (ga_checkbox(distort, "Distort (!)", 80, 160, params).get_clicked(params))
	{
		distort = !distort;
	}

	if (ga_checkbox(echo, "Echo", 80, 240, params).get_clicked(params))
	{
		echo = !echo;
	}

	if (ga_checkbox(tremolo, "Tremolo", 80, 320, params).get_clicked(params))
	{
		tremolo = !tremolo;
	}

	if (ga_checkbox(chorus, "Chorus", 80, 400, params).get_clicked(params))
	{
		chorus = !chorus;
	}

	if (ga_button("+", 70, 480, params, true).get_clicked(params))
	{
		low_gain = fmin(low_gain + 5.0f, 10.0f);
	}
	auto _ = ga_label("Lo", 62, 520, params, true);
	std::string low_string = std::to_string(low_gain);
	low_string.resize(low_gain >= 0.0f ? (low_gain < 10.0f ? 1 : 2) : (low_gain > -10.0f ? 2 : 3));
	_ = ga_label(low_string.c_str(), 62, 560, params, true);
	if (ga_button("-", 70, 600, params, true).get_clicked(params))
	{
		low_gain = fmax(low_gain - 5.0f, -80.f);
	}

	if (ga_button("+", 170, 480, params, true).get_clicked(params))
	{
		mid_gain = fmin(mid_gain + 5.0f, 10.0f);
	}
	_ = ga_label("Mid", 162, 520, params, true);
	std::string mid_string = std::to_string(mid_gain);
	mid_string.resize(mid_gain >= 0.0f ? (mid_gain < 10.0f ? 1 : 2) : (mid_gain > -10.0f ? 2 : 3));
	_ = ga_label(mid_string.c_str(), 162, 560, params, true);
	if (ga_button("-", 170, 600, params, true).get_clicked(params))
	{
		mid_gain = fmax(mid_gain - 5.0f, -80.f);
	}

	if (ga_button("+", 270, 480, params, true).get_clicked(params))
	{
		high_gain = fmin(high_gain + 5.0f, 10.0f);
	}
	_ = ga_label("Hi", 262, 520, params, true);
	std::string high_string = std::to_string(high_gain);
	high_string.resize(high_gain >= 0.0f ? (high_gain < 10.0f ? 1 : 2) : (high_gain > -10.0f ? 2 : 3));
	_ = ga_label(high_string.c_str(), 262, 560, params, true);
	if (ga_button("-", 270, 600, params, true).get_clicked(params))
	{
		high_gain = fmax(high_gain - 5.0f, -80.f);
	}

	if (ga_button("Reset Three EQ", 60, 640, params, true).get_clicked(params))
	{
		low_gain = 0.0f;
		mid_gain = 0.0f;
		high_gain = 0.0f;
	}

	_ = ga_label("Adjust Vol", 800, 80, params);
	std::string volume_string = std::to_string(volume);
	volume_string.resize(4);
	_ = ga_label(volume_string.c_str(), 1180, 75, params, true);
	if (ga_button("-", 760, 80, params).get_clicked(params))
	{
		volume = fmax(volume - 0.2f, 0.0f);
	}
	if (ga_button("+", 1140, 80, params).get_clicked(params))
	{
		volume = fmin(volume + 0.2f, 2.0f);
	}
	if (ga_button("Reset", 920, 120, params, true).get_clicked(params))
	{
		volume = 1.0f;
	}

	_ = ga_label("Adjust Pan", 800, 180, params);
	std::string pan_string = std::to_string(pan);
	pan < 0 ? pan_string.resize(5) : pan_string.resize(4);
	_ = ga_label(pan_string.c_str(), 1180, 175, params, true);
	if (ga_button("<", 760, 180, params).get_clicked(params))
	{
		pan = fmax(pan - 0.2f, -1.0f);
	}
	if (ga_button(">", 1140, 180, params).get_clicked(params))
	{
		pan = fmin(pan + 0.2f, 1.0f);
	}
	if (ga_button("Reset", 920, 220, params, true).get_clicked(params))
	{
		pan = 0.0f;
	}
}

char g_root_path[256];
static void set_root_path(const char* exepath)
{
#if defined(GA_MSVC)
	strcpy_s(g_root_path, sizeof(g_root_path), exepath);

	// Strip the executable file name off the end of the path:
	char* slash = strrchr(g_root_path, '\\');
	if (!slash)
	{
		slash = strrchr(g_root_path, '/');
	}
	if (slash)
	{
		slash[1] = '\0';
	}
#elif defined(GA_MINGW)
	char* cwd;
	char buf[PATH_MAX + 1];
	cwd = getcwd(buf, PATH_MAX + 1);
	strcpy_s(g_root_path, sizeof(g_root_path), cwd);

	g_root_path[strlen(cwd)] = '/';
	g_root_path[strlen(cwd) + 1] = '\0';
#endif
}
