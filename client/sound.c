#include "sound.h"

int create_capture_device(sound_t * sound)
{
    int error;

    sound->sample_spec.format = PA_SAMPLE_S16LE;
    sound->sample_spec.rate = 44100;
    sound->sample_spec.channels = 2;

    sound->device = pa_simple_new(NULL, CAPTURE_NAME, PA_STREAM_RECORD, NULL, "record", &sound->sample_spec, NULL, NULL, &error);
    if (!sound->device)
        return -1;

    return 0;
}

int create_playback_device(sound_t * sound)
{
    int error;

    sound->sample_spec.format = PA_SAMPLE_S16LE;
    sound->sample_spec.rate = 44100;
    sound->sample_spec.channels = 2;

    sound->device = pa_simple_new(NULL, CAPTURE_NAME, PA_STREAM_PLAYBACK, NULL, "play", &sound->sample_spec, NULL, NULL, &error);
    if (!sound->device)
        return -1;

    return 0;
}

int destroy_capture_sound_device(sound_t * sound)
{
    if (!sound->device) return -1;

    pa_simple_free(sound->device);
    memset(sound, 0, sizeof(*sound));

    return 0;
}

int destroy_playback_sound_device(sound_t * sound)
{
    if (!sound->device) return -1;

    int rc;
    int error;

    rc = pa_simple_drain(sound->device, &error);
    if (rc < 0)
        return -1;

    pa_simple_free(sound->device);
    memset(sound, 0, sizeof(*sound));

    return 0;
}

int read_data(sound_t * sound, void * data, uint32_t length)
{
    int rc;
    int error;

    rc = pa_simple_read(sound->device, data, length, &error);
    if (rc < 0)
        return -1;

    return 0;
}

int write_data(sound_t * sound, void * data, uint32_t length)
{
    int rc;
    int error;

    rc = pa_simple_write(sound->device, data, length, &error);
    if (rc < 0)
        return -1;

    return 0;
}