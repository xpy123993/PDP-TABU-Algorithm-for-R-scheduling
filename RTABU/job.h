#pragma once
#define VALUE_DEFAULT -1

#include "utils.h"
#include <algorithm>
using namespace std;

const int ALLOCATE_MAX_M = 50;
const int ALLOCATE_MAX_N = 350;

const int JOB_PROCTIME_MIN = 1;
const int JOB_PROCTIME_MAX = 10;


/*
Job:
proc_time i: processing time when the job being proceed on machine i


test_data is a global variable, stores the current solving problem
*/

struct Job
{
	int proc_time[ALLOCATE_MAX_M];
	int due_date;
} test_data[ALLOCATE_MAX_N];

struct TaskInfo
{
    int M, N;
    double b;
    TaskInfo(){M = N = 10; b = 3;}
} current_config;

int cmp_edd(int index_l, int index_r)
{
	return test_data[index_l].due_date < test_data[index_r].due_date;
}

int cmp_avg_proc(const int index_l, const int index_r)
{
	int sum = 0;
	for (int i = 0; i < current_config.M; i++)
		sum += test_data[index_l].proc_time[i] - test_data[index_r].proc_time[i];
	return sum < 0;
}

void print_job_info(Job& j)
{
	draw_line(10);
	printf("Array[] Process Time:\n");
	for (int i = 0; i < current_config.M; i++)
	{
		printf("%d ", j.proc_time[i]);
	}
	printf("\n");
	printf("Due Date: %d\n", j.due_date);
	draw_line(10);
	printf("\n");
}

int get_lateness(int job_index, int machine_index, int completion_time)
{
	return min(test_data[job_index].proc_time[machine_index], max(0, completion_time - test_data[job_index].due_date));
}
