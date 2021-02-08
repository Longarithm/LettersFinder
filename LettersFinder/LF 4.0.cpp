#define _CRT_SECURE_NO_WARNINGS //for visual studio 2013

#include <iostream>
#include <fstream> 
#include <cstring>
#include <string>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

//maximal constants, can be bigger
const int HEIGHT = 1000;
const int WIDTH = 2000;
const int MAXAREA = 200;
const int dirx[] = {0, 0, -1, 1};
const int diry[] = {-1, 1, 0, 0};

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

struct point {
	int x, y;

	point() {
		x = y = 0;
	}
	point(int _x, int _y) {
		x = _x;
		y = _y;
	}
	bool operator==(point p) {
		return x == p.x && y == p.y;
	}
};

bool operator<(const point a, const point b) {
	return a.x < b.x || a.x == b.x && a.y < b.y;
}

point operator+(point a, point b) {
	return point(a.x + b.x, a.y + b.y);
}

int distance(point a, point b) {
	return abs(a.x - b.x) + abs(a.y - b.y);
}

struct figure {
	vector<point> f;
	point center;

	figure() {
		f.clear();
	}
	figure(point _center) {
		f.clear();
		center = _center;
	}
};

FILE *streamIn;

void SkipBits(int n) {
	for (int i = 0; i < n; i++) {
		getc(streamIn);
	}
}

inline bool Black(int r, int g, int b) {
	return (r < 200) && (g < 200) && (b < 200); 
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

void InitializeFiles() {
	streamIn = fopen("./sample2_mono_small.bmp", "r");
    //streamIn = fopen("./sample1_antialiased.bmp", "r");         
    //streamIn = fopen("./sample3_mono_large.bmp", "r");
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

int pic(point p) {
	if (p.x < 0 || p.x >= height || p.y < 0 || p.y >= width)
		return 0;
	return code[p.x][p.y];
}

vector<point> FindSameFigures(figure omega) {
	vector<point> ans;
	ans.clear();

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			point cur_center = point(i, j);

			if (omega.center == cur_center)
				continue;

			int failure = 0;
			for (int k = 0; k < omega.f.size(); k++) {
				if (pic(omega.center + omega.f[k]) != pic(cur_center + omega.f[k]))
					failure++;
				if (failure >= 1)
					break;
			}
			
			if (failure <= 0)
				ans.push_back(cur_center);
		}

	return ans;
}

void PrintFigure(figure omega) {
	sort(omega.f.begin(), omega.f.end());

	point minp, maxp;
	for (int i = 0; i < omega.f.size(); i++) {
		minp.x = min(minp.x, omega.f[i].x);
		minp.y = min(minp.y, omega.f[i].y);
		maxp.x = max(maxp.x, omega.f[i].x);
		maxp.y = max(maxp.y, omega.f[i].y);
	}

	char w[MAXAREA][MAXAREA];

	for (int i = 0; i < MAXAREA; i++)
		for (int j = 0; j < MAXAREA; j++)
			w[i][j] = ' ';
	for (int i = 0; i < omega.f.size(); i++)
		w[omega.f[i].x - minp.x][omega.f[i].y - minp.y] = 
			(pic(omega.center + omega.f[i]) ? '#' : '_');

	cout << "<------------------------------------------------------->\n";
	for (int i = 0; i <= maxp.x - minp.x; i++) {
		for (int j = 0; j <= maxp.y - minp.y; j++)
			cout << w[i][j];
		cout << '\n';
	}
	cout << '\n';
}

figure ExpandFigure(figure omega) { 
	vector<point> centers;
	set<point> used, new_coords; //relative coordinates

	for (int i = 0; i < omega.f.size(); i++)
		used.insert(omega.f[i]);

	while (1) {
		
		//DEBUG
		//PrintFigure(omega);

		centers.clear();
		centers = FindSameFigures(omega);

		//finding new pretenders
		for (auto it : omega.f) {
			point cur_pt = it;
			for (int d = 0; d < 4; d++) {
				point new_pt = point(cur_pt.x + dirx[d], cur_pt.y + diry[d]);

				if (pic(omega.center + new_pt) == 0 && max(abs(new_pt.x), abs(new_pt.y)) >= 10)
					continue;

				if (used.find(new_pt) == used.end() && new_coords.find(new_pt) == new_coords.end())
					new_coords.insert(new_pt);
			}
		}
		
		//choose some random blocks
		random_shuffle(centers.begin(), centers.end());
		int cent_number = min(15, int(centers.size()));
		centers.resize(cent_number);

		if (cent_number <= 5 || new_coords.empty())
			break;

		//check new coordinates
		for (auto it : new_coords) {
			point add_pt = it;

			int failure = 0;
			for (int i = 0; i < cent_number; i++) {
				if (pic(omega.center + add_pt) != pic(centers[i] + add_pt))
					failure++;
			}

			if (failure <= 2) {
				omega.f.push_back(add_pt);
			}
			used.insert(add_pt);
		}

		new_coords.clear();
	}

	return omega;
}

void FindAllAreas() {
	//make picture smaller
	height = 1000;
	width = 1000;
	
	//generate square 5x5
	figure omega(point(0, 0));
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			omega.f.push_back(point(i, j));

	//make some tries...
	for (int iter = 0; iter < 10; iter++) {
		omega.f.clear();
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 5; j++)
				omega.f.push_back(point(i, j));

		//get random point
		do 
			omega.center = point(rand() % height, rand() % width);
		while (pic(omega.center) == 0);

		omega = ExpandFigure(omega);	
		PrintFigure(omega);
	}

	/* TEST
	omega.center = point(38, 38);
	omega = ExpandFigure(omega);	
	PrintFigure(omega);
	*/
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
					symbol = '#';
					break;
                case 'I': symbol='1'; 
					break;
                case 'x': symbol='x';
					break;
				case 'o': symbol='o';
					break;       
				default: 
					symbol = '.';
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
	FindAllAreas();
	ConvertFromCode();
	OutputAnswer();

	return 0;
}
