#include "sound.h"

int sound_init_sample_spec(sound_t * sound,
                     uint32_t  format,
                     uint32_t  rate,
                     uint32_t channels)
{
    if (!sound) return -1;

    sound->sample_spec.format = format ? format : PA_SAMPLE_S16LE;
    sound->sample_spec.rate = rate ? rate : 44100;
    sound->sample_spec.channels = channels ? channels : 2;

    return 0;
}

int sound_create_capture_device(sound_t * sound)
{
    int error;

    sound->capture = pa_simple_new(NULL, LIBRARY_NAME, PA_STREAM_RECORD, NULL, "record", &sound->sample_spec, NULL, NULL, &error);
    if (!sound->capture)
        return -1;

    return 0;
}

int sound_create_playback_device(sound_t * sound)
{
    int error;

    sound->playback = pa_simple_new(NULL, LIBRARY_NAME, PA_STREAM_PLAYBACK, NULL, "play", &sound->sample_spec, NULL, NULL, &error);
    if (!sound->playback)
        return -1;

    return 0;
}

int sound_destroy_capture_sound_device(sound_t * sound)
{
    if (!sound->capture) return -1;

    pa_simple_free(sound->capture);

    return 0;
}

int sound_destroy_playback_sound_device(sound_t * sound)
{
    if (!sound->playback) return -1;

    int rc;
    int error;

    rc = pa_simple_drain(sound->playback, &error);
    if (rc < 0)
        return -1;

    pa_simple_free(sound->playback);

    return 0;
}

int sound_read_data(sound_t * sound, void * data, uint32_t length)
{
    int rc;
    int error;

    rc = pa_simple_read(sound->capture, data, length, &error);
    if (rc < 0)
        return -1;

    return 0;
}

int sound_write_data(sound_t * sound, void * data, uint32_t length)
{
    int rc;
    int error;

    rc = pa_simple_write(sound->playback, data, length, &error);
    if (rc < 0)
        return -1;

    return 0;
}