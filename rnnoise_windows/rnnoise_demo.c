#include <stdio.h>
#include "rnnoise.h"
#include <stdint.h>
#define FRAME_SIZE 480

#include "unittest_helper.h"

// The wrapped of rnnoise's |rnnoise_process_frame| function so as to make sure its input/outpu is |f32| format.
// Note tha the frame size is fixed 480.
float rnnoise_process(DenoiseState* pRnnoise, float* pFrameOut, float* pFrameIn){
    unsigned int n;
    float vadProb;
    float buffer[FRAME_SIZE];

    // Note: Be careful for the format of the input data.
    for (n = 0; n < FRAME_SIZE; ++n){
        buffer[n] = pFrameIn[n] * 32768.0f;
    }

    vadProb = rnnoise_process_frame(pRnnoise, buffer, buffer);
    for (n = 0; n < FRAME_SIZE; ++n){
        pFrameOut[n] = drwav_clamp(buffer[n], -32768, 32767) * (1.0f / 32768.0f);
    }
    return vadProb;
}

int rnnoise_free(DenoiseState* pRnnoise){
    if (!pRnnoise){
        return -1;
    }

    rnnoise_destroy(pRnnoise);
    return 0;
}

int rnnoise_demo(const char* inFile, const char* outFile, uint16_t loopNum, uint16_t isOutput) {
    // Step 0: read wav data
    printf("open file read...\n");
    drwav* pWavIn = open_wavfile(inFile);
    if (pWavIn == NULL) {
        printf("Cannot open wav file\n");
        return -1;
    }

    uint16_t channels = pWavIn->channels;
    uint16_t sampleRate = pWavIn->sampleRate;
    uint64_t totalFrameCnt = pWavIn->totalPCMFrameCount;
    if (channels != 1) {
        printf("Only support mono wav file\n");
        uninit_wavfile(pWavIn);
        return -1;
    }

    printf("open file write\n");
    drwav* pWavOut = init_wavfile(outFile, 1, 48000, 16);
    if (!pWavOut) {
        printf("Cannot open output file\n");
        drwav_uninit(pWavIn);
        return -1;
    }

    // Step 1: create rnnoise block
    printf("Initialize rnnoise\n");
    DenoiseState* pRnnoise = rnnoise_create(NULL);
    if (!pRnnoise) {
        printf("initialized rnnoise error\n");
        return -1;
    }

    // Step 2: set loop times and output mode(optional)
    uint16_t nProcessCnt = loopNum;
    if (nProcessCnt <= 0) {
        nProcessCnt = 1;
    }

    if ( nProcessCnt > 20) {
        nProcessCnt = 20;
        printf("Only support 20 times\n");
    }

    printf("Loop for %d times for profile...\n", nProcessCnt);
    switch (isOutput)
    {
    case 0:
        printf("Output mode is OFF\n");
        break;
    case 1:
        printf("Output mode is ON\n");
        break;
    default:
        printf("invail mode, set output mode to OFF\n");
        isOutput = 0;
        break;
    }

    // Step 3: rnnoise frame process and output
    float frameIn[480];
    float frameOut[480];
    printf("Run...\n");
    double startTime = now();
    for (size_t j = 0; j < nProcessCnt; j++)
    {
        for (size_t n = 0; n < totalFrameCnt / 480; n++) {
            get_frame_f32(pWavIn, frameIn, 480);
            rnnoise_process(pRnnoise, frameOut, frameIn);
            if (isOutput) {
                write_frames_f32(pWavOut, 480, frameOut);
            }
        }

        reset_stream(pWavIn);
    }

    double timeInterval = calcElapsed(startTime, now()) * 1000;
    printf("Run end,time interval is\t%lf ms, each frame cost %lf ms / 10ms\n", timeInterval, timeInterval / (totalFrameCnt / 480 * nProcessCnt));

    // Step 4: uninit wavfile and rnnoise object
    uninit_wavfile(pWavIn);
    uninit_wavfile(pWavOut);
    rnnoise_destroy(pRnnoise);
    return 0;
}

int main(int argc, char **argv)
{
    char inFile[256];
    char outFile[256];
    uint16_t loopTimes = 1;
    uint16_t isOutput = 1;

#ifdef _WIN32
    // Windows platform
    sprintf(inFile, "../resources/audio_in_with_noise_48k.wav");
    sprintf(outFile, "../resources/audio_in_with_noise_ns.wav");
    rnnoise_demo(inFile, outFile, loopTimes, isOutput);
    return 0;
#endif // _WIN32

    // Linux platform
    if (argc == 1) {
        printf("Usage: %s <inFile> <outFile> <loopTimes, opt.> <isOutput, opt.>\n", argv[0]);
        return  0;
    }

    sprintf(inFile, argv[1]);
    sprintf(outFile, argv[2]);
    if (argc > 4) {
        loopTimes = atoi(argv[3]);
    }

    if (argc > 5) {
        isOutput = atoi(argv[4]);
    }

    rnnoise_demo(inFile, outFile, loopTimes, isOutput);
    return 0;
}


// A  modified official demo for reference, only support for 16-bit PCM file
int rnnoise_demo_official(const char* inFile, const char* outFile) {
    int i;
    int first = 1;
    float x[FRAME_SIZE];
    FILE* fin, * fout;
    DenoiseState* st = rnnoise_create(NULL);

    fin = fopen(inFile, "rb+");
    fout = fopen(outFile, "rb+");
    if (fin == NULL || fout == NULL)
    {
        fprintf(stderr, "Open input/ouptut file stream fail...\n");
        return 1;
    }

    while (1)
    {
        short tmp[FRAME_SIZE];
        fread(tmp, sizeof(short), FRAME_SIZE, fin);
        if (feof(fin))
            break;
        for (i = 0; i < FRAME_SIZE; i++)
            x[i] = tmp[i];
        rnnoise_process_frame(st, x, x);
        for (i = 0; i < FRAME_SIZE; i++)
            tmp[i] = x[i];
        if (!first)
            fwrite(tmp, sizeof(short), FRAME_SIZE, fout);
        first = 0;
    }

    rnnoise_destroy(st);
    fclose(fin);
    fclose(fout);
    return 0;
}