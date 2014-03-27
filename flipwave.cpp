#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <algorithm>

typedef float   stereodata[2];
using namespace std;

int parseHeaders(ifstream& inFile, ofstream& outFile) {
	//Parse through RIFF header
	char chunkID[4];
	unsigned int chunkSize;
	char format[4];

	inFile.read(		chunkID, 		4);
	inFile.read((char*)	&chunkSize, 	4);
	inFile.read(		format, 		4);

	chunkID[4] = '\0';
	format[4] = '\0';

	assert(strcmp(chunkID, "RIFF") == 0 );
	assert(strcmp(format, "WAVE") == 0 );

	//Parse though fmt chunk
	char subChunk1ID[4];
	unsigned int subChunk1Size;
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned int sampleRate;
	unsigned int byteRate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;

	//read in headers
	inFile.read(		subChunk1ID, 	4);
	inFile.read((char*)	&subChunk1Size, 4);
	inFile.read((char*)	&audioFormat, 	2);
	inFile.read((char*)	&numChannels, 	2);
	inFile.read((char*)	&sampleRate, 	4);
	inFile.read((char*)	&byteRate, 		4);
	inFile.read((char*)	&blockAlign, 	2);
	inFile.read((char*)	&bitsPerSample, 2);

	subChunk1ID[4] = '\0';

	assert(strcmp(subChunk1ID,"fmt ") == 0 );
	assert(subChunk1Size == 16);
	assert(audioFormat == 1);
	assert(numChannels == 2);
	//assert(sampleRate == 44100);
	assert(bitsPerSample == 16);
	assert(audioFormat == 1);

	//Parse through data chunk
	char subChunk2ID[4];
	unsigned int subChunk2Size;

	inFile.read(		subChunk2ID, 	4);
	inFile.read((char*)	&subChunk2Size, 4);

	subChunk2ID[4] = '\0';
	assert(strcmp(subChunk2ID, "data") == 0 );

	//write to output file
	outFile.write(			chunkID, 		4);
	outFile.write((char*)	&chunkSize, 	4);
	outFile.write(			format, 		4);

	outFile.write(			subChunk1ID, 	4);
	outFile.write((char*)	&subChunk1Size, 4);
	outFile.write((char*)	&audioFormat, 	2);
	outFile.write((char*)	&numChannels, 	2);
	outFile.write((char*)	&sampleRate, 	4);
	outFile.write((char*)	&byteRate, 		4);
	outFile.write((char*)	&blockAlign, 	2);
	outFile.write((char*)	&bitsPerSample, 2);

	outFile.write(			subChunk2ID, 	4);
	outFile.write((char*)	&subChunk2Size, 4);

	return subChunk2Size;
}

void writeSamp(stereodata& samp, ofstream& outFile) {

	signed short int l, r;
	l = (samp[0]*32767);
	r = (samp[1]*32767);

	outFile.write((char*)	&l, 2);
	outFile.write((char*)	&r, 2);
}

void readSamp(stereodata& samp, ifstream& inFile) {
	signed short int l, r;

	inFile.read((char*)	&l, 2);
	inFile.read((char*) &r, 2);

	samp[0] = (float)l/32767;
	samp[1] = (float)r/32767;
}

void parseData(ifstream& inFile, ofstream& outFile, int numBytes) {

	int flip = 1;
	stereodata inp;
	while (numBytes -= 4 >= 0) {

		readSamp( inp, inFile);

		inp[0] *= flip;
		inp[1] *= flip;

		writeSamp( inp, outFile);

		flip *= -1;
	}

}

int main(int argc, char* argv[]) {

	if (argc != 3) {
		cout << "Proper usage: flipwave <filename> <output>" << endl;
		exit(-1);
	}

	cout << "Reading in file..." << endl;
	ifstream input(argv[1], ios::binary);
	ofstream output(argv[2], ios::binary);

	//Parse the headers
	int numBytes = parseHeaders(input, output);
	parseData(input, output, numBytes);
	cout << "Done." << endl;

	return 0;
}