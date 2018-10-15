#pragma once
#include <string>
#include <fstream>
#include "fstream"
#include <iostream>
#include <vector>
struct HEADER
{
	char chunk_ID[4];                           // RIFF string
	std::uint32_t  chunk_size;                  // overall size of
	char format[4];                             // WAVE string
	char fmt_chunk_marker[4];                   // fmt string with trailing null char
	std::uint32_t length_of_fmt;                // length of the format data
	std::uint16_t  format_type;                 // format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	std::uint16_t  channels;                    // no.of channels
	std::uint32_t  sample_rate;                 // sampling rate (blocks per second)
	std::uint32_t  byte_rate;                   // SampleRate * NumChannels * BitsPerSample/8
	std::uint16_t  block_align;                 // NumChannels * BitsPerSample/8
	std::uint16_t  bits_per_sample;             // bits per sample, 8- 8bits, 16- 16 bits etc
	char data_chunk_header[4];					// DATA string or FLLR string
	std::uint32_t  data_size;                   // NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read

	std::uint32_t file_size;
	unsigned char *data;
};
class WAV {
public:
	/* WAV-header parameters				Memory address - Occupied space - Describes */
	char chunk_ID[4];                           // 0x00 4 byte - RIFF string
	std::uint32_t  chunk_size;                  // 0x04 4 byte - overall size of
	char format[4];                             // 0x08 4 byte - WAVE string
	char fmt_chunk_marker[4];                   // 0x0c 4 byte - fmt string with trailing null char
	std::uint32_t length_of_fmt;                // 0x10 4 byte - length of the format data,the next part
	std::uint16_t  format_type;                 // 0x14 2 byte - format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	std::uint16_t  channels;                    // 0x16 2 byte - nunbers of channels
	std::uint32_t  sample_rate;                 // 0x18 4 byte - sampling rate (blocks per second)
	std::uint32_t  byte_rate;                   // 0x1c 4 byte - SampleRate * NumChannels * BitsPerSample/8 [比特率]
	std::uint16_t  block_align;                 // 0x20 2 byte - NumChannels * BitsPerSample/8 [块对齐=通道数*每次采样得到的样本位数/8]
	std::uint16_t  bits_per_sample;             // 0x22 2 byte - bits per sample, 8- 8bits, 16- 16 bits etc [位宽]
	char data_chunk_header[4];					// 0x24 4 byte - DATA string or FLLR string
	std::uint32_t  data_size;                   // 0x28 4 byte - NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk 
												//				that will be read,that is the size of PCM data.

	std::uint16_t  sample_numbers;
	//unsigned char *data;						// a pointer 
	unsigned char *pcm_left;						// a pointer 
	unsigned char *pcm_right;						// a pointer 
	float *data_left;
	float *data_right;

	int read(string file_name);
	int write(string file_name);
	int decode_pcm(char * pcm_data, float *data);
	int recode_pcm(float*data,char *pcm_recode);
	uint16_t getChannels();
	uint16_t getSampleRate();
	uint16_t getByteRate();
};