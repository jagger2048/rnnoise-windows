// Rnnoise-windows.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include "fstream"
#include <iostream>
//
#include "wav.h"
#include <string>
#include <stdio.h>

#include "rnnoise.h"

#include <malloc.h>
#include "denoise.c"

//#include "_kiss_fft_guts.h"
#include "kiss_fft.c"
#include "pitch.c"

#include "rnn.c"
//#include "rnn.h"
#include "rnn_data.c"
#include "AudioFile.cpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // !M_PI

#ifndef FRAME_SIZE
#define FRAME_SIZE 480
#endif // !FRAME_SIZE


using namespace std;
int main(int argc, char **argv) {
	// -task 1: read the wav file's PCM data
	// -task 2: apply the PCM data to the rnnoise and denoise the audio data
	// -task 3: output the wav file 

	string directory = ".\\assets\\";
	ifstream fi;
	ofstream fo;
	float x[FRAME_SIZE];
	DenoiseState *st;
	st = rnnoise_create();
	fi.open(directory + "audio_in_with_noise.wav" , ios::binary);
	fo.open(directory + "denoise_3.wav" , ios::binary);

	char header[44];									// the pcm data start from the 44th byte
	fi.read( (char*)&header , sizeof(char) * 44);		// read the header data of the input .wav file
	fo.write(header, 44);								// write the header for the output .wav file
	vector<char> tt;
	cout << "ready to process\n";
	while (fi.good()) {
		short tmp[FRAME_SIZE];
		fi.read( (char*)&tmp , sizeof(short) * FRAME_SIZE);
		for (size_t i = 0; i<FRAME_SIZE; i++) x[i] = tmp[i];
		rnnoise_process_frame(st, x, x);
		for (size_t i = 0; i<FRAME_SIZE; i++) tmp[i] = x[i];
		fo.write( (char*)&tmp , sizeof(short)*FRAME_SIZE);
	}

	rnnoise_destroy(st);
	fi.close();
	fo.close();
	cout << "\n ok\n";

	return 0;
}
