#include "Grid.h"

#include <vector>
#include <iostream>

using std::vector;
							//each individual block is assumed to be 1x1 unit

Grid::Grid()
{
	size = 7;
	makeGridX(size);
	makeGridZ(size);
}

Grid::Grid(int n)			//only use odd numbers for now
{							//even numbers will give off centred cross
	size = n;				//default is 7
	makeGridX(size);
	makeGridZ(size);
}

Grid::~Grid()
{
	gridX.clear();
	gridZ.clear();
}


std::vector<std::pair<int, int>> Grid::getBuildingPos()
{
	std::vector<std::pair<int, int>> result;
	int center = (int) (size / 2);

	for (int row = 0; row < size; row++)
	{
		for (int col = 0; col < size; col++)
		{
			if (row == center || col == center)
				continue;
			int index = row * size + col;
			result.push_back(std::pair<int, int>(gridX[index], gridZ[index]));
		}
	}

	return result;
}

std::vector<std::pair<int, int>> Grid::getRoadPos()
{
	std::vector<std::pair<int, int>> result;
	int center = (int)(size / 2);

	for (int row = 0; row < size; row++)
	{
		for (int col = 0; col < size; col++)
		{
			if (row == center || col == center)
			{
				int index = row * size + col;
				result.push_back(std::pair<int, int>(gridX[index], gridZ[index]));
			}
		}
	}

	return result;
}

void Grid::makeGridX(int n)
{
	int m = n / 2;
	float d = m * 1.0;
	
	for (int i=0; i < n; i++)
	{
		for (int j=0; j<n; j++)
		{
			gridX.push_back((j-d));
			//std::cout << (j - d) << std::endl;
		}
	}
}

void Grid::makeGridZ(int n)
{
	int m = n / 2;
	float d = m * 1.0;
	
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			gridZ.push_back((i - d));
			//std::cout << (i - d) << std::endl;
		}
	}
}