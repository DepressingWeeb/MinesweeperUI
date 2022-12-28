#define NOMINMAX

#include <SDL.h>

#include <SDL_image.h>

#include <SDL_ttf.h>

#include <iostream>

#include <string>

#include <vector>

#include <set>

#include <tuple>

#include <Windows.h>

#include <time.h>

#include <fstream>

#include <deque>

#include <utility>

#include <algorithm>

#include <sstream>

#include <iomanip>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;
const int CELL_SIZE = 25;

//init global variables
SDL_Window* window = NULL;
SDL_Renderer* gRenderer = NULL;
bool isGameLoaded = false;
vector<pair<int, int>> toAnimate;

void init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	IMG_Init(IMG_INIT_JPG);
	TTF_Init();
	window = SDL_CreateWindow("SDL Tutorial", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
}
inline bool isExist(const string& name) {
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
	SDL_Texture* img_1 = loadImgTexture("resources/Cells/1.png");
	SDL_Texture* img_2 = loadImgTexture("resources/Cells/2.png");
	SDL_Texture* img_3 = loadImgTexture("resources/Cells/3.png");
	SDL_Texture* img_4 = loadImgTexture("resources/Cells/4.png");
	SDL_Texture* img_5 = loadImgTexture("resources/Cells/5.png");
	SDL_Texture* img_6 = loadImgTexture("resources/Cells/6.png");
	SDL_Texture* img_7 = loadImgTexture("resources/Cells/7.png");
	SDL_Texture* img_8 = loadImgTexture("resources/Cells/8.png");
	SDL_Texture* img_empty = loadImgTexture("resources/Cells/empty.png");
	SDL_Texture* img_bomb = loadImgTexture("resources/Cells/bomb.png");
	SDL_Texture* img_flag = loadImgTexture("resources/Cells/flag.png");
	SDL_Texture* img_unknown = loadImgTexture("resources/Cells/unknown.png");
	SDL_Texture* arr[12] = {
	   img_empty,
	   img_1,
	   img_2,
	   img_3,
	   img_4,
	   img_5,
	   img_6,
	   img_7,
	   img_8,
	   img_bomb,
	   img_flag,
	   img_unknown
	};

	Grid() {}
	Grid(int n_rows, int n_cols, int bomb_number) {
		this->rows = n_rows;
		this->cols = n_cols;
		this->bombNumber = bomb_number;
		this->lose = false;
		this->visible = vector < vector < bool >>(n_rows, vector < bool >(n_cols, false));
		this->flag = vector < vector < bool >>(n_rows, vector < bool >(n_cols, false));
		this->gridContent = initGridContent(n_rows, n_cols, bomb_number);
		this->gridCoordinate = initGridCoordinate(n_rows, n_cols);
	}

	static inline bool checkValid(int row, int col, int rows, int cols) {
		return row >= 0 && row < rows&& col >= 0 && col < cols;
	}

	static vector < vector < SDL_Rect >> initGridCoordinate(int rows, int cols) {
		SDL_Rect tempRect;
		vector < vector < SDL_Rect >> gridCoordinate(rows, vector < SDL_Rect >(cols, tempRect));
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				createRect(&gridCoordinate[i][j], j * CELL_SIZE, i * CELL_SIZE + 50, CELL_SIZE, CELL_SIZE);
			}
		}
		return gridCoordinate;
	}

	static vector < vector < int >> initGridContent(int rows, int cols, int bomb_number) {
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

	void drawGridContent(int r = -1, int c = -1, int frames = -1) {
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

	void handleMouseEvent(SDL_Event* e) {
		int row, col;
		row = (e->button.y - 50) / CELL_SIZE;
		col = e->button.x / CELL_SIZE;
		//left-mouse click
		if (e->button.button == SDL_BUTTON_LEFT) recur(row, col);
		else if (e->button.button == SDL_BUTTON_RIGHT) flag[row][col] = true;
		return;
	}

	void recur(int row, int col) {
		/*
		if (checkValid(row, col, rows, cols) == false) {
			return;
		}
		else if (visible[row][col]) {
			return;
		}
		else if (gridContent[row][col] > 0) {
			visible[row][col] = true;
		}
		else if (gridContent[row][col] == 0) {
			visible[row][col] = true;
			recur(row - 1, col - 1);
			recur(row - 1, col);
			recur(row - 1, col + 1);
			recur(row, col - 1);
			recur(row, col + 1);
			recur(row + 1, col - 1);
			recur(row + 1, col);
			recur(row + 1, col + 1);
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
				bombExplode = loadImgTexture("resources/BombExplode/explode-" + std::to_string(i) + ".png");
				SDL_RenderCopy(gRenderer, bombExplode, NULL, &gridCoordinate[row][col]);
				SDL_RenderPresent(gRenderer);
				SDL_Delay(100);
			}
		}*/
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

	bool checkWin() {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if (not visible[i][j] && gridContent[i][j] > -1) return false;
			}
		}
		return true;
	}

	bool checkLose() {
		return lose;
	}
};
void saveGame(Grid* grid) {
	ofstream outfile("save.txt");
	outfile << grid->rows << " " << grid->cols << " " << grid->bombNumber << " " << grid->lose << endl;
	for (int i = 0; i < grid->rows; i++) {
		for (int j = 0; j < grid->cols; j++) {
			outfile << grid->gridContent[i][j] << " ";
		}
		outfile << endl;
	}
	for (int i = 0; i < grid->rows; i++) {
		for (int j = 0; j < grid->cols; j++) {
			outfile << grid->visible[i][j] << " ";
		}
		outfile << endl;
	}
	for (int i = 0; i < grid->rows; i++) {
		for (int j = 0; j < grid->cols; j++) {
			outfile << grid->flag[i][j] << " ";
		}
		outfile << endl;
	}
	outfile.close();
}
void saveScore(Uint32 timeInMili, int rows, int cols) {
	fstream outfile("score.txt", ios::out | ios::app);
	time_t t = time(0);   // get time now
	struct tm now;
	localtime_s(&now, &t);
	char tnow[80];
	strftime(tnow, sizeof(tnow), "%Y-%m-%d %X", &now);
	outfile <<tnow<<" ";
	outfile << timeInMili << " " << rows << " " << cols << endl;
	outfile.close();
}

