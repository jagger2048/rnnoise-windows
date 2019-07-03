#include <stdio.h>
#include <stdint.h>

#include "rnnoise.h"
#include "denoise.h"

#include "wavfile.h"
#include <fstream>
#include <iostream>
int rnnoiseDemo(char* inFile,char* outFile) {
    /*
    *	A simple demo for Rnnoise
    */

	if (inFile == NULL || outFile == NULL)
	{
		fprintf(stderr, "In/out file error\n");
		return -1;
	}
    // Step 1 : read wav file(pcm data)
    WAV* wavfile = NULL;
    wavfile = wavfile_read(inFile);
	if (wavfile == NULL)
	{
		fprintf(stderr, "Output in file failed\n");
		return -1;
	}
    size_t totalSamples = wavfile->totalPCMFrameCount;							// samples per channel

    int16_t* outputS16 = (int16_t*)malloc(sizeof(int16_t) * totalSamples);

    int16_t* inputS16 = (int16_t*)malloc(sizeof(int16_t) * totalSamples);
    memcpy(inputS16, wavfile->pDataS16[0], sizeof(int16_t) * totalSamples);	//|inputS16| stores the pcm data(S16 format)

    printf("Frame size is %d , samplerate is %d\n", FRAME_SIZE, wavfile->sampleRate);

    wavfile_destory(wavfile);

    // Step 2 : initialize the rnnoise module
    DenoiseState* st = rnnoise_create();

    // Step 3 : process the pcm frame data with rnnoise
    float flag = 0;						// 0 ~ 1 ||  have not noise~ have noise
    float processFrame[FRAME_SIZE] = {};
    for (size_t n = 0; n < totalSamples / FRAME_SIZE * FRAME_SIZE; n += FRAME_SIZE)
    {

        for (size_t i = 0; i < FRAME_SIZE; i++) processFrame[i] = inputS16[n + i];

        //  Apply the PCM data to the rnnoise and denoise the audio data
        flag = rnnoise_process_frame(st, processFrame, processFrame);
        /*
            Note:	Although the input and output format of the |rnnoise_process_frame() |function is |float|,
                    we should treat them as |S16|,short format PCM instead of |F32|,float format PCM.
                    If you feed the process function with |F32| PCM data,the function will result in a 0 return
                    and do not denoise the data. For more details,you can test yourself.
        */

        for (size_t i = 0; i < FRAME_SIZE; i++) outputS16[n + i] = processFrame[i];

        //printf("%f \n", flag);
    }
    // Step 4 : Export to wav file for check.
    wavfile_write_s16(outFile, &outputS16, totalSamples, 1, wavfile->sampleRate);

    free(inputS16);
    free(outputS16);
    rnnoise_destroy(st);
}

void rnnoiseOfficialDemo() {

    //  A  modified official demo for reference
	// Only support 1 channel, 16 bits, 48k samplerate wav file
	FILE* ifp = NULL;
	FILE* ofp = NULL;
    float x[FRAME_SIZE];
    DenoiseState* st;
    st = rnnoise_create();
    ifp = fopen("speech with noise 48k.wav", "rb+");	// input wav file
	ofp = fopen("out 48k.wav", "wb+");				// output wav file

    char header[44];									// the pcm data start from the 44th byte of wav file
    fread((char*)& header, sizeof(char) * 44,1,ifp);	// read the header data of the  .wav file
    fwrite(header, 44,1,ofp);							// write the header for the  .wav file
    std::cout << "ready to process\n";
	short tmp[FRAME_SIZE];
    while ( fread((char*)& tmp, sizeof(short) * FRAME_SIZE, 1, ifp) == 1) {

        for (size_t i = 0; i < FRAME_SIZE; i++) x[i] = tmp[i];
        rnnoise_process_frame(st, x, x);
        for (size_t i = 0; i < FRAME_SIZE; i++) tmp[i] = x[i];
        fwrite((char*)& tmp, sizeof(short) * FRAME_SIZE,1,ofp);
    }

    rnnoise_destroy(st);
	printf("process end...\n");
}


int main(int argc, char **argv) {

	char inFile[256];
	char outFile[256];

#ifdef _WIN32
	// Windows platform
	sprintf(inFile, "./resources/audio_in_with_noise_48k.wav");
	sprintf(outFile, "./resources/audio_ns.wav");
	rnnoiseDemo(inFile, outFile);
	return 0;
#endif // _WIN32

	// Linux platform
	if (argc < 2 || argc >3)
	{
		printf("Usage: ./%s inFile.wav outFile.wav\n", argv[0]);
		return -1;
	}
	sprintf(inFile, argv[1]);
	sprintf(outFile, argv[2]);
	rnnoiseDemo(inFile, outFile);

    return 0;
}