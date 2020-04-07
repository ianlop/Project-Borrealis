#pragma once

#include <vector>

using std::vector;

class Grid
{
public:
	Grid();
	Grid(int);
	~Grid();

	vector<double> getGridX();
	vector<double> getGridZ();


private:
	int size;
	vector<double> gridX;
	vector<double> gridZ;
	void makeGridX(int);
	void makeGridZ(int);
};