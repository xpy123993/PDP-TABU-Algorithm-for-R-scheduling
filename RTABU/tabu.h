#pragma once
#include <algorithm>
#include <windows.h>
#include "listalgo.h"
using namespace std;

//Parameter for tabu search
const int TABU_LIST_LENGTH = 100;
const int TABU_ITERATION_MAXN = 100;
const int TABU_UNCHANGED_ITERATION_MAXN = 50;
const bool TABU_UNCHANGED_ITERATION_MODE = true;
const int TABU_RANDOM_SELECTION_ITERATION = 20;

const double TABU_ACCEPT_PROBABILITY = 0.02;

/*

2017-4-11
禁忌搜索算法

find_next_solution_random_method()
策略1:全排列搜索策略

find_next_solution_combination_method()
策略2:随机行走搜索策略

*/
class TabuOptimizer
{
	int recur;
	int tabu_pointer_1, tabu_pointer_2;

	bool random_accept;

	// 2017-5-3
	// 增加统计算法时间
	int tick_count;

	bool terminated;
	// 2017-5-1
	// 添加短期记忆、中期记忆表
	Solution* tabu_list_1;
	Solution* tabu_list_2;

	int init_cost;

	bool solution_exists(const Solution& solution)
	{
		for (int i = 0; i < TABU_LIST_LENGTH; i++)
		{
			if (tabu_list_1[i].equals(solution))
				return true;
			if (tabu_list_2[i].equals(solution))
				return true;
		}
		return false;
	}

	void record_short_term_memory(const Solution& solution)
	{
		tabu_list_1[tabu_pointer_1++] = solution;
		if (tabu_pointer_1 >= TABU_LIST_LENGTH)
			tabu_pointer_1 = 0;
	}

	void record_medium_term_memory(const Solution& solution)
	{
		tabu_list_2[tabu_pointer_2++] = solution;
		if (tabu_pointer_2 >= TABU_LIST_LENGTH)
			tabu_pointer_2 = 0;
	}

	Solution find_neighbor_solution(const Solution& source)
	{
		Solution current = source;
		int m0 = u(0, current_config.M), m1 = u(0, current_config.M);
		int s0 = u(0, current_config.N), s1 = u(0, current_config.N);
		swap(current.value[m0][s0], current.value[m1][s1]);
		return current;
	}

	Solution find_next_solution_random_method(const Solution& source)
	{
		Solution current_solution = source;
		int current_solution_value = evaluate(current_solution), temp_value;
		bool updated = false;

		for (int i = 0; i < TABU_RANDOM_SELECTION_ITERATION; i++)
		{
			Solution next_solution = find_neighbor_solution(source);
			temp_value = evaluate(next_solution);

			// 2017-5-3
			// change search method

			if ((!solution_exists(next_solution) || (random_accept && u(1, 100) < 100 * TABU_ACCEPT_PROBABILITY)) && (temp_value < current_solution_value) || !updated)
			{
				record_short_term_memory(next_solution);
				current_solution_value = temp_value;
				current_solution = next_solution;
				updated = true;
			}
		}

		return current_solution;
	}

	Solution find_next_solution_combination_method(const Solution& source)
	{

		Solution local_best = source;
		Solution current = source;

		int current_value = evaluate(source);
		int local_best_value = current_value;
		bool updated = false;

		for (int i = 0; i < current_config.M; i++)
		{
			for (int i0 = 1; i0 < current_config.N; i0++)
			{
				for (int i1 = 0; i1 < i0; i1++)
				{
					if (source.value[i][i0] != VALUE_DEFAULT || source.value[i][i1] != VALUE_DEFAULT)
					{
						current = source;
						swap(current.value[i][i0], current.value[i][i1]);
						current_value = evaluate(current);

						if (current_value < local_best_value && !solution_exists(current))
						{
							local_best_value = current_value;
							local_best = current;
							updated = true;
						}
					}
				}
			}
		}

		for (int i = 0; i < current_config.N; i++)
		{
			for (int i0 = 1; i0 < current_config.M; i0++)
			{
				for (int i1 = 0; i1 < i0; i1++)
				{
					if (source.value[i0][i] != VALUE_DEFAULT || source.value[i1][i] != VALUE_DEFAULT)
					{
						current = source;
						swap(current.value[i0][i], current.value[i1][i]);
						current_value = evaluate(current);
						if (current_value < local_best_value && !solution_exists(current))
						{
							local_best_value = current_value;
							local_best = current;
							updated = true;
						}
					}
				}
			}
		}

		//terminated = !updated;

		return local_best;
	}

	void initialize()
	{
		tabu_pointer_1 = tabu_pointer_2 = 0;
		terminated = false;
		tick_count = 0;
	}

public:

	TabuOptimizer(bool random_accept): random_accept(random_accept)
	{
		tabu_list_1 = new Solution[TABU_LIST_LENGTH];
		tabu_list_2 = new Solution[TABU_LIST_LENGTH];
	}

	~TabuOptimizer()
	{
		delete[] tabu_list_1;
		delete[] tabu_list_2;
	}

	Solution minimize()
	{
		initialize();
		ListOptimizer init_optimizer;


		Solution current_solution =
			init_optimizer.get_minc_solution();

		Solution best_solution = current_solution;

		init_cost = evaluate(best_solution);

		tick_count = GetTickCount();
		int unchanged_iteration = 0, iteration = 0;
		int last_solution_value = 0, current_value = evaluate(current_solution), best_value = evaluate(best_solution);

		// 2017-4-26
		// print data for graph

		char filename[256];
		sprintf(filename, "../Data/Tabu%d-%d.txt", current_config.M, current_config.N);
		FILE *fp = NULL;
		if (PRINT_ITERATION_SOLUTION)
			fp = fopen(filename, "w");

		while (iteration < TABU_ITERATION_MAXN)
		{
			// 2017-5-3
			// change strategy of tabu search
			// record_solution(current_solution);
			current_solution = find_next_solution_random_method(current_solution);
			current_value = evaluate(current_solution);

			if (best_value > current_value)
			{
				record_medium_term_memory(current_solution);
				best_value = current_value;
				best_solution = current_solution;
			}

			if (TABU_UNCHANGED_ITERATION_MODE)
			{
				if (current_value != last_solution_value)
				{
					last_solution_value = current_value;
					unchanged_iteration = 0;
				}
				else
				{
					unchanged_iteration++;
					if (unchanged_iteration > TABU_UNCHANGED_ITERATION_MAXN)
						break;
				}
			}
			iteration++;

			if (terminated) break;

			// 2017-4-26
			// print data for graph
			if (PRINT_ITERATION_SOLUTION)
				fprintf(fp, "%d\n", current_value);
		}

		tick_count = GetTickCount() - tick_count;

		if (PRINT_ITERATION_SOLUTION)
			fclose(fp);

		if (!check_solution(current_solution))
		{
			printf("Warning: Unusual Solution found\n");
			print_solution(current_solution);
		}
		return best_solution;
	}
	// 2017-5-3
	// 增加统计算法时间
	double get_running_time()
	{
		return tick_count / 1000.0;
	}

	int get_inital_cost()
	{
		return init_cost;
	}
};
