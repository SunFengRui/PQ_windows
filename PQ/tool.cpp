#include "tool.h"

//快速排序算法 
int compar(const void *a, const void *b)
{
	return *(double*)a > *(double*)b ? 1 : -1;    //升序排列
}
void color(short x) //自定义函根据参数改变颜色   
{
	if (x >= 0 && x <= 15)//参数在0-15的范围颜色  
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);    //只有一个参数，改变字体颜色   
	else//默认的颜色白色  
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}
#define AN_BUFFER_400kLEN	(400*1000)
#define NZEROS 6
#define NPOLES 6
#define GAIN   7.070101825e+10
static double xv[NZEROS + 1], yv[NPOLES + 1];
static void filterloop(short *next_input_value, double *next_output_value)
{
	int i;
	for (i = 0; i < AN_BUFFER_400kLEN; i++)
	{
		xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6];
		xv[6] = next_input_value[i] / GAIN;
		yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6];
		yv[6] = (xv[0] + xv[6]) + 6 * (xv[1] + xv[5]) + 15 * (xv[2] + xv[4])
			+ 20 * xv[3]
			+ (-0.8856901133 * yv[0]) + (5.4216464901 * yv[1])
			+ (-13.8294247400 * yv[2]) + (18.8152897330 * yv[3])
			+ (-14.4004405300 * yv[4]) + (5.8786191597 * yv[5]);
		next_output_value[i] = yv[6];
	}
}
unsigned long minValue(unsigned long a, unsigned long b, unsigned long c)
{
	if (a < b)
	{
		if (a < c)
			return a;
		else
			return c;
	}
	else
	{
		if (b < c)
			return b;
		else
			return c;
	}
}
unsigned long maxValue(unsigned long a, unsigned long b, unsigned long c)
{
	if (a < b)
	{
		if (b < c)
			return c;
		else
			return b;
	}
	else
	{
		if (a < c)
			return c;
		else
			return a;
	}
}
double find_max(double *a, int size)
{
	double temp;
	temp = a[0];
	for (int i = 1; i < size; i++)
	{
		if (temp < a[i])
			temp = a[i];
	}
	return temp;

}
double find_min(double *a, int size)
{
	double temp;
	temp = a[0];
	for (int i = 1; i < size; i++)
	{
		if (temp > a[i])
			temp = a[i];
	}
	return temp;

}
void bubble_sort(double *a, int size)
{
	double temp;
	char flag = 1;
	for (int i = 0; (i < size) && (flag); i++)
	{
		flag = 0;
		for (int j = 0; j < (size - i - 1); j++)
		{
			if (a[j] > a[j + 1])
			{
				temp = a[j];
				a[j] = a[j + 1];
				a[j + 1] = temp;
				flag = 1;
			}

		}
	}
}
double average(double *a, int size)
{
	double result;
	bubble_sort(a, size);
	result = (a[size / 2] + a[size / 2 - 1] + a[size / 2 - 2] + a[size / 2 + 1]) / 4;
	return result;
}