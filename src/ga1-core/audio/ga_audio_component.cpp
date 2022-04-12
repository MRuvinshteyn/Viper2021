#include "ga_audio_component.h"

ga_audio_component::ga_audio_component(class ga_entity* ent) : ga_component(ent)
{
	FMOD::System_Create(&_system);
}

ga_audio_component::~ga_audio_component()
{

}