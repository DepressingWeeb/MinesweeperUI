#pragma once

#include <SDL.h>

#include <SDL_image.h>

#include <SDL_ttf.h>

#include <vector>
using namespace std;
//Screen dimension constants
extern const int SCREEN_WIDTH ;
extern const int SCREEN_HEIGHT ;
extern const int CELL_SIZE ;

//init global variables
extern SDL_Window* window ;
extern SDL_Renderer* gRenderer;
extern bool isGameLoaded ;
extern vector<pair<int, int>> toAnimate;

