#include "unittest_helper.h"

// This is a wavfile read/write demo
int main(int argc, char* argv[]){
    if( argc == 1){
        printf("Usage: %s <inWavfile> <outWavfile>\n", argv[0]);
        return 0;
    }

    drwav* pWavIn = open_wavfile(argv[1]);
    if(!pWavIn){
        printf("Cannot open wav file\n");
        return -1;
    }

    printf("%s wav file messages:\nsample rate:\t%d\nchannels:\t%d\ntotalPCMFrameCount:\t%ld\nBitsPerSample:\t%d\n",
        argv[1],
        pWavIn->sampleRate,
        pWavIn->channels,
        pWavIn->totalPCMFrameCount,
        pWavIn->bitsPerSample
        );

    drwav* pWavOut = init_wavfile(argv[2], pWavIn->channels, pWavIn->sampleRate, pWavIn->bitsPerSample);
    if(!pWavOut){
        printf("Cannot create wav file\n");
        uninit_wavfile(pWavIn);
        return -1;
    }

    printf("Test start...\n");
    // read wav frame and write to output wav file
    int16_t frameIn[480];
    int16_t frameOut[480];
    for (size_t n = 0; n < pWavIn->totalPCMFrameCount/480; n++)
    {
        get_frame_s16(pWavIn, frameIn, 480);
        // your processing here
        for (size_t i = 0; i < 480; i++) {
            frameOut[i] = frameIn[i];
        }

        write_frames_s16(pWavOut, 480, frameOut);
    }

    printf("Test end...\n");
    uninit_wavfile(pWavIn);
    uninit_wavfile(pWavOut);
    return 0;
}