#pragma once

#include <vector>

using std::vector;

class Grid
{
public:
	Grid();
	Grid(int);
	~Grid();

	vector<float> getGridX();
	vector<float> getGridZ();


private:
	int size;
	vector<float> gridX;
	vector<float> gridZ;
	void makeGridX(int);
	void makeGridZ(int);
};