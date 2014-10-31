/** math.cpp **/

#include <cstdlib>

float linear_function(const float x1, const float x2, const float y1, const float y2, const float x) {
	return (y1 - y2) / (x1 - x2) * (x - x1) + y1;
}

float randomf(const float bottom, const float top) {
	return (float)rand() / (float)RAND_MAX * (top - bottom) + bottom;
}

float clampf(const float value, const float bottom, const float top) {
	if(value > top) return top;
	else if(value < bottom) return bottom;
	else return value;
}
