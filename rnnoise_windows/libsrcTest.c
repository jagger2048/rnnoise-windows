#include <stdint.h>
#include <stdio.h>
#include "unittest_helper.h"
#include "libresample.h"

#define BUFFER_SIZE 2048
float in_buffer[BUFFER_SIZE];
float ref_buffer[BUFFER_SIZE];
float out_buffer[BUFFER_SIZE];

int wavTest(const char* inWav, const char* outWav, int16_t targetSr) {
    drwav* pWavin = open_wavfile(inWav);
    if (pWavin == NULL) {
        printf("Cannot open wav file\n");
        return -1;
    }

    uint16_t channels = pWavin->channels;
    uint16_t currentSr = pWavin->sampleRate;
    uint64_t totalPCMFrameCnt = pWavin->totalPCMFrameCount;

    printf("open file write\n");
    printf("outfile: %s\n", outWav);

    drwav* pWavOut = init_wavfile(outWav, 1, targetSr, 16);
    if (!pWavOut) {
        printf("Cannot open output file\n");
        drwav_uninit(pWavOut);
        return -1;
    }

    printf("test  target: %d, current: %d\n", targetSr, currentSr);
    double factor = (targetSr*1.0f) / currentSr;
    printf("factor is %lf\n", factor);
    void* handle = resample_open(1, factor, factor);
    printf("open pass\n");
    int16_t fwidth = resample_get_filter_width(handle);
    int sampleProc = 0;

    printf("Test start\n");
    // in 16k out 48k
    for (size_t n = 0; n < totalPCMFrameCnt / 256; n++) {
        get_frame_f32(pWavin, in_buffer, 256);

        resample_process(handle, factor,
                           in_buffer, 256,
                           0, &sampleProc, // lastFlag 最後一個
                           out_buffer, 256 * factor);

        write_frames_f32(pWavOut, 256 * factor, out_buffer);
    }
    printf("Test end\n");
    printf("total cnt %ld  %ld %d\n", pWavOut->totalPCMFrameCount, pWavOut->dataChunkDataSize, pWavOut->sampleRate);

    uninit_wavfile(pWavin);
    uninit_wavfile(pWavOut);

    return 0;
}

int main(int argc, char** argv) {
    char micFile[256];
    char refFile[256];
    char outFile[256];
    printf("This is a src demo\n");

    if (argc == 1) {
        printf("Usage: %s <inFile> <outFile> <target samplerate> \n", argv[0]);
        return 0;
    }

    sprintf(micFile, "%s", argv[1]);
    sprintf(outFile, "%s", argv[2]);
    int targetSr = atoi(argv[3]);
    printf("target samplerate is %d\n", targetSr);
    wavTest(micFile, outFile, targetSr);

    printf("demo end...\n");

    return 0;
}