#pragma once

#include <stdlib.h>
//Parameters for batch test
const bool GENERATE_JOB_ON_START = true;
const bool PRINT_ITERATION_SOLUTION = false;
const bool PRINT_TABLE = true;

void dump_critical_error(const char* message)
{
	printf("[FATAL ERROR] %s\n", message);
	getchar();
	exit(0);
}

int indexof(int* array, int size, int value)
{
	for (int i = 0; i < size; i++)
	{
		if (array[i] == value)
			return i;
	}
	return -1;
}

int argmax(int* array, int size)
{
	int max_index = 0;
	for (int i = 1; i < size; i++)
	{
		if (array[i] > array[max_index])
			max_index = i;
	}
	return max_index;
}

int argmin(int* array, int size)
{
	int min_index = 0;
	for (int i = 1; i < size; i++)
	{
		if (array[min_index] > array[i])
			min_index = i;
	}
	return min_index;
}

double avg(int* array, int size)
{
	double sum = 0;
	for (int i = 0; i < size; i++)
		sum += array[i];
	return sum / size;
}

void draw_line(int line_width)
{
	while (line_width--)
		printf("-");
	printf("\n");
}

int u(int min, int max)
{
	return min == max ? min: min + (rand() % (max - min));
}

void print_array(int* array, int size)
{
    for(int i = 0; i < size; i ++)
        printf("%d ", array[i]);
    printf("\n");
}
