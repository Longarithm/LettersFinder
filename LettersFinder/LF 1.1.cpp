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

const int HEIGHT = 5;//823;
const int WIDTH = 5;//1438;
const int SIZE = HEIGHT * WIDTH;

int image[HEIGHT][WIDTH + 1][3], sum[HEIGHT][WIDTH + 1], pixel[3];
int numstr, numchar, idstr;
vector<string> code;
string current;

int main() {

	freopen("output.txt", "w", stdout);
	
	FILE *streamIn;
	//streamIn = fopen("./sample2_mono_small.bmp", "r");
	streamIn = fopen("./pixels.bmp", "r");
	if (streamIn == (FILE *)0){
		printf("File opening error ocurred. Program exited.\n");
		exit(0);
	}

	int byte;
	int count = 0;
	for (int i = 0; i < 18 * 3; i++) 
		byte = getc(streamIn); //strip out BMP header

	numstr = HEIGHT - 1;
	idstr = 0;
	numchar = 0;

	for (int i = 0; i < SIZE; i++) { //for each pixel
		for (int j = 2; j >= 0; j--) {
			pixel[j] = getc(streamIn); 
			image[numstr][numchar][j] = pixel[j]; 
		}
		numchar++;

		if (idstr == 0 && numchar == WIDTH || idstr == 1 && numchar == WIDTH + 1 || idstr == 2 && numchar == WIDTH + 1) {
			idstr = (idstr + 1) % 3;
			numstr--;
			numchar = 0;
		}

		printf("pixel %d : [%d,%d,%d]\n", i+1, pixel[0], pixel[1], pixel[2]);
	}

	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			sum[i][j] = image[i][j][0] + image[i][j][1] + image[i][j][2]; 
		}
	}

	for (int i = 0; i < HEIGHT; i++) {
		current = "";
		for (int j = 0; j < WIDTH; j++) {
			int cursum = sum[i][j]; //* 4;
			//cursum += sum[i - 1][j] + sum[i][j - 1] + sum[i + 1][j] + sum[i][j + 1];
			char c;
			c = (cursum < 255 ? '1' : ' ');
			current += c;
		}
		code.push_back(current);
	}

	for (int i = 0; i < code.size(); i++)
		cout << code[i] << '\n';

	return 0;
}