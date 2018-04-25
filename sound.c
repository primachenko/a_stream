#include "sound.h"

int create_capture_device(sound_t ** sound_out)
{
    int error;

    sound_t * sound = malloc(sizeof(sound_t));
    if (!sound) return -1;

    sound->sample_spec.format = PA_SAMPLE_S16LE;
    sound->sample_spec.rate = 44100;
    sound->sample_spec.channels = 2;

    sound->device = pa_simple_new(NULL, CAPTURE_NAME, PA_STREAM_RECORD, NULL, "record", &sound->sample_spec, NULL, NULL, &error);
    if (!sound->device)
        return -1;

    *sound_out = sound;

    return 0;
}

int create_playback_device(sound_t ** sound_out)
{
    int error;

    sound_t * sound = malloc(sizeof(sound_t));
    if (!sound) return -1;

    sound->sample_spec.format = PA_SAMPLE_S16LE;
    sound->sample_spec.rate = 44100;
    sound->sample_spec.channels = 2;

    sound->device = pa_simple_new(NULL, CAPTURE_NAME, PA_STREAM_PLAYBACK, NULL, "play", &sound->sample_spec, NULL, NULL, &error);
    if (!sound->device)
        return -1;

    *sound_out = sound;

    return 0;
}

int destroy_capture_sound_device(sound_t * sound)
{
    if (!sound->device) return -1;

    pa_simple_free(sound->device);
    memset(sound, 0, sizeof(*sound));
    free(sound);

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
    free(sound);

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

int write_wav_header(FILE*   file_p)
{
    int ret;

    wavfile_header_t wav_header;
    memset(&wav_header, 0, sizeof(wav_header));

    size_t write_count;

    wav_header.ChunkID[0] = 'R';
    wav_header.ChunkID[1] = 'I';
    wav_header.ChunkID[2] = 'F';
    wav_header.ChunkID[3] = 'F';

    wav_header.Format[0] = 'W';
    wav_header.Format[1] = 'A';
    wav_header.Format[2] = 'V';
    wav_header.Format[3] = 'E';

    wav_header.Subchunk1ID[0] = 'f';
    wav_header.Subchunk1ID[1] = 'm';
    wav_header.Subchunk1ID[2] = 't';
    wav_header.Subchunk1ID[3] = ' ';

    wav_header.Subchunk1Size = SUBCHUNK1SIZE;
    wav_header.AudioFormat = AUDIO_FORMAT;
    wav_header.NumChannels = NUM_CHANNELS;
    wav_header.SampleRate = 44100;
    wav_header.ByteRate = BYTE_RATE;
    wav_header.BlockAlign = BLOCK_ALIGN;
    wav_header.BitsPerSample = BITS_PER_SAMPLE;

    wav_header.Subchunk2ID[0] = 'd';
    wav_header.Subchunk2ID[1] = 'a';
    wav_header.Subchunk2ID[2] = 't';
    wav_header.Subchunk2ID[3] = 'a';

    write_count = fwrite(&wav_header, sizeof(wavfile_header_t), 1, file_p);

    ret = (1 != write_count)? -1 : 0;

    return ret;
}