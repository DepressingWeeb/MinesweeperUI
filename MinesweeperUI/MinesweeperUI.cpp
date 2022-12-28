#define NOMINMAX

#include <Windows.h>

#include "Global.h"

#include "Utils.h"

#include "Class.h"

#include "FileIO.h"

#include "MinesweeperUI.h"

#include <tuple>
using namespace std;

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
	window = SDL_CreateWindow("Minesweeper", 100, 100, cols * CELL_SIZE, rows * CELL_SIZE + 100, SDL_WINDOW_SHOWN);
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
		SDL_RenderPresent(gRenderer);
	}
	return;
}
