#include "utils.h"
#include "job.h"
#include "solution.h"

#pragma once

#include <algorithm>
using namespace std;


class ListOptimizer
{
public:
	Solution get_min_makespan_first_solution()
	{
		Solution solution;
		int proc_time[ALLOCATE_MAX_M] = { 0 };
		int proc_count[ALLOCATE_MAX_M] = { 0 };
		int selected_index = 0;
		for (int i = 0; i < current_config.N; i++)
		{
			selected_index = argmin(proc_time, current_config.M);
			proc_time[selected_index] += test_data[i].proc_time[selected_index];
			solution.value[selected_index][proc_count[selected_index]++] = i;
		}
		return solution;
	}
	Solution get_min_proc_time_first_solution()
	{
		Solution solution;
		int proc_count[ALLOCATE_MAX_M] = { 0 };
		int selected_index = 0;
		for (int i = 0; i < current_config.N; i++)
		{
			selected_index = argmin(test_data[i].proc_time, current_config.M);
			solution.value[selected_index][proc_count[selected_index]++] = i;
		}
		return solution;
	}
	Solution get_minc_solution()
	{
		Solution solution;
		int proc_time[ALLOCATE_MAX_M] = { 0 };
		int proc_count[ALLOCATE_MAX_M] = { 0 };
		int selected_index = 0;
		for (int i = 0; i < current_config.N; i++)
		{
			selected_index = argmin(proc_time, current_config.M);
			proc_time[selected_index] += test_data[i].proc_time[selected_index];
			solution.value[selected_index][proc_count[selected_index]++] = i;
		}
		if (!check_solution(solution))
		{
			printf("Warning: Unusual Solution found\n");
			print_solution(solution);
		}
		return solution;
	}
	Solution get_miny_solution()
	{
		Solution solution;
		int proc_time[ALLOCATE_MAX_M] = { 0 };
		int proc_count[ALLOCATE_MAX_M] = { 0 };
		int proc_late[ALLOCATE_MAX_M] = { 0 };
		int selected_index = 0;
		for (int i = 0; i < current_config.N; i++)
		{
			selected_index = argmin(proc_late, current_config.M);
			proc_time[selected_index] += test_data[i].proc_time[selected_index];
			proc_late[selected_index] += get_lateness(i, selected_index, proc_time[selected_index]);
			solution.value[selected_index][proc_count[selected_index]++] = i;
		}
		if (!check_solution(solution))
		{
			printf("Warning: Unusual Solution found\n");
			print_solution(solution);
		}
		return solution;
	}
	Solution get_spt_edd_solution()
	{
		Solution solution;
		int proc_count[ALLOCATE_MAX_M] = { 0 };
		int selected_index;
		for (int i = 0; i < current_config.N; i++)
		{
			selected_index = argmin(test_data[i].proc_time, current_config.M);
			solution.value[selected_index][proc_count[selected_index]++] = i;
		}
		for (int i = 0; i < current_config.M; i++)
		{
			if (proc_count[i] > 0)
				sort(solution.value[i], solution.value[i] + proc_count[i], cmp_edd);
		}
		if (!check_solution(solution))
		{
			printf("Warning: Unusual Solution found\n");
			print_solution(solution);
		}
		return solution;
	}
	Solution get_lpt_edd_solution()
	{
		Solution solution;
		int proc_count[ALLOCATE_MAX_M] = { 0 };
		int selected_index;
		for (int i = 0; i < current_config.N; i++)
		{
			selected_index = argmax(test_data[i].proc_time, current_config.M);
			solution.value[selected_index][proc_count[selected_index]++] = i;
		}
		for (int i = 0; i < current_config.M; i++)
		{
			if (proc_count[i] > 0)
				sort(solution.value[i], solution.value[i] + proc_count[i], cmp_edd);
		}
		if (!check_solution(solution))
		{
			printf("Warning: Unusual Solution found\n");
			print_solution(solution);
		}
		return solution;
	}
	Solution get_random_solution()
	{
		Solution solution;
		int m, s;
		for (int i = 0; i < current_config.N; i++)
		{
			m = u(0, current_config.M);
			s = u(0, current_config.N);
			if (solution.value[m][s] == VALUE_DEFAULT)
				solution.value[m][s] = i;
			else
				i--;
		}
		if (!check_solution(solution))
		{
			printf("Warning: Unusual Solution found\n");
			print_solution(solution);
		}
		return solution;
	}
};
