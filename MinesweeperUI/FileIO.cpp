#include "FileIO.h"
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
	outfile << tnow << " ";
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