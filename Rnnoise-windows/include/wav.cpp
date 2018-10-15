#include "wav.h"
#include "wav.h"
#include "wav.h"
#include "fstream"
#include <iostream>

int WAV::read(string file_name)
{
	ifstream fi(file_name,ios::binary);
	if (!fi.is_open()) std::cout << "Cannot open the file!\n";

	while (fi.good()) {

		// start to read the wav file.
		// read the wav file's header
		fi.seekg(0);
		fi.read( (char*)&chunk_ID, sizeof(chunk_ID[0]));
		fi.read( (char*)&chunk_ID, sizeof(chunk_ID[0]));
		fi.read( (char*)&format, sizeof(format[0]));
		fi.read( (char*)&fmt_chunk_marker, sizeof(fmt_chunk_marker[0]));
		fi.read( (char*)&length_of_fmt, sizeof(length_of_fmt));
		fi.read( (char*)&length_of_fmt, sizeof(length_of_fmt));
		fi.read( (char*)&format_type, sizeof(format_type));
		fi.read( (char*)&channels, sizeof(channels));
		fi.read( (char*)&sample_rate, sizeof(sample_rate));
		fi.read( (char*)&byte_rate, sizeof(byte_rate));
		fi.read( (char*)&block_align, sizeof(block_align));
		fi.read( (char*)&bits_per_sample, sizeof(bits_per_sample));
		fi.read( (char*)&data_chunk_header, sizeof(data_chunk_header[0]));
		fi.read( (char*)&data_size, sizeof(data_size));

		sample_numbers = data_size / bites_per_sample;
		// read the wav file's PCM data
		if (channels != 1) {
			// stereo
			bool channel_select = true;
			pcm_left = new char[data_size];
			pcm_right = new char[data_size];
			size_t countL = 0;
			size_t countR = 0;
			while (fi.good()) {
				if (channel_select) {
					fi.read(pcm_left+ countL* bits_per_sample, bits_per_sample);
					channel_select = false;
				}
				else {
					channel_select = true;
					fi.read(pcm_right + countR * bits_per_sample, bits_per_sample);
				}
			}
			decode_pcm(pcm_left, data_left);
			decode_pcm(pcm_right, data_right);

		}
		else {
			// mono
			pcm_left = new char[data_size];
			fi.read(pcm_left, data_size);
			decode_pcm(pcm_left, data_left);
		}
		
	}
	return 0;
}

int WAV::write(string file_name)
{
	ofstream fo(file_name);
	//fo.seekp(0);
	fo.write((char*)&chunk_ID, sizeof(chunk_ID[0]));
	fo.write((char*)&chunk_ID, sizeof(chunk_ID[0]));
	fo.write((char*)&format, sizeof(format[0]));
	fo.write((char*)&fmt_chunk_marker, sizeof(fmt_chunk_marker[0]));
	fo.write((char*)&length_of_fmt, sizeof(length_of_fmt));
	fo.write((char*)&length_of_fmt, sizeof(length_of_fmt));
	fo.write((char*)&format_type, sizeof(format_type));
	fo.write((char*)&channels, sizeof(channels));
	fo.write((char*)&sample_rate, sizeof(sample_rate));
	fo.write((char*)&byte_rate, sizeof(byte_rate));
	fo.write((char*)&block_align, sizeof(block_align));
	fo.write((char*)&bits_per_sample, sizeof(bits_per_sample));
	fo.write((char*)&data_chunk_header, sizeof(data_chunk_header[0]));

	fo.write((char*)&data_size, sizeof(data_size));
	fo.write((char*)&pcm_recode, sizeof(pcm_recode[0]));

	return 0;
}

int WAV::decode_pcm(char * pcm_data,float *data)
{
	data = new float[data_size / bits_per_sample];	// µãÊý
	short temp;
	for (size_t n = 0; n != data_size / bits_per_sample; ++n) {
	
		temp = (char *)(pcm_data + n * bits_per_sample);
		data[n] = (float)temp;
	}
	return 0;
}
