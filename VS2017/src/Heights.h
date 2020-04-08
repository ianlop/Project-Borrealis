#pragma once


#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <vector>

float map(float input, float start, float end, float newStart, float newEnd)
{
	float output = newStart + ((newEnd - newStart) / (end - start)) * (input - start);
	return output;
}


float randomizeHeight(float startRange, float endRange)
{
	float result = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	return map(result, 0, 1, startRange, endRange);
}

