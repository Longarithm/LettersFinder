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
const int MAXLEN = 20;

struct hash_function {
	long long pow[2], p[2][WIDTH]; //max(HEIGHT, WIDTH)

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

FILE *streamIn;

void SkipBits(int n) {
	for (int i = 0; i < n; i++) {
		getc(streamIn);
	}
}

inline bool Black(int r, int g, int b) {
	return r == 0 && g == 0 && b == 0;
}

inline int GetBitNumber() {
	int result = 0, byte;
	for (int i = 0; i < 4; i++) {
		byte = getc(streamIn);
		result += byte << (i * 8);
	}
	return result;
}

int height, width, area;
int image[HEIGHT][WIDTH][3], pixel[3];
int code[HEIGHT][WIDTH];
int numstr, numchar;
vector<string> out;
string current;
hash_function hf;
long long prefix_hash[HEIGHT][WIDTH];

inline long long CountHash(int x, int y, int dx, int dy) {
	if (x < 0 || y < 0)
		return 0;
	long long result = prefix_hash[x + dx - 1][y + dy - 1];
	if (x > 0)
		result -= prefix_hash[x - 1][y + dy - 1] * hf.p[0][dx];
	if (y > 0)
		result -= prefix_hash[x + dx - 1][y - 1] * hf.p[1][dy];
	if (x > 0 && y > 0)
		result -= prefix_hash[x - 1][y - 1] * hf.p[0][dx] * hf.p[1][dy];
	return result;
}

void InitializeFiles() {
	streamIn = fopen("./sample2_mono_small.bmp", "r");

	if (streamIn == (FILE *)0){
		printf("File opening error occurred. Program exited.\n");
		exit(0);
	}

	freopen("output.txt", "w", stdout);
}

void ScanPicture() {
	SkipBits(18); //strip out BMP header

	//count size of image
	width = GetBitNumber();
	height = GetBitNumber();
	area = height * width;

	SkipBits(18 * 3 - 18 - 8); //skip last bits in header...

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

void CountHashPrefixes() {
	hf = hash_function(5, 7);

	for (int k = 0; k < 1; k++) {
		prefix_hash[0][0] = code[0][0];
		for (int j = 1; j < width; j++)
			prefix_hash[0][j] = prefix_hash[0][j - 1] * hf.pow[1] + code[0][j];

		for (int i = 1; i < height; i++) {
			prefix_hash[i][0] = prefix_hash[i - 1][0] * hf.pow[0] + code[i][0];

			for (int j = 1; j < width; j++) {
				prefix_hash[i][j] = prefix_hash[i - 1][j] * hf.pow[0];
				prefix_hash[i][j] += prefix_hash[i][j - 1] * hf.pow[1];
				prefix_hash[i][j] -= prefix_hash[i - 1][j - 1] * hf.pow[0] * hf.pow[1];

				prefix_hash[i][j] += code[i][j];
			}
		}
	}	
}

void CalculateHashes(int M, int N, long long hash_MxN[HEIGHT][WIDTH]) { //this gives array[-,-] of Hashes of MxN rectangles 
	for (int i = 0; i < height - M + 1; i++) {
		for (int j = 0; j < width - N + 1; j++) {
			hash_MxN[i][j] = CountHash(i, j, M, N);
		}
	}
}

map<long long, int> CountOccurences(int M, int N, long long hash_MxN[HEIGHT][WIDTH]) { 
	//counts number of occurences of each MxN rectangle, 
	//actually counts the number of occurences of MxN rectangels with the same hash

	CalculateHashes(M, N, hash_MxN);
	map<long long, int> number_of_occur;
	number_of_occur.clear();
	for (int i = 0; i < height - M + 1; i++) {
		for (int j = 0; j < width - N + 1; j++) {
			number_of_occur[hash_MxN[i][j]]++;
		}
	}

	return number_of_occur;
}

long long hash_MxN[HEIGHT][WIDTH], hash_MxN1[HEIGHT][WIDTH], hash_M1xN[HEIGHT][WIDTH];
map<long long, int> number_of_occur_MxN, number_of_occur_MxN1, number_of_occur_M1xN;

void FindFixedRectangles(int M, int N) { 
    int M1 = M + 1, N1 = N + 1; 
	
	number_of_occur_MxN = CountOccurences(M, N, hash_MxN);
	number_of_occur_MxN1 = CountOccurences(M, N1, hash_MxN1);
	number_of_occur_M1xN = CountOccurences(M1, N, hash_M1xN);

	for (int i = 0; i < height - M + 1; i++) {
		for (int j = 0; j < width - N + 1; j++) {
			
			if (number_of_occur_MxN1[hash_MxN1[i][j]] < number_of_occur_MxN[hash_MxN[i][j]]
			&& number_of_occur_M1xN[hash_M1xN[i][j]] < number_of_occur_MxN[hash_MxN[i][j]]
			&& (j > 0 || number_of_occur_MxN1[hash_MxN1[i][j - 1]] < number_of_occur_MxN[hash_MxN[i][j]])
			&& (i > 0 || number_of_occur_M1xN[hash_M1xN[i - 1][j]] < number_of_occur_MxN[hash_MxN[i][j]])) { 
			
				//so, we have found a distinguished rectangle! 
				
				//"distinguish" its corners

				//code[i][j] = 2;
				//code[i + M - 1][j + N - 1] = 2;

				//or just print it
				printf("%d %d %d %d is a distingished rectangle\n", i + 1, j + 1, i + M, j + N); 
				
				//make xor of say 3rd byte of each pixel in n x m -> M, N with a random number                       
				//and then write everything to the same file (or a copy of it)
			}
		}
	}	

}

void FindAllRectangles() {
	//H=W=100 (or we have very slow realization)
	height = 400;
	width = 400;

	int M = 8, N = 5;
	FindFixedRectangles(M, N);
	//here we can try other sizes...				
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
	//output all strings
	for (int i = 0; i < out.size(); i++)
		cout << out[i] << '\n';
}

int main() {

	InitializeFiles();
	ScanPicture();
	ConvertToCode();
	CountHashPrefixes();
	FindAllRectangles();
	ConvertFromCode();
	OutputAnswer();
	
	return 0;
}