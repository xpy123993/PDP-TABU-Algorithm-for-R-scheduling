#pragma once

#include "listalgo.h"

//Parameters for pso
const double MUTATION_PROBABILITY = 0.02;
const double SELECTION_PROBABILITY = 0.5;
const double SAVE_PROBABILITY = 0.40;

const int POPULATION_SIZE = 50;
const int PSO_ITERATION_MAXN = 10000;
const int PSO_UNCHANGED_ITERATION_MAXN = 2000;
const bool PSO_UNCHANGED_ITERATION_MODE = true;

/*
2017-3-15
粒子群算法

Struct Pack:
编码存储结构

pack(): 将机器号、任务在机器内序列编码为int整数
unpack_machine_id(): 从包内解压出机器号
unpack_sequence_id(): 从包内解压出序列号

粒子群公式
v = r0 * v0 + r1 * (g - x) + r2 * (p - x)
x = x + v
*/

class PSOOptimizer
{
	// Strategy: assign jobs to machine, no order
	class Pack
	{
		int data[ALLOCATE_MAX_N];

	public:

		Pack() {}
		Pack(const Solution& solution)
		{
			initialize_from_solution(solution);
		}

		int pack_data(int machine_id, int order_in_sequence)
		{
			return machine_id * ALLOCATE_MAX_N + order_in_sequence;
		}

		int unpack_data_machine_id(int data) const
		{
			return data / ALLOCATE_MAX_N;
		}

		int unpack_data_order(int data) const
		{
			return data % ALLOCATE_MAX_N;
		}

		Solution get_solution() const
		{
			Solution result;

			for (int i = 0; i < current_config.N; i++)
				result.value[unpack_data_machine_id(data[i])][unpack_data_order(data[i])] = i;

			return result;
		}

		int get_fitness_value()
		{
			return evaluate(get_solution());
		}

		void initialize_as_sequence()
		{


			for (int i = 0; i < current_config.N; i++)
			{
				data[i] = pack_data(u(0, current_config.M), u(0, current_config.N));
				if (indexof(data, i, data[i]) != -1)
					i--;
			}



			//print_solution(get_solution());
			//printf("value = %d\n", get_fitness_value());

		}

		void initialize_as_binary()
		{
			for (int i = 0; i < current_config.N; i++)
			{
				data[i] = u(0, 100) < (SAVE_PROBABILITY * 100) ? 1 : 0;
			}
		}

		friend Pack operator+(const Pack& l, const Pack& r)
		{
			int index_1, index_2;
			Pack result;

			for (int i = 0; i < current_config.N; i++)
			{
				result.data[i] = l.data[i];
			}
			for (int i = 0; i < current_config.N; i++)
			{
				if (u(0, 100) < SELECTION_PROBABILITY * 100)
				{

					if (r.data[i] != VALUE_DEFAULT)
					{
						index_1 = indexof(result.data, current_config.N, r.data[i]);

						if (index_1 != -1)
						{
							index_2 = i;
							//if(index_1 == index_2) printf("...\n");
							swap(result.data[index_1], result.data[index_2]);
						}
						else
						{
							result.data[i] = r.data[i];
						}
					}
				}
			}
			return result;
		}

		friend Pack operator-(const Pack& l, const Pack& r)
		{
			Pack result;
			for (int i = 0; i < current_config.N; i++)
			{
				if (l.data[i] == r.data[i])
					result.data[i] = VALUE_DEFAULT;
				else
					result.data[i] = l.data[i];
			}
			return result;
		}

		friend Pack operator*(const Pack& l, const Pack& r)
		{
			Pack result;
			for (int i = 0; i < current_config.N; i++)
			{
				if (l.data[i] == 1)
					result.data[i] = r.data[i];
				else
					result.data[i] = VALUE_DEFAULT;
			}
			return result;
		}

		Pack& operator=(const Pack& r)
		{
			for (int i = 0; i < current_config.N; i++)
				data[i] = r.data[i];
			return *this;
		}

		void initialize_from_solution(const Solution& solution)
		{
			for (int i = 0; i < current_config.M; i++)
			{
				for (int j = 0; j < current_config.N; j++)
				{
					if (solution.value[i][j] != VALUE_DEFAULT)
					{
						data[solution.value[i][j]] = pack_data(i, j);
					}
				}
			}
		}

		void mutation()
		{

			for (int i = 0; i < current_config.N; i++)
			{
				if (u(0, 100) < MUTATION_PROBABILITY * 100)
				{
					int new_position = pack_data(u(0, current_config.M), u(0, current_config.N));
					int index = indexof(data, current_config.N, new_position);
					if (index == -1)
					{
						// new position is empty
						data[i] = new_position;
					}
					else
					{
						data[index] = data[i];
						data[i] = new_position;
						//printf("found\n");
					}
				}
			}

			//check_solution(get_solution());
		}
	};

