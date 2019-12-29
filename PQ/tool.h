#pragma once
#include "pcap.h"
int compar(const void *a, const void *b);
void color(short x);
unsigned long minValue(unsigned long a, unsigned long b, unsigned long c);
unsigned long maxValue(unsigned long a, unsigned long b, unsigned long c);
double average(double *a, int size);
void filterloop(short *next_input_value, double *next_output_value);