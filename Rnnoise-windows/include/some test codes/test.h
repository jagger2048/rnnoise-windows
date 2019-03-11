#pragma once

ifstream fi;
ofstream fo;
float x[FRAME_SIZE];
DenoiseState *st;
st = rnnoise_create();
fi.open("speech with noise 32k.wav", ios::binary);
fo.open("out 32k.wav", ios::binary);

char header[44];									// the pcm data start from the 44th byte
fi.read((char*)&header, sizeof(char) * 44);		// read the header data of the input .wav file
fo.write(header, 44);								// write the header for the output .wav file
cout << "ready to process\n";
while (fi.good()) {
	short tmp[FRAME_SIZE];
	fi.read((char*)&tmp, sizeof(short) * FRAME_SIZE);
	for (size_t i = 0; i<FRAME_SIZE; i++) x[i] = tmp[i];
	rnnoise_process_frame(st, x, x);
	for (size_t i = 0; i<FRAME_SIZE; i++) tmp[i] = x[i];
	fo.write((char*)&tmp, sizeof(short)*FRAME_SIZE);
}

rnnoise_destroy(st);
fi.close();
fo.close();
cout << "\n ok\n";
