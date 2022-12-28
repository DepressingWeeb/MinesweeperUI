#pragma once

#include <SDL.h>

#include <SDL_image.h>

#include <SDL_ttf.h>

#include <string>
using namespace std;

void drawDifficulty(string* text1, string* text2, string* text3, TTF_Font* diffFont);
tuple < int, int, int > gridSetting();
void newGame();
void gameLoop();
void loadGame();
void Leaderboard();
void startMenu();
