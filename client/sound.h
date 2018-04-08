#ifndef SOUND_H
#define SOUND_H

#include "config.h"

#define LIBRARY_NAME "a_stream"

int sound_init_sample_spec(sound_t * sound,
                     uint32_t  format,
                     uint32_t  rate,
                     uint32_t channels);
int sound_create_capture_device(sound_t * sound);
int sound_create_playback_device(sound_t * sound);
int sound_read_data(sound_t * sound,
              void    * data,
              uint32_t  length);
int sound_write_data(sound_t * sound,
               void    * data,
               uint32_t  length);
int sound_destroy_capture_sound_device(sound_t * sound);
int sound_destroy_playback_sound_device(sound_t * sound);

#endif /* SOUND_H */