	class Particle
	{
		Pack velocity;
		Pack position;

		Pack local_best;
		int local_best_value;

	public:

		void onPositionChanged(Pack& global_best, int& global_best_value)
		{
			int value = position.get_fitness_value();
			if (value < local_best_value)
			{
				local_best = position;
				local_best_value = value;

				if (value < global_best_value)
				{
					global_best = position;
					global_best_value = value;
				}
			}
		}

		void setPosition(Pack& new_position, Pack& global_best, int& global_best_value)
		{
			position = new_position;
			int value = position.get_fitness_value();

			onPositionChanged(global_best, global_best_value);
		}
		void update(Pack& global_best, int& global_best_value)
		{
			Pack r0, r1, r2;

			r0.initialize_as_binary();
			r1.initialize_as_binary();
			r2.initialize_as_binary();

			velocity = r0 * velocity + r1 * (local_best - position) + r2 * (global_best - position);
			velocity.mutation();

			position = position + velocity;

			onPositionChanged(global_best, global_best_value);

		}

		void initialize()
		{
			velocity.initialize_as_sequence();
			position.initialize_as_sequence();
			local_best.initialize_as_sequence();
			local_best_value = local_best.get_fitness_value();
		}
	};

	Pack global_best;
	Particle* particles;

	int global_best_value;
	int initial_spec_solution_count;

	void add_list_solution(const Solution& solution)
	{
		Pack pack(solution);
		particles[initial_spec_solution_count++].setPosition(pack, global_best, global_best_value);
	}

	void initialize()
	{
		for (int i = 0; i < POPULATION_SIZE; i++)
		{
			particles[i].initialize();
		}

		global_best.initialize_as_sequence();
		global_best_value = global_best.get_fitness_value();
		initial_spec_solution_count = 0;

		// add some list solution
		ListOptimizer listOptimizer;
		add_list_solution(listOptimizer.get_minc_solution());
		//printf("minc_added\n");
		add_list_solution(listOptimizer.get_miny_solution());
		//printf("miny_added\n");
		add_list_solution(listOptimizer.get_lpt_edd_solution());
		//printf("lpt_edd_added\n");
		add_list_solution(listOptimizer.get_spt_edd_solution());
		//printf("spt_edd_added\n");
		//add_list_solution(listOptimizer.get_min_makespan_first_solution());
		//add_list_solution(listOptimizer.get_min_proc_time_first_solution());

		tick_count = 0;
	}

	// 2017-5-3
	// 增加统计算法时间

	int tick_count;

public:

	PSOOptimizer()
	{
		particles = new Particle[POPULATION_SIZE];
	}

	~PSOOptimizer()
	{
		delete[] particles;
	}

	// 2017-5-3
	// 增加统计算法时间

	double get_running_time()
	{
		return tick_count / 1000.0;
	}

	Solution minimize()
	{
		initialize();

		int last_best_value = -1;
		int unchanged_iteration = 0, iteration = 0;

		// 2017-4-26
		// print data for graph
		char filename[256];
		sprintf(filename, "../Data/Pso%d-%d.txt", current_config.M, current_config.N);
		FILE *fp = NULL;
		if (PRINT_ITERATION_SOLUTION)
			fp = fopen(filename, "w");
		tick_count = GetTickCount();
		while (iteration < PSO_ITERATION_MAXN)
		{
			for (int i = 0; i < POPULATION_SIZE; i++)
			{
				particles[i].update(global_best, global_best_value);
			}
			if (PSO_UNCHANGED_ITERATION_MODE)
			{
				if (last_best_value != global_best_value)
				{
					last_best_value = global_best_value;
					//printf("new solution found, current value: %d\n", global_best_value);
					unchanged_iteration = 0;
				}
				else
				{
					unchanged_iteration++;
					if (unchanged_iteration > PSO_UNCHANGED_ITERATION_MAXN)
						break;
				}
			}
			iteration++;

			// 2017-4-26
			// print data for graph
			if (PRINT_ITERATION_SOLUTION)
				fprintf(fp, "%d\n", global_best_value);
		}

		tick_count = GetTickCount() - tick_count;

		if (PRINT_ITERATION_SOLUTION)
			fclose(fp);

		Solution current_solution = global_best.get_solution();

		if (!check_solution(current_solution))
		{
			printf("Warning: Unusual Solution found\n");
			print_solution(current_solution);
		}
		return current_solution;
	}

};
