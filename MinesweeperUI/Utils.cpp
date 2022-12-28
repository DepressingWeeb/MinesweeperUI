#include "Utils.h"
#include "Global.h"
#include "FileIO.h"
#include "Class.h"
#include <fstream>
#include <tuple>
#include <algorithm>
#include <sstream>
#include <iomanip>
using namespace std;

void init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	IMG_Init(IMG_INIT_JPG);
	TTF_Init();
	window = SDL_CreateWindow("Minesweeper", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
}
bool isExist(const string& name) {
	ifstream f(name.c_str());
	return f.good();
}
SDL_Rect* createRect(SDL_Rect* rect, int x, int y, int w, int h) {
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
	return rect;
}

SDL_Texture* loadImgTexture(string path) {
	SDL_Texture* newTexture = NULL;
	newTexture = IMG_LoadTexture(gRenderer, path.c_str());
	return newTexture;
}

void quitGame() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(gRenderer);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	exit(0);
}

void quitAndSave(Grid* grid) {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(gRenderer);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	saveGame(grid);
	exit(0);
}

void createButton(SDL_Texture* button, SDL_Texture* buttonOnHover, bool leftMouseDown, int x, int y, int w, int h, void(*func)()) {
	int mouseX, mouseY;
	SDL_Rect buttonRect;
	SDL_GetMouseState(&mouseX, &mouseY);

	if (x < mouseX && mouseX < x + w && y < mouseY && mouseY < y + h) {
		if (leftMouseDown && func != NULL) func();
		SDL_RenderCopy(gRenderer, buttonOnHover, NULL, createRect(&buttonRect, x, y, w, h));
	}
	else {
		SDL_RenderCopy(gRenderer, button, NULL, createRect(&buttonRect, x, y, w, h));
	}
}

void createText(TTF_Font* font, SDL_Color color, string text, int x, int y, int w, int h) {
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
	SDL_FreeSurface(textSurface);
	SDL_Rect rect;
	SDL_RenderCopy(gRenderer, texture, NULL, createRect(&rect, x, y, w, h));
	SDL_DestroyTexture(texture);
}

bool isAllDigit(string s) {
	for (int i = 0; i < s.size(); i++) {
		if (not isdigit(s[i])) return false;
	}
	return true;
}

void filterText(string* text1, string* text2, string* text3, int h, int v) {
	if (*text1 != "") *text1 = to_string(min(stoi(*text1), v / CELL_SIZE));
	if (*text2 != "") *text2 = to_string(min(stoi(*text2), h / CELL_SIZE));
	if (*text3 != "" && *text1 != "" && *text2 != "") *text3 = to_string(min(stoi(*text3), stoi(*text1) * stoi(*text2) - 1));
}

string getTime(Uint32 timeInMili) {
	int totalSeconds = timeInMili / 1000;
	string minutes = to_string(totalSeconds / 60);
	string seconds = to_string(totalSeconds % 60);
	if (minutes.size() == 1) minutes = "0" + minutes;
	if (seconds.size() == 1) seconds = "0" + seconds;
	return minutes + ":" + seconds;
}

bool cmpScore(tuple<string, string, Uint32, int, int> t1, tuple<string, string, Uint32, int, int> t2) {
	return get<2>(t1) < get<2>(t2);
}

vector<tuple<string, string, Uint32, int, int>> sortScore() {
	vector<tuple<string, string, Uint32, int, int>> ans;
	string d, t;
	Uint32 totalTime;
	int rows, cols;
	ifstream fin("score.txt");
	string s;
	while (getline(fin, s)) {
		stringstream ss(s);
		ss >> d >> t >> totalTime >> rows >> cols;
		ans.push_back(make_tuple(d, t, totalTime, rows, cols));
	}
	fin.close();
	sort(ans.begin(), ans.end(), cmpScore);
	return ans;

}

string doubleToString(double d) {
	ostringstream streamObj;
	streamObj << fixed << setprecision(2) << d;
	string strObj = streamObj.str();
	return strObj;
}
