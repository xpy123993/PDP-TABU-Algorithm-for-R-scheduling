#pragma once

#include "job.h"
#include <string.h>

/*
Solution: data-structure for a solution to the problem

value[i][j], the j-th processing job on machine i

*/

struct Solution
{
public:
	int value[ALLOCATE_MAX_M][ALLOCATE_MAX_N];
	Solution()
	{
		for (int i = 0; i < ALLOCATE_MAX_M; i++)
			for (int j = 0; j < ALLOCATE_MAX_N; j++)
				value[i][j] = VALUE_DEFAULT;
	}

	bool equals(const Solution& solution)
	{
		for (int i = 0; i < current_config.M; i++)
		{
			if (memcmp(value[i], solution.value[i], current_config.N * sizeof(int)) != 0)
				return false;
		}
		return true;
	}
};

int evaluate(const Solution& solution)
{
	int proc_time[ALLOCATE_MAX_M] = { 0 };
	int latework = 0;

	int job_detected_count = 0;

	for (int i = 0; i < current_config.M; i++)
	{
		for (int j = 0; j < current_config.N; j++)
		{
			if (~solution.value[i][j])
			{
				Job& current_job = test_data[solution.value[i][j]];
				proc_time[i] += current_job.proc_time[i];
				latework += get_lateness(solution.value[i][j], i, proc_time[i]);
				job_detected_count++;
			}
		}
	}

	if (job_detected_count != current_config.N)
	{
		printf("ERROR: Job data lost, current number: %d (%d)\n", job_detected_count, current_config.N);
	}

	return latework;
}

bool check_solution(const Solution& solution)
{
	int jobs[ALLOCATE_MAX_N];
	int count = 0;
	for (int i = 0; i < current_config.N; i++)
	{
		jobs[i] = 0;
	}
	for (int i = 0; i < current_config.M; i++)
	{
		for (int j = 0; j < current_config.N; j++)
		{
			if (solution.value[i][j] != VALUE_DEFAULT)
			{
				count++;
				jobs[solution.value[i][j]] ++;
				if (jobs[solution.value[i][j]] > 1) return false;
			}
		}
	}

	return count == current_config.N;
}

void print_solution(const Solution& solution, const char* tagName = NULL)
{
	draw_line(10);
	if (tagName != NULL)
	{
		printf("Solution: %s\n", tagName);
	}
	int proc_time[ALLOCATE_MAX_M] = { 0 };
	for (int i = 0; i < current_config.M; i++)
	{
		for (int j = 0; j < current_config.N; j++)
		{
			if (solution.value[i][j] != VALUE_DEFAULT)
				proc_time[i] += test_data[solution.value[i][j]].proc_time[i];
		}
	}
	for (int i = 0; i < current_config.M; i++)
	{
		printf("M%d (Process Time = %d): ", i, proc_time[i]);
		for (int j = 0; j < current_config.N; j++)
		{
			if (solution.value[i][j] != VALUE_DEFAULT)
			{
				Job &current_job = test_data[solution.value[i][j]];
				printf("%d ", solution.value[i][j]);
			}
		}
		printf("\n");

	}

	printf("Value = %d\n", evaluate(solution));
	draw_line(10);
}
