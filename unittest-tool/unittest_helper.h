/*
    This is a unittest tools set contains these models:
    1. [-] wavfile read/write
    2. [-] time consume calculate for profile
    3. [] sample rate converter
*/

#include <stdio.h>
#include <string.h>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
//  =================================================== //
//              wav file process tools                  //
//  =================================================== //

// open wav file
drwav* open_wavfile(const char* filename){

    drwav* pWav =(drwav*)malloc(sizeof(drwav));

    if (pWav == NULL || !drwav_init_file(pWav, filename)) {
        printf("Error in opening wav file...\n");
        return NULL;
    }

    return pWav;
}

// read s16 format wav data
size_t get_frame_s16(drwav* pWav, int16_t* pFrameData, size_t frameSize){

    if (drwav_read_pcm_frames_s16(pWav, (size_t)frameSize, pFrameData) != frameSize) {
        return 0;
    }

    return frameSize;
}

// read f32 format wav data
size_t get_frame_f32(drwav* pWav, float* pFrameData, size_t frameSize){

    if (drwav_read_pcm_frames_f32(pWav, (size_t)frameSize, pFrameData) != frameSize) {
        return 0;
    }

    return frameSize;
}

// reset wav stream
int reset_stream(drwav* pWav){

    return drwav_seek_to_pcm_frame(pWav,0); // or return drwav_seek_to_first_pcm_frame(pWav);
}

// initialize wavfile write
drwav* init_wavfile(const char* filename, uint16_t channels, uint16_t sampleRate, uint16_t bitsPerSample){
    drwav_data_format format;
    format.container = drwav_container_riff;     // <-- drwav_container_riff = normal WAV files, drwav_container_w64 = Sony Wave64.
    format.format = DR_WAVE_FORMAT_PCM;          // <-- Any of the DR_WAVE_FORMAT_* codes.
    format.channels = channels;
    format.sampleRate = sampleRate;
    format.bitsPerSample = bitsPerSample;
    drwav* pWav = drwav_open_file_write(filename, &format);
    if (pWav == NULL) {
        printf("error in init write file...\n");
        return NULL;
    }

    return pWav;
}

// uninitialize wavfile
int uninit_wavfile(drwav* pWav){
    return drwav_uninit(pWav);
}

// write s16 format frame data into stream
uint64_t write_frames_s16(drwav* pWav,uint64_t frameCount,int16_t* pSamples){
    size_t framesWritten;
    framesWritten = drwav_write_pcm_frames(pWav, frameCount, pSamples);
    pWav->dataChunkDataSize += framesWritten * pWav->bitsPerSample * pWav->channels / 8; // bytes written
    return framesWritten;
}

// write f32 format frame data into stream
uint64_t write_frames_f32(drwav* pWav, uint64_t frameCount, float* pSamples){
    size_t framesWritten = 0;
    int16_t PCMBuffer[32];   // support up to 32 channels per pcm frame
    for (size_t i = 0; i < frameCount; ++i) {
        drwav_f32_to_s16(PCMBuffer, pSamples + i * pWav->channels, pWav->channels);
        framesWritten += drwav_write_pcm_frames(pWav, 1, PCMBuffer);
        pWav->dataChunkDataSize += pWav->bitsPerSample * pWav->channels / 8; // bytes written
    }

    return framesWritten;
}


//  =================================================== //
//                  profile tools                       //
//  =================================================== //

#if   defined(__APPLE__)
# include <mach/mach_time.h>
#elif defined(_WIN32)
# define WIN32_LEAN_AND_MEAN

# include <windows.h>

#else // __linux

# include <time.h>

# ifndef  CLOCK_MONOTONIC //_RAW
#  define CLOCK_MONOTONIC CLOCK_REALTIME
# endif
#endif

static
uint64_t nanotimer() {
    static int ever = 0;
#if defined(__APPLE__)
    static mach_timebase_info_data_t frequency;
    if (!ever) {
        if (mach_timebase_info(&frequency) != KERN_SUCCESS) {
            return 0;
        }
        ever = 1;
    }
    return  (mach_absolute_time() * frequency.numer / frequency.denom);
#elif defined(_WIN32)
    static LARGE_INTEGER frequency;
    if (!ever) {
        QueryPerformanceFrequency(&frequency);
        ever = 1;
    }
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return (t.QuadPart * (uint64_t) 1e9) / frequency.QuadPart;
#else // __linux
    struct timespec t = {0};
    if (!ever) {
        if (clock_gettime(CLOCK_MONOTONIC, &t) != 0) {
            return 0;
        }
        ever = 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * (uint64_t) 1e9) + t.tv_nsec;
#endif
}

// return now time in us
static double now() {
    static uint64_t epoch = 0;
    if (!epoch) {
        epoch = nanotimer();
    }
    return (nanotimer() - epoch) / 1e9;
};

// return time elapsed in us, time_interval * 1000 = ms
static double calcElapsed(double start, double end) {
    double took = -start;
    return took + end;
}

//  =================================================== //
//           sampleRate convert tools                  //
//  =================================================== //

// TODO: add sample rate convert tools