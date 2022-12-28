#pragma once

#include <SDL.h>

#include <SDL_image.h>

#include <SDL_ttf.h>

#include <vector>

using namespace std;
class Grid {
public:
	int rows;
	int cols;
	int bombNumber;
	bool lose;
	vector < vector < bool >> visible;
	vector < vector < bool >> flag;
	vector < vector < int >> gridContent;
	vector < vector < SDL_Rect >> gridCoordinate;
	SDL_Texture* img_1 = NULL;
	SDL_Texture* img_2 = NULL;
	SDL_Texture* img_3 = NULL;
	SDL_Texture* img_4 = NULL;
	SDL_Texture* img_5 = NULL;
	SDL_Texture* img_6 = NULL;
	SDL_Texture* img_7 = NULL;
	SDL_Texture* img_8 = NULL;
	SDL_Texture* img_empty = NULL;
	SDL_Texture* img_bomb = NULL;
	SDL_Texture* img_flag = NULL;
	SDL_Texture* img_unknown = NULL;
	SDL_Texture* arr[12];
	Grid();
	Grid(int n_rows, int n_cols, int bomb_number);
	void initResources();
	static inline bool checkValid(int row, int col, int rows, int cols);
	static vector < vector < SDL_Rect >> initGridCoordinate(int rows, int cols);
	static vector < vector < int >> initGridContent(int rows, int cols, int bomb_number);
	void drawGridContent(int r = -1, int c = -1, int frames = -1);
	void handleMouseEvent(SDL_Event* e);
	void BFS(int row, int col);
	bool checkWin();
	bool checkLose();
};