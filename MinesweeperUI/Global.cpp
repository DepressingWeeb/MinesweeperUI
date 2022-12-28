#include "Global.h"
extern const int SCREEN_WIDTH = 640;
extern const int SCREEN_HEIGHT = 640;
extern const int CELL_SIZE = 25;

//init global variables
extern SDL_Window* window = NULL;
extern SDL_Renderer* gRenderer = NULL;
extern bool isGameLoaded = false;
vector<pair<int, int>> toAnimate;