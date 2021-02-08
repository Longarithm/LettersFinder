#define _CRT_SECURE_NO_WARNINGS //for visual studio 2013

#include <iostream>
#include <fstream> 
#include <cstring>
#include <string>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

//maximal constants, can be bigger
const int HEIGHT = 1000;
const int WIDTH = 2000;

struct hash_function {
	long long pow[2], p[2][WIDTH]; //max(HEIGHT, WIDTH)
	long long hash[HEIGHT][WIDTH];

	hash_function() {};

	hash_function(long long pow1, long long pow2) {
		pow[0] = pow1;
		pow[1] = pow2;
		for (int i = 0; i < 2; i++) {
			p[i][0] = 1;
			for (int j = 1; j < WIDTH; j++) {
				p[i][j] = (p[i][j - 1] * pow[i]);
			}
		}
	}
};

void SkipBits(FILE *streamIn, int n) {
	for (int i = 0; i < n; i++) {
		getc(streamIn);
	}
}

inline bool Black(int r, int g, int b) {
	return r == 0 && g == 0 && b == 0;
}

inline int GetBitNumber(FILE *streamIn) {
	int result = 0, byte;
	for (int i = 0; i < 4; i++) {
		byte = getc(streamIn);
		result += byte << (i * 8);
	}
	return result;
}

FILE *streamIn;
int height, width, area;
int image[HEIGHT][WIDTH][3], pixel[3];
int code[HEIGHT][WIDTH];
int numstr, numchar;
vector<string> out;
string current;
hash_function h[2];

void InitializeInput() {
	streamIn = fopen("./sample2_mono_small.bmp", "r");

	if (streamIn == (FILE *)0){
		printf("File opening error occurred. Program exited.\n");
		exit(0);
	}
}

void ScanPicture() {
	SkipBits(streamIn, 18); //strip out BMP header

	//count size of image
	width = GetBitNumber(streamIn);
	height = GetBitNumber(streamIn);
	area = height * width;

	SkipBits(streamIn, 18 * 3 - 18 - 8); //skip last bits in header...

	numstr = height - 1;
	numchar = 0;

	for (int i = 0; i < area; i++) { //scan each pixel
		for (int j = 2; j >= 0; j--) {
			pixel[j] = getc(streamIn); 
			image[numstr][numchar][j] = pixel[j]; 
		}

		numchar++;
		if (numchar == width) {
			numstr--;
			numchar = 0;
		}

		if ((i + 1) % width == 0) { //skip "extra" bits!
			getc(streamIn);
			getc(streamIn);
		}

		//printf("pixel %d: [%d,%d,%d]\n", i + 1, pixel[0], pixel[1], pixel[2]);
	}
}

void ConvertToCode() {
	//convert picture into binary code
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			code[i][j] = Black(image[i][j][0], image[i][j][1], image[i][j][2]);
		}
	}
}

void ConvertFromCode() {
	//convert bytecode into string
	for (int i = 0; i < height; i++) {
		current = "";
		for (int j = 0; j < width; j++) {
			char symbol;
			
			switch(code[i][j]) {
				case 0: 
					symbol = ' ';
					break;
				case 1: 
					symbol = '1';
					break;
				default: 
					symbol = 'x';
			}
			
			current += symbol;
		}
		out.push_back(current);
	}
}

void OutputAnswer() {
	freopen("output.txt", "w", stdout);

	//output all strings
	for (int i = 0; i < out.size(); i++)
		cout << out[i] << '\n';
}

int main() {

	InitializeInput();
	ScanPicture();
	ConvertToCode();
	
	//get hashes
	h[0] = hash_function(257, 1999);
	h[1] = hash_function(1000000007, 123479);
	for (int i = 0; i < 2; i++) {

	}

	ConvertFromCode();
	OutputAnswer();
	
	return 0;
}