#include <stdio.h>
#include <stdint.h>

#include "rnnoise.h"
#include "denoise.h"

#include "wavfile.h"
#include <fstream>
#include <iostream>
//using namespace std;
void rnnoiseDemo() {
	/*
	*	A simple demo for Rnnoise
	*/
	// Step 1 : read wav file(pcm data)
	WAV* wavfile;
	//wavfile = wavfile_read("audio_in_with_noise.wav");
	wavfile = wavfile_read("./resources/audio_in_with_noise 48k.wav");
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
	wavfile_write_s16("after rnnoise processed 32k.wav", &outputS16, totalSamples, 1, wavfile->sampleRate);

	free(inputS16);
	free(outputS16);
	rnnoise_destroy(st);

}

void rnnoiseOfficialDemo() {

	//  A  modified official demo for reference

	std::ifstream fi;
	std::ofstream fo;
	float x[FRAME_SIZE];
	DenoiseState* st;
	st = rnnoise_create();
	fi.open("speech with noise 32k.wav", std::ios::binary);	// input wav file
	fo.open("out 32k.wav", std::ios::binary);				// output wav file

	char header[44];									// the pcm data start from the 44th byte of wav file
	fi.read((char*)& header, sizeof(char) * 44);			// read the header data of the  .wav file
	fo.write(header, 44);								// write the header for the  .wav file
	std::cout << "ready to process\n";
	while (fi.good()) {
		short tmp[FRAME_SIZE];
		fi.read((char*)& tmp, sizeof(short) * FRAME_SIZE);
		for (size_t i = 0; i < FRAME_SIZE; i++) x[i] = tmp[i];
		rnnoise_process_frame(st, x, x);
		for (size_t i = 0; i < FRAME_SIZE; i++) tmp[i] = x[i];
		fo.write((char*)& tmp, sizeof(short) * FRAME_SIZE);
	}

	rnnoise_destroy(st);
	fi.close();
	fo.close();
	std::cout << "\n ok\n";
}


int main(int argc, char **argv) {

	rnnoiseDemo();
    return 0;
}