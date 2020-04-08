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

vector<float> Grid::getGridX()
{
	return gridX;
}

vector<float> Grid::getGridZ()
{
	return gridZ;
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
			std::cout << (j - d) << std::endl;
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
			std::cout << (i - d) << std::endl;
		}
	}
}