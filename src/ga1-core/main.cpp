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

ga_font* g_font = nullptr;
static void setup_piano(ga_frame_params* params);
static void set_root_path(const char* exepath);

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	ga_job::startup(0xffff, 256, 256);

	// Create objects for three phases of the frame: input, sim and output.
	ga_input* input = new ga_input();
	ga_sim* sim = new ga_sim();
	ga_output* output = new ga_output(input->get_window());

	// Create the default font:
	g_font = new ga_font("VeraMono.ttf", 64.0f, 512, 512);

	// Create camera.
	ga_camera* camera = new ga_camera({ 0.0f, 7.0f, 20.0f });
	ga_quatf rotation;
	rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(15.0f));
	camera->rotate(rotation);

	// Create an entity whose movement is driven by Lua script.
	ga_entity lua;
	lua.translate({ 0.0f, 2.0f, 1.0f });
	ga_lua_component lua_move(&lua, "data/scripts/move.lua");
	ga_cube_component lua_model(&lua, "data/textures/rpi.png");

	// create audio component
	ga_audio_component lua_audio(&lua);

	// add the note to be used by the piano
#ifdef _DEBUG
	lua_audio.add_sound("Debug/data/sounds/c4.mp3");
#else
	lua_audio.add_sound("Release/data/sounds/c4.mp3");
#endif
		
	sim->add_entity(&lua);

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
		setup_piano(&params);

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

static void setup_piano(ga_frame_params *params)
{
	if (ga_button("C", 160, 600, params).get_clicked(params))
	{

	}
	if (ga_button("C#", 220, 600, params).get_clicked(params))
	{

	}
	if (ga_button("D", 320, 600, params).get_clicked(params))
	{

	}
	if (ga_button("D#", 380, 600, params).get_clicked(params))
	{

	}
	if (ga_button("E", 480, 600, params).get_clicked(params))
	{

	}
	if (ga_button("F", 540, 600, params).get_clicked(params))
	{

	}
	if (ga_button("F#", 600, 600, params).get_clicked(params))
	{

	}
	if (ga_button("G", 700, 600, params).get_clicked(params))
	{

	}
	if (ga_button("G#", 760, 600, params).get_clicked(params))
	{

	}
	if (ga_button("A", 860, 600, params).get_clicked(params))
	{

	}
	if (ga_button("A#", 920, 600, params).get_clicked(params))
	{

	}
	if (ga_button("B", 1020, 600, params).get_clicked(params))
	{

	}
	if (ga_button("C", 1080, 600, params).get_clicked(params))
	{

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