Grid loadGameSave(int rows, int cols, int bombNumber, ifstream& fin) {
	Grid grid = Grid(rows, cols, bombNumber);
	int tmp;
	fin >> tmp;
	grid.lose = tmp;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			fin >> tmp;
			grid.gridContent[i][j] = tmp;
		}
	}
	int temp;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			fin >> temp;
			grid.visible[i][j] = temp;
		}
	}
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			fin >> temp;
			grid.flag[i][j] = temp;
		}
	}
	fin.close();
	return grid;
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
void quitFunc(bool* quit) {
	*quit = true;
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

void drawDifficulty(string* text1, string* text2, string* text3, TTF_Font* diffFont) {
	if (*text1 != "" && *text2 != "" && *text3 != "") {
		int rows = stoi(*text1);
		int cols = stoi(*text2);
		int bombNumber = stoi(*text3);
		float ratio = static_cast<float>(bombNumber) / static_cast<float>(rows * cols);
		if (ratio >= 0.25) {
			createText(diffFont, { 255,0,0 }, "HARD", 50, 550, 200, 80);
		}
		else if (ratio >= 0.125) {
			createText(diffFont, { 255,255,0 }, "MEDIUM", 50, 550, 200, 80);
		}
		else {
			createText(diffFont, { 0,255,0 }, "EASY", 50, 550, 200, 80);
		}
	}
}

tuple < int, int, int > gridSetting() {
	TTF_Font* numberFont = TTF_OpenFont("resources/Font/Lato-Light.ttf", 500);
	TTF_Font* letterFont = TTF_OpenFont("resources/Font/VeganStyle.ttf", 500);
	TTF_Font* diffFont = TTF_OpenFont("resources/Font/ParryHotter.ttf", 500);
	SDL_Texture* imgTextBox = loadImgTexture("resources/Textbox/txtbox.png");
	SDL_Texture* imgButtonPlay = loadImgTexture("resources/Large Buttons/Large Buttons/Play.png");
	SDL_Texture* imgButtonPlayColored = loadImgTexture("resources/Large Buttons/Colored Large Buttons/Play.png");
	SDL_Texture* bg = loadImgTexture("resources/BG/bg2.jpg");

	bool quit = false;
	bool leftMouseDown = false;
	int activeField = 0;
	string text1 = "";
	string text2 = "";
	string text3 = "";

	int horizontal = GetSystemMetrics(SM_CXSCREEN) - 100;
	int vertical = GetSystemMetrics(SM_CYSCREEN) - 200;

	SDL_Rect rect;
	SDL_Event e;

	SDL_StartTextInput();
	while (!quit) {
		SDL_RenderClear(gRenderer);
		SDL_RenderCopy(gRenderer, bg, NULL, createRect(&rect, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
		createText(letterFont, { 0 , 0 , 0 }, "Rows", 50, 50, 150, 50);
		createText(letterFont, { 0 , 0 , 0 }, "Columns", 50, 250, 200, 50);
		createText(letterFont, { 0 , 0 , 0 }, "Bombs", 50, 450, 200, 50);
		SDL_RenderCopy(gRenderer, imgTextBox, NULL, createRect(&rect, 300, 50, 270, 60));
		SDL_RenderCopy(gRenderer, imgTextBox, NULL, createRect(&rect, 300, 250, 270, 60));
		SDL_RenderCopy(gRenderer, imgTextBox, NULL, createRect(&rect, 300, 450, 270, 60));
		createText(numberFont, { 0 , 0 , 0 }, text1, (300 + 570 - 20) / 2 - text1.size() * 30 / 2, 55, text1.size() * 30, 50);
		createText(numberFont, { 0 , 0 , 0 }, text2, (300 + 570 - 20) / 2 - text2.size() * 30 / 2, 255, text2.size() * 30, 50);
		createText(numberFont, { 0 , 0 , 0 }, text3, (300 + 570 - 20) / 2 - text3.size() * 30 / 2, 455, text3.size() * 30, 50);
		createButton(imgButtonPlay, imgButtonPlayColored, leftMouseDown, 450, 575, 150, 50, NULL);
		drawDifficulty(&text1, &text2, &text3, diffFont);
		if (SDL_GetTicks64() % 1000 >= 500) {
			switch (activeField) {
			case 1:
				SDL_RenderDrawLine(gRenderer, (300 + 570 - 20) / 2 + text1.size() * 30 / 2, 60, (300 + 570 - 20) / 2 + text1.size() * 30 / 2, 50 + 50);
				break;
			case 2:
				SDL_RenderDrawLine(gRenderer, (300 + 570 - 20) / 2 + text2.size() * 30 / 2, 260, (300 + 570 - 20) / 2 + text2.size() * 30 / 2, 250 + 50);
				break;
			case 3:
				SDL_RenderDrawLine(gRenderer, (300 + 570 - 20) / 2 + text3.size() * 30 / 2, 460, (300 + 570 - 20) / 2 + text3.size() * 30 / 2, 450 + 50);
				break;
			}
		}
		leftMouseDown = false;
		if (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				/* Quit */
				quit = true;
				quitGame();
				break;
			case SDL_TEXTINPUT:
				if (not isAllDigit(e.text.text)) break;
				/* Add new text onto the end of our text */
				if (activeField == 1) text1 += e.text.text;
				else if (activeField == 2) text2 += e.text.text;
				else if (activeField == 3) text3 += e.text.text;
				filterText(&text1, &text2, &text3, horizontal, vertical);
				break;
			case SDL_MOUSEBUTTONDOWN:
				leftMouseDown = true;
				if (e.button.x > 300 && e.button.x < 300 + 270 && e.button.y > 50 && e.button.y < 50 + 60) activeField = 1;
				else if (e.button.x > 300 && e.button.x < 300 + 270 && e.button.y > 250 && e.button.y < 250 + 60) activeField = 2;
				else if (e.button.x > 300 && e.button.x < 300 + 270 && e.button.y > 450 && e.button.y < 450 + 60) activeField = 3;
				else activeField = 0;
				if (e.button.x > 450 && e.button.x < 450 + 150 && e.button.y > 575 && e.button.y < 575 + 50 && text1 != "" && text2 != "" && text3 != "") quit = true;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_BACKSPACE) {
					if (activeField == 1 && text1.size() != 0) text1.pop_back();
					else if (activeField == 2 && text2.size() != 0) text2.pop_back();
					else if (activeField == 3 && text3.size() != 0) text3.pop_back();
				}
			}
		}
		SDL_RenderPresent(gRenderer);
	}
	SDL_StopTextInput();
	return make_tuple(stoi(text1), stoi(text2), stoi(text3));
}

string getTime(Uint32 timeInMili) {
	int totalSeconds = timeInMili / 1000;
	string minutes = to_string(totalSeconds / 60);
	string seconds = to_string(totalSeconds % 60);
	if (minutes.size() == 1) minutes = "0" + minutes;
	if (seconds.size() == 1) seconds = "0" + seconds;
	return minutes + ":" + seconds;
}
void startMenu();
void newGame() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(gRenderer);
	startMenu();
}
void gameLoop() {
	ifstream fin;
	int rows, cols, bombNumber;
	if (not isGameLoaded) tie(rows, cols, bombNumber) = gridSetting();
	else {
		fin.open("save.txt");
		fin >> rows >> cols >> bombNumber;
	}
	cols = max(cols, 8);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(gRenderer);
	window = SDL_CreateWindow("SDL Tutorial", 100, 100, cols * CELL_SIZE, rows * CELL_SIZE + 100, SDL_WINDOW_SHOWN);
	gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	TTF_Font* timerFont = TTF_OpenFont("resources/Font/E1234.ttf", 500);
	SDL_Texture* newGameButton = loadImgTexture("resources/Large Buttons/Large Buttons/NewGame.png");
	SDL_Texture* newGameButtonColored = loadImgTexture("resources/Large Buttons/Colored Large Buttons/NewGame.png");
	SDL_Texture* quitButton = loadImgTexture("resources/Large Buttons/Large Buttons/Quit.png");
	SDL_Texture* quitButtonColored = loadImgTexture("resources/Large Buttons/Colored Large Buttons/Quit.png");
	SDL_Texture* losePrompt = loadImgTexture("resources/Prompt/lose.png");
	SDL_Texture* winPrompt = loadImgTexture("resources/Prompt/win2.png");
	SDL_Texture* bg2 = loadImgTexture("resources/BG/bg2.jpg");
	SDL_Rect timerBG = { 0 , 0 , cols * CELL_SIZE , 50 };
	SDL_Rect BG = { 0 , 0 , cols * CELL_SIZE, rows * CELL_SIZE + 50 };
	SDL_Rect buttonBG = { 0, rows * CELL_SIZE + 50 ,cols * CELL_SIZE ,50 };
	//SDL_Rect timerBG = { (cols * CELL_SIZE / 2) - 75, 0, 150, 50 };

	Grid grid = Grid(rows, cols, bombNumber);
	bool hasSave = false;
	if (isGameLoaded) {
		grid = loadGameSave(rows, cols, bombNumber, fin);
		isGameLoaded = false;
		hasSave = true;
	}
	//SDL_SetWindowSize(window, grid.cols * CELL_SIZE, grid.rows * CELL_SIZE);
	SDL_Event e;
	bool quit = false;
	bool leftMouseDown = false;
	Uint32 startTime = SDL_GetTicks();
	Uint32 finalTime = 0;
	int mouseX, mouseY, row, col;
	int animateTimes = -1;
	while (!quit) {
		//Clear Screen and set to last color set in SDL_SetRenderDrawColor()
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderFillRect(gRenderer, &timerBG);
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		if (not grid.checkLose() && not grid.checkWin()) {
			createText(timerFont, { 0 , 255 , 0 }, getTime(SDL_GetTicks() - startTime), (cols * CELL_SIZE / 2) - 75, 0, 150, 50);
			finalTime = SDL_GetTicks() - startTime;
		}
		else {
			createText(timerFont, { 0 , 255 , 0 }, getTime(finalTime), (cols * CELL_SIZE / 2) - 75, 0, 150, 50);
		}
		SDL_RenderCopy(gRenderer, bg2, &buttonBG, &buttonBG);
		createButton(newGameButton, newGameButtonColored, leftMouseDown, 0, rows * CELL_SIZE + 50, 100, 50, &newGame);
		createButton(quitButton, quitButtonColored, leftMouseDown, cols * CELL_SIZE - 100, rows * CELL_SIZE + 50, 100, 50, NULL);
		leftMouseDown = false;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				quitAndSave(&grid);
				break;
			case SDL_MOUSEBUTTONDOWN:
				leftMouseDown = true;
				if (e.button.x > cols * CELL_SIZE - 100 && e.button.x < cols * CELL_SIZE && e.button.y > rows * CELL_SIZE + 50 && e.button.y < rows * CELL_SIZE + 100) quitAndSave(&grid);
				if (animateTimes != -1) continue;
				if (not grid.checkLose() && not grid.checkWin()) {
					grid.handleMouseEvent(&e);
				}
				if (toAnimate.size() > 0) animateTimes = 0;
				break;
			}
		}

		//Blit texture to screen
		if (animateTimes == -1)	grid.drawGridContent();
		else {
			int idx = animateTimes / 2;
			int r, c;
			tie(r, c) = toAnimate[idx];
			grid.drawGridContent(r, c, idx);
			SDL_RenderCopy(gRenderer, grid.arr[grid.gridContent[r][c]], NULL, &grid.gridCoordinate[r][c]);
			if (animateTimes % 2 == 0) {
				SDL_Rect coordRect = grid.gridCoordinate[r][c];
				SDL_Rect fRect = { coordRect.x + 6,coordRect.y + 6,coordRect.w - 6,coordRect.h - 6 };
				SDL_RenderCopy(gRenderer, grid.img_unknown, NULL, &fRect);
			}
			animateTimes++;
			if (animateTimes == toAnimate.size() * 2) {
				animateTimes = -1;
				toAnimate = vector<pair<int, int>>();
			}
		}

		SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 255);
		SDL_GetMouseState(&mouseX, &mouseY);
		row = max(min((mouseY - 50) / CELL_SIZE, rows - 1), 0);
		col = max(min(mouseX / CELL_SIZE, cols - 1), 0);
		SDL_RenderDrawRect(gRenderer, &grid.gridCoordinate[row][col]);
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		if ((grid.checkLose() || grid.checkWin()) && animateTimes == -1) {
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 128);
			SDL_RenderFillRect(gRenderer, &BG);
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
			if (grid.checkLose()) {
				SDL_RenderCopy(gRenderer, losePrompt, NULL, &BG);
			}
			else {
				SDL_RenderCopy(gRenderer, winPrompt, NULL, &BG);
				if (!hasSave) {
					saveScore(SDL_GetTicks() - startTime, rows, cols);
					hasSave = true;
				}
			}
		}
		//Update display
		SDL_RenderPresent(gRenderer);
	}
	return;
}

