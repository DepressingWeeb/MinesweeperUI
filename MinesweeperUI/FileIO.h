#pragma once
#include "Class.h"
#include <fstream>
using namespace std;
void saveGame(Grid* grid);
void saveScore(Uint32 timeInMili, int rows, int cols);
Grid loadGameSave(int rows, int cols, int bombNumber, ifstream& fin);

