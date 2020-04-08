#pragma once

#include <vector>

//using std::vector;

class Grid
{
public:
	Grid();
	Grid(int);
	~Grid();

	inline std::vector<float> getGridX() const { return gridX; }
	inline std::vector<float> getGridZ() const { return gridZ; }

	std::vector<std::pair<int, int>> getBuildingPos();
	std::vector<std::pair<int, int>> getRoadPos();

private:
	int size;
	std::vector<float> gridX;
	std::vector<float> gridZ;
	void makeGridX(int);
	void makeGridZ(int);
};