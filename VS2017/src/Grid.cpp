#include "Grid.h"

#include <vector>

using std::vector;

Grid::Grid()
{
	size = 7;
	makeGridX(size);
	makeGridZ(size);
}

Grid::Grid(int n)
{
	size = n;
	makeGridX(size);
	makeGridZ(size);
}

vector<double> Grid::getGridX()
{
	return gridX;
}

vector<double> Grid::getGridZ()
{
	return gridZ;
}

void Grid::makeGridX(int n)
{
	for (int i=0; i < n; i++)
	{
		for (int j=0; j<n; i++)
		{
			gridX.push_back((j-3.0));
		}
	}
}

void Grid::makeGridZ(int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; i++)
		{
			gridX.push_back((i - 3.0));
		}
	}
}