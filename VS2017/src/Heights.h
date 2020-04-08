#pragma once


#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <vector>

float map(float input, int start, int end, int newStart, int newEnd)
{
	float output = newStart + ((newEnd - newStart) / (end - start)) * (input - start);
	return output;
}


float randomizeHeight(int startRange, int endRange)
{
	float result = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	return map(result, 0, 1, startRange, endRange);
}