void loadGame() {
	if (isExist("save.txt")) {
		isGameLoaded = true;
		gameLoop();
	}
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
	while (getline(fin,s)) {
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

void Leaderboard() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(gRenderer);
	window = SDL_CreateWindow("Leaderboard", 100, 100, 1000,615, SDL_WINDOW_SHOWN);
	gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_Rect rect;
	TTF_Font* boardFont = TTF_OpenFont("resources/Font/Lato-Light.ttf", 500);
	SDL_Texture* leaderboard = loadImgTexture("resources/Leaderboard/leaderboard.png");
	SDL_Texture* quitButton = loadImgTexture("resources/Large Buttons/Large Buttons/Quit.png");
	SDL_Texture* quitButtonColored = loadImgTexture("resources/Large Buttons/Colored Large Buttons/Quit.png");
	SDL_Texture* backButton = loadImgTexture("resources/Large Buttons/Large Buttons/Back.png");
	SDL_Texture* backButtonColored = loadImgTexture("resources/Large Buttons/Colored Large Buttons/Back.png");
	vector<tuple<string, string, Uint32, int, int>> sortedScore = sortScore();
	SDL_RenderClear(gRenderer);
	SDL_RenderCopy(gRenderer, leaderboard, NULL, createRect(&rect, 0, 0, 1000, 615));
	int lim = sortedScore.size() > 6 ? 6 : sortedScore.size();
	string d, t;
	Uint32 totalTime;
	int rows, cols;
	for (int i = 0; i < lim; i++) {
		tie(d, t, totalTime, rows, cols) = sortedScore[i];
		createText(boardFont, { 255,255,255 }, d+" "+t, 175, 230 + 56 * i, 200, 30);
		createText(boardFont, { 255,255,255 }, getTime(totalTime), 685, 230 + 56 * i, 50, 30);
		createText(boardFont, { 255,255,255 }, to_string(rows), 430, 230 + 56 * i, 15*to_string(rows).size(), 30);
		createText(boardFont, { 255,255,255 }, to_string(cols), 550, 230 + 56 * i, 15 * to_string(cols).size(), 30);
		createText(boardFont, { 255,255,255 }, doubleToString(static_cast<double>(rows*cols)/static_cast<double>(totalTime/1000)), 815, 230 + 56 * i, 50, 30);
		createText(boardFont, { 255,255,255 }, to_string(sortedScore.size()), 0,0, 30, 30);

	}
	SDL_RenderPresent(gRenderer);
	bool leftMouseDown = false;
	SDL_Event e;
	bool quit = false;
	while (!quit) {
		createButton(backButton, backButtonColored,leftMouseDown,0,560,200,40,&newGame);
		leftMouseDown = false;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				quitGame();
			case SDL_MOUSEBUTTONDOWN:
				leftMouseDown = true;
			}
		}
		SDL_RenderPresent(gRenderer);
	}
}
void startMenu() {
	init();
	SDL_Event e;
	SDL_Texture* bg = loadImgTexture("resources/BG/bg.png");
	SDL_Texture* startButton = loadImgTexture("resources/Large Buttons/Large Buttons/Start.png");
	SDL_Texture* startButtonColored = loadImgTexture("resources/Large Buttons/Colored Large Buttons/Start.png");
	SDL_Texture* exitButton = loadImgTexture("resources/Large Buttons/Large Buttons/Exit.png");
	SDL_Texture* exitButtonColored = loadImgTexture("resources/Large Buttons/Colored Large Buttons/Exit.png");
	SDL_Texture* loadButton = loadImgTexture("resources/Large Buttons/Large Buttons/Load.png");
	SDL_Texture* loadButtonColored = loadImgTexture("resources/Large Buttons/Colored Large Buttons/Load.png");
	SDL_Texture* leaderboard = loadImgTexture("resources/Square Buttons/Square Buttons/leaderboard.png");
	SDL_Texture* leaderboardColored = loadImgTexture("resources/Square Buttons/Colored Square Buttons/leaderboard.png");
	SDL_Rect rect;
	bool quit = false;
	while (!quit) {
		//Clear Screen and set to last color set in SDL_SetRenderDrawColor()
		SDL_RenderClear(gRenderer);
		SDL_RenderCopy(gRenderer, bg, NULL, createRect(&rect, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
		bool leftMouseDown = false;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				quitGame();
				break;
			case SDL_MOUSEBUTTONDOWN:
				leftMouseDown = e.button.button == SDL_BUTTON_LEFT;
				break;
			}
		}
		createButton(startButton, startButtonColored, leftMouseDown, 350, 325, 200, 50, &gameLoop);
		createButton(loadButton, loadButtonColored, leftMouseDown, 350, 425, 200, 50, &loadGame);
		createButton(exitButton, exitButtonColored, leftMouseDown, 350, 525, 200, 50, &quitGame);
		createButton(leaderboard, leaderboardColored, leftMouseDown, 410, 260, 75, 50, &Leaderboard);
		//Blit texture to screen
		//Update display
		SDL_RenderPresent(gRenderer);
	}
	return;
}


int main(int argc, char* args[]) {
	startMenu();
	return 0;
}