#ifndef SOUND_H
#define SOUND_H

#include "config.h"

#define CAPTURE_NAME "a_stream"

int create_capture_device(sound_t * sound);
int read_data(sound_t * sound, void * data, uint32_t length);
int destroy_capture_sound_device(sound_t * sound);

int create_playback_device(sound_t * sound);
int write_data(sound_t * sound, void * data, uint32_t length);
int destroy_playback_sound_device(sound_t * sound);

#endif /* SOUND_H */