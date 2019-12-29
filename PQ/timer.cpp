#include "timer.h"

LARGE_INTEGER iFreq;
LARGE_INTEGER iBegTime;
double CurrentTime()
{
	double result_time;
	QueryPerformanceFrequency(&iFreq);
	QueryPerformanceCounter(&iBegTime);
	result_time = (double)iBegTime.QuadPart / iFreq.QuadPart;
	return result_time;
}