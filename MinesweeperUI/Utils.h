#pragma once
#define NOMINMAX

#include <SDL.h>

#include <SDL_image.h>

#include <SDL_ttf.h>

#include <string>

#include "Class.h"


using namespace std;

void init();
bool isExist(const string& name);
SDL_Rect* createRect(SDL_Rect* rect, int x, int y, int w, int h);
SDL_Texture* loadImgTexture(string path);
void quitGame();
void quitAndSave(Grid* grid);
void createButton(SDL_Texture* button, SDL_Texture* buttonOnHover, bool leftMouseDown, int x, int y, int w, int h, void(*func)());
void createText(TTF_Font* font, SDL_Color color, string text, int x, int y, int w, int h);
bool isAllDigit(string s);
void filterText(string* text1, string* text2, string* text3, int h, int v);
string getTime(Uint32 timeInMili);
bool cmpScore(tuple<string, string, Uint32, int, int> t1, tuple<string, string, Uint32, int, int> t2);
vector<tuple<string, string, Uint32, int, int>> sortScore();
string doubleToString(double d);
