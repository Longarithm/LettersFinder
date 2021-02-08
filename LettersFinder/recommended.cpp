Hi ,

thanks, good code! 

but ya ne uweren, chto ya wse ponjal. Mozhno poprosit'
Was dopisat' sledujushie funkcii ? 

void CalculateHashes(int M,N; array? Hash_at_MxN ) { // this gives array[-,-] of Hashas of MxN rectangles 
        for (int n = 1; n <= image->length - N; n) {
                for (int m = 1; n <= image->height-M ; m) {
                       Hash_at_MxN[m,n]=CountHash(m,n,m+M,n+N)     }}
     }



void CountOccurences(int M,N, array? Hashes, array? Number_of_Occurences) { // (tries to) counts number of occurences of each MxN rectangle, 
                                // actually counts the number of occurences of MxN rectangels with the same hash
                                // this assumes that hash is a 2-3 bytes ... 
      for (int n = 1; n <= image->length - N; n) {
                for (int m = 1; n <= image->height-M ; m) {
                       Number_of_Occurences[Hash_at_MxN[m,n]]=Number_of_Occurences[Hash_at_MxN[m,n]] + 1  }}
     }



void FindRectangeles(int M, N) { 
        
    M1=M+1; N1=N+1; 
    for (int n = 1; n <= image->length - N1; n) {
                for (int m = 1; n <= image->height-M1 ; m) {
                    
              if ( Number_of_Occurences_MxN1[Hash_at_MxN[m,n]]< Number_of_Occurences_MxN[Hash_at_MxN[m,n]]
                    & Number_of_Occurences_M1xN[Hash_at_MxN[m,n]]< Number_of_Occurences_MxN[Hash_at_MxN[m,n]]
                  & Number_of_Occurences_MxN1[Hash_at_MxN[m,n-1]]< Number_of_Occurences_MxN[Hash_at_MxN[m,n]]
                    & Number_of_Occurences_M1xN[Hash_at_MxN[m-1,n]]< Number_of_Occurences_MxN[Hash_at_MxN[m,n]])
                  { // then we have found a distinguished rectangle! 
                      printf( %?, "n x m -> M, N is a distingished rectangle") 
                      make xor of say 3rd byte of each pixel in n x m -> M, N with a random number                       
                      and then write everything to the same file (or a copy of it)
                  }
                 
     }

       }
 }



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
const int MAXDIFF = 15;
const int SQUARESCHECK = 10;
const int dx[] = {1, -1, 0, 0};
const int dy[] = {0, 0, 1, -1};

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

struct submatrix {
	int x, y;
	submatrix() {};

	submatrix(int _x, int _y) {
		x = _x, y = _y;
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
hash_function hf[2];
long long hash_val[2][HEIGHT][WIDTH];

inline long long CountSubHash(int id, int x, int y, int dx, int dy) {
	if (x < 0 || y < 0)
		return 0;
	long long result = hash_val[id][x + dx - 1][y + dy - 1];
	if (x > 0)
		result -= hash_val[id][x - 1][y + dy - 1] * hf[id].p[0][dx];
	if (y > 0)
		result -= hash_val[id][x + dx - 1][y - 1] * hf[id].p[1][dy];
	if (x > 0 && y > 0)
		result -= hash_val[id][x - 1][y - 1] * hf[id].p[0][dx] * hf[id].p[1][dy];
	return result;
}

void InitializeFiles() {
      streamIn = fopen("./sample2_mono_small.bmp", "r");
      //  streamIn = fopen("./sample3_mono_large.bmp", "r");          

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
			getc(streamIn);     // misha:> note these bits seem to be always null(?)
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

void CountHashes() {
	//get hashes
	hf[0] = hash_function(5, 7);
	//temporarily unused
	//hf[1] = hash_function(257, 1999);

	for (int k = 0; k < 1; k++) {
		hash_val[k][0][0] = code[0][0];
		for (int j = 1; j < width; j++)
			hash_val[k][0][j] = hash_val[k][0][j - 1] * hf[k].pow[1] + code[0][j];

		for (int i = 1; i < height; i++) {
			hash_val[k][i][0] = hash_val[k][i - 1][0] * hf[k].pow[0] + code[i][0];

			for (int j = 1; j < width; j++) {
				hash_val[k][i][j] = hash_val[k][i - 1][j] * hf[k].pow[0];
				hash_val[k][i][j] += hash_val[k][i][j - 1] * hf[k].pow[1];
				hash_val[k][i][j] -= hash_val[k][i - 1][j - 1] * hf[k].pow[0] * hf[k].pow[1];

				hash_val[k][i][j] += code[i][j];
			}
		}
	}	
}

map<long long, int> hashlib[MAXLEN + 1][MAXLEN + 1];

void FindRectangles() {
	//H=W=100 (or we have very slow realization)
	height = 550;
	width = 250;

	long long curhash, newhash;
	vector<submatrix> emp;
	emp.clear();
	for (int i = 0; i <= MAXLEN; i++)
		for (int j = 0; j <= MAXLEN; j++) {
			hashlib[i][j].clear();
		}

	for (int n = 1; n <= MAXLEN; n++) {
		for (int m = max(n - MAXDIFF, 1); m <= min(n + MAXDIFF, MAXLEN); m++) {
			
			for (int i = 0; i < height - n + 1; i++) {
				for (int j = 0; j < width - m + 1; j++) {
					curhash = CountSubHash(0, i, j, n, m);
					if (hashlib[n][m].find(curhash) == hashlib[n][m].end()) {
						hashlib[n][m][curhash] = 1;
					}
					hashlib[n][m][curhash]++;
				}
			}
		}
	}

	int stringnumber, covernumber;

	for (int n = 1; n <= MAXLEN - 1; n++) {
		for (int m = max(n - MAXDIFF + 1, 1); m <= min(n + MAXDIFF - 1, MAXLEN - 1); m++) {
			//cerr << n << ' ' << m << '\n';

			for (int i = 0; i < height - n + 1; i++) {
				for (int j = 0; j < width - m + 1; j++) {
					bool good = 1;

					curhash = CountSubHash(0, i, j, n, m);

					if (curhash == 0)
						continue;

					stringnumber = hashlib[n][m][curhash];
					covernumber = 0;
					//if we can cover our square with more squares
					for (int d = 0; d < 4; d++) {
						int ci = min(i, i + dx[d]), cj = min(j, j + dy[d]);
						int cn = n + abs(dx[d]), cm = m + abs(dy[d]);
						newhash = CountSubHash(0, ci, cj, cn, cm);
						covernumber += hashlib[cn][cm][newhash];
					}

					good = covernumber < stringnumber;
					if (good) {
						printf("%d %d %d %d %d %d\n", i, j, i + n - 1, j + m - 1, stringnumber, covernumber);
					}
				}
			}
		}
	}
}

void ConvertFromCode() {
	//convert bytecode into string
	for (int i = 0; i < height; i=i+2) { // i=0,2,4,...,height 
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
	CountHashes();
	FindRectangles();
	ConvertFromCode();
	OutputAnswer();
	
	return 0;
}
