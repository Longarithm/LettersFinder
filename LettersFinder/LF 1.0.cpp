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

const int INF = 1E9;
const int SIZE = 1438 * 823;

int image[SIZE][3], numstr, numchar; // first number here is 1024 pixels in my image, 3 is for RGB values
vector<string> code;
string current;

int main() {

	freopen("output.txt", "w", stdout);
	
	FILE *streamIn;
	streamIn = fopen("./sample2_mono_small.bmp", "r");
	if (streamIn == (FILE *)0){
		printf("File opening error ocurred. Exiting program.\n");
		exit(0);
	}

	int byte;
	int count = 0;
	for(int i = 0; i < 18 * 3; i++) byte = getc(streamIn);  // strip out BMP header

	current = "";
	numstr = numchar = 0;

	for(int i = 0; i < SIZE; i++){    // foreach pixel
		image[i][2] = getc(streamIn);  // use BMP 24bit with no alpha channel
		image[i][1] = getc(streamIn);  // BMP uses BGR but we want RGB, grab byte-by-byte
		image[i][0] = getc(streamIn);  // reverse-order array indexing fixes RGB issue...
		
		if (numstr == 0 && numchar == 1438 || numstr == 1 && numchar == 1439 || numstr == 2 && numchar == 1439) {
			code.push_back(current);
			numstr = (numstr + 1) % 3;
			numchar = 0;
			current = "";
		}
		current += (image[i][0] + image[i][1] + image[i][2] < 256 ? '1' : ' ');
		numchar++;
	}

	reverse(code.begin(), code.end());
	for (int i = 0; i < code.size(); i++)
		cout << code[i] << '\n';

	return 0;
}