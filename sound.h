#ifndef SOUND_H
#define SOUND_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pulse/simple.h>
#include <pulse/error.h>

typedef struct
{
    pa_simple * device;
    pa_sample_spec sample_spec;
} sound_t;

typedef enum
{
    STREAMCAST_ON,
    STREAMCAST_OFF
} streamcast_status_t;

typedef struct wavfile_header_s
{
    char    ChunkID[4];     /*  4   */
    int32_t ChunkSize;      /*  4   */
    char    Format[4];      /*  4   */

    char    Subchunk1ID[4]; /*  4   */
    int32_t Subchunk1Size;  /*  4   */
    int16_t AudioFormat;    /*  2   */
    int16_t NumChannels;    /*  2   */
    int32_t SampleRate;     /*  4   */
    int32_t ByteRate;       /*  4   */
    int16_t BlockAlign;     /*  2   */
    int16_t BitsPerSample;  /*  2   */

    char    Subchunk2ID[4];
    int32_t Subchunk2Size;
} wavfile_header_t;

#define CAPTURE_NAME "a_stream"

#define SUBCHUNK1SIZE   (16)
#define AUDIO_FORMAT    (1)
#define NUM_CHANNELS    (2)
#define SAMPLE_RATE     (44100)

#define BITS_PER_SAMPLE (16)

#define BYTE_RATE       (SAMPLE_RATE * NUM_CHANNELS * BITS_PER_SAMPLE/8)
#define BLOCK_ALIGN     (NUM_CHANNELS * BITS_PER_SAMPLE/8)

int write_wav_header(FILE*   file_p);

int create_capture_device(sound_t ** sound);
int read_data(sound_t * sound, void * data, uint32_t length);
int destroy_capture_sound_device(sound_t * sound);

int create_playback_device(sound_t ** sound);
int write_data(sound_t * sound, void * data, uint32_t length);
int destroy_playback_sound_device(sound_t * sound);

#endif /* SOUND_H */