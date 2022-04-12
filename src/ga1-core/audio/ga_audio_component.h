#pragma once

#include "entity/ga_component.h"
#include <fmod.hpp>

class ga_audio_component : public ga_component
{
public:
	ga_audio_component(class ga_entity* ent);
	virtual ~ga_audio_component();

private:
	FMOD::System* _system;
};