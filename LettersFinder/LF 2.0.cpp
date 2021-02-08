#pragma comment(linker, "/STACK:16777216")
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream> 
#include <cstring>
#include <string>
#include <cstdio>
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>

using namespace std;

//maximal constants
const int HEIGHT = 1000;
const int WIDTH = 2000;

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

int height, width, area;
int image[HEIGHT][WIDTH][3], pixel[3];
int numstr, numchar;
vector<string> code;
string current;

int main() {

	freopen("output.txt", "w", stdout);
	
	FILE *streamIn;
	streamIn = fopen("./sample2_mono_small.bmp", "r");

	if (streamIn == (FILE *)0){
		printf("File opening error ocurred. Program exited.\n");
		exit(0);
	}

	for (int i = 0; i < 18; i++) 
		getc(streamIn); //strip out BMP header

	//count size of image
	width = GetBitNumber(streamIn);
	height = GetBitNumber(streamIn);
	area = height * width;

	for (int i = 0; i < 18 * 3 - 18 - 8; i++) 
		getc(streamIn); //skip last bits in header...

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
	
	//convert picture into binary string
	for (int i = 0; i < height; i++) {
		current = "";
		for (int j = 0; j < width; j++) {
			char symbol;
			symbol = (Black(image[i][j][0], image[i][j][1], image[i][j][2]) ? '1' : ' ');
			current += symbol;
		}
		code.push_back(current);
	}

	//output binary strings...
	for (int i = 0; i < code.size(); i++)
		cout << code[i] << '\n';
	
	return 0;
}