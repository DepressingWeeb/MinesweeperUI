#include "Class.h"
#include "Utils.h"
#include "Global.h"
#include <set>
#include <time.h>
#include <deque>
#include <tuple>

Grid::Grid(){}

Grid::Grid(int n_rows, int n_cols, int bomb_number) {
	this->rows = n_rows;
	this->cols = n_cols;
	this->bombNumber = bomb_number;
	this->lose = false;
	this->visible = vector < vector < bool >>(n_rows, vector < bool >(n_cols, false));
	this->flag = vector < vector < bool >>(n_rows, vector < bool >(n_cols, false));
	initResources();
	this->gridContent = initGridContent(n_rows, n_cols, bomb_number);
	this->gridCoordinate = initGridCoordinate(n_rows, n_cols);
}

void Grid::initResources() {
	img_1 = loadImgTexture("resources/Cells/1.png");
	img_2 = loadImgTexture("resources/Cells/2.png");
	img_3 = loadImgTexture("resources/Cells/3.png");
	img_4 = loadImgTexture("resources/Cells/4.png");
	img_5 = loadImgTexture("resources/Cells/5.png");
	img_6 = loadImgTexture("resources/Cells/6.png");
	img_7 = loadImgTexture("resources/Cells/7.png");
	img_8 = loadImgTexture("resources/Cells/8.png");
	img_empty = loadImgTexture("resources/Cells/empty.png");
	img_bomb = loadImgTexture("resources/Cells/bomb.png");
	img_flag = loadImgTexture("resources/Cells/flag.png");
	img_unknown = loadImgTexture("resources/Cells/unknown.png");
	arr[0] = img_empty;
	arr[1] = img_1;
	arr[2] = img_2;
	arr[3] = img_3;
	arr[4] = img_4;
	arr[5] = img_5;
	arr[6] = img_6;
	arr[7] = img_7;
	arr[8] = img_8;
	arr[9] = img_bomb;
	arr[10] = img_flag;
	arr[11] = img_unknown;

}

inline bool Grid::checkValid(int row, int col, int rows, int cols) {
	return row >= 0 && row < rows&& col >= 0 && col < cols;
}

vector < vector < SDL_Rect >> Grid::initGridCoordinate(int rows, int cols) {
	SDL_Rect tempRect;
	vector < vector < SDL_Rect >> gridCoordinate(rows, vector < SDL_Rect >(cols, tempRect));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			createRect(&gridCoordinate[i][j], j * CELL_SIZE, i * CELL_SIZE + 50, CELL_SIZE, CELL_SIZE);
		}
	}
	return gridCoordinate;
}

vector < vector < int >> Grid::initGridContent(int rows, int cols, int bomb_number) {
	vector < vector < int >> gridContent(rows, vector < int >(cols, 0));
	set < pair < int, int >> bombCoordinate;
	int row, col, intersect;
	srand(time(0));
	while (true) {
		row = rand() % rows;
		col = rand() % cols;
		bombCoordinate.insert(make_pair(row, col));
		if (bombCoordinate.size() >= bomb_number) break;
	}
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (bombCoordinate.find(make_pair(i, j)) != bombCoordinate.end()) {
				gridContent[i][j] = -1;
				continue;
			}
			set < pair < int, int >> adjacent = {
			   make_pair(i - 1, j - 1),
			   make_pair(i - 1, j),
			   make_pair(i - 1, j + 1),
			   make_pair(i, j - 1),
			   make_pair(i, j + 1),
			   make_pair(i + 1, j - 1),
			   make_pair(i + 1, j),
			   make_pair(i + 1, j + 1)
			};
			intersect = 0;
			for (auto p : adjacent) {
				if (bombCoordinate.find(p) != bombCoordinate.end()) intersect++;
			}
			gridContent[i][j] = intersect;
		}
	}
	return gridContent;
}

void Grid::drawGridContent(int r , int c , int frames ) {
	int idx = 0;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (i == r && j == c) continue;
			if (visible[i][j]) {
				auto it = find(toAnimate.begin(), toAnimate.end(), make_pair(i, j));
				if (r != -1 && it != toAnimate.end() && it - toAnimate.begin() > frames) {
					SDL_RenderCopy(gRenderer, img_unknown, NULL, &gridCoordinate[i][j]);
					continue;
				}
				if (gridContent[i][j] == -1) idx = 9;
				else idx = gridContent[i][j];
			}
			else {
				if (flag[i][j]) idx = 10;
				else idx = 11;
			}
			SDL_RenderCopy(gRenderer, arr[idx], NULL, &gridCoordinate[i][j]);
		}
	}
	return;
}

void Grid::handleMouseEvent(SDL_Event* e) {
	int row, col;
	row = (e->button.y - 50) / CELL_SIZE;
	col = e->button.x / CELL_SIZE;
	//left-mouse click
	if (e->button.button == SDL_BUTTON_LEFT) BFS(row, col);
	else if (e->button.button == SDL_BUTTON_RIGHT) flag[row][col] = true;
	return;
}

void Grid::BFS(int row, int col) {
	deque <pair<int, int>> cells;
	cells.push_back(make_pair(row, col));
	while (!cells.empty()) {
		tie(row, col) = cells.front();
		cells.pop_front();
		if (checkValid(row, col, rows, cols) == false) {
			continue;
		}
		else if (visible[row][col]) {
			continue;
		}
		else if (gridContent[row][col] > 0) {
			visible[row][col] = true;
			toAnimate.push_back(make_pair(row, col));
		}
		else if (gridContent[row][col] == 0) {
			visible[row][col] = true;
			toAnimate.push_back(make_pair(row, col));
			cells.push_back(make_pair(row - 1, col - 1));
			cells.push_back(make_pair(row - 1, col));
			cells.push_back(make_pair(row - 1, col + 1));
			cells.push_back(make_pair(row, col - 1));
			cells.push_back(make_pair(row, col + 1));
			cells.push_back(make_pair(row + 1, col - 1));
			cells.push_back(make_pair(row + 1, col));
			cells.push_back(make_pair(row + 1, col + 1));
		}
		else {
			lose = true;
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					if (gridContent[i][j] == -1) visible[i][j] = true;
				}
			}
			drawGridContent();
			SDL_RenderPresent(gRenderer);
			SDL_Texture* bombExplode = NULL;
			SDL_Rect rect;
			for (int i = 0; i <= 16; i++) {
				bombExplode = loadImgTexture("resources/BombExplode/explode-" + to_string(i) + ".png");
				SDL_RenderCopy(gRenderer, bombExplode, NULL, &gridCoordinate[row][col]);
				SDL_RenderPresent(gRenderer);
				SDL_Delay(100);
			}
		}
	}
}

bool Grid::checkWin() {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (not visible[i][j] && gridContent[i][j] > -1) return false;
		}
	}
	return true;
}

bool Grid::checkLose() {
	return lose;
}