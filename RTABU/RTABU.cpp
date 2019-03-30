#include <time.h>
#include <stdio.h>
#include "pch.h"

// basic data structures and utility functions
#include "job.h"
#include "solution.h"
#include "utils.h"

// algorithms
#include "tabu.h"
#include "listalgo.h"
#include "pso.h"
#include "pdp.h"

// data generation methods
#include "generatetest.h"

using namespace std;
const int NUM_MAXN = 1000007;

const int TASK_PSO = 1, TASK_LPT_EDD = 2, TASK_SPT_EDD = 4, TASK_MINC = 8, TASK_MINY = 16, TASK_TABU = 32, TASK_PDP = 64, TASK_TABU_RAND = 128;

ListOptimizer listOptimizer;

void write_record_header(FILE *fp)
{
	fprintf(fp, "M, N, B, ALGO_NAME, RESULT, RUNNING_TIME\n");
}

void insert_algorithm_record(FILE *fp, const char* algo_name, double result, double running_time)
{
	fprintf(fp, "%d, %d, %f, %s, %f, %f\n", current_config.M, current_config.N, current_config.b, algo_name, result, running_time);
	// flush immediately to avoid data loss due to program crash
	fflush(fp);

	printf("[%s] result = %.2f, running_time = %.4f\n", algo_name, result, running_time);
}

double negative_improve_rate(double base, double value)
{
	return -100.0 * (value - base) / base;
}

void run_algorithm(FILE* fp, int tasks)
{
	double result, running_time;
	double pdp_result;

	printf("Solving case: n = %d, m = %d, b = %.3f\n", current_config.N, current_config.M, current_config.b);
	if (tasks & TASK_LPT_EDD)
	{
		result = evaluate(listOptimizer.get_lpt_edd_solution());
		insert_algorithm_record(fp, "LPT_EDD", result, 0);
	}
	if (tasks & TASK_SPT_EDD)
	{
		result = evaluate(listOptimizer.get_spt_edd_solution());
		insert_algorithm_record(fp, "SPT_EDD", result, 0);
	}
	if (tasks & TASK_MINC)
	{
		result = evaluate(listOptimizer.get_minc_solution());
		insert_algorithm_record(fp, "MINC", result, 0);
	}
	if (tasks & TASK_MINY)
	{
		result = evaluate(listOptimizer.get_miny_solution());
		insert_algorithm_record(fp, "MINY", result, 0);
	}
	if (tasks & TASK_PDP)
	{
		result = pdp_solve();
		pdp_result = result;
		running_time = pdp_running_time;
		insert_algorithm_record(fp, "PDP", result, running_time);
	}
	if (tasks & TASK_PSO)
	{
		PSOOptimizer psoOptimizer;
		result = evaluate(psoOptimizer.minimize());
		running_time = psoOptimizer.get_running_time();
		insert_algorithm_record(fp, "PSO", result, running_time);
	}
	if (tasks & TASK_TABU)
	{
		TabuOptimizer tabuOptimizer(false);

		result = evaluate(tabuOptimizer.minimize());
		running_time = tabuOptimizer.get_running_time();

		if (tasks & TASK_PDP)
		{
			insert_algorithm_record(fp, "TABU", result / pdp_result, running_time);
			if(result / pdp_result < 1)
				dump_critical_error("PDP RESULT LARGER THAN TABU");
		}
		else
			insert_algorithm_record(fp, "TABU", result, running_time);
	}
	if (tasks & TASK_TABU_RAND)
	{
		TabuOptimizer tabuOptimizer(true);

		result = evaluate(tabuOptimizer.minimize());
		running_time = tabuOptimizer.get_running_time();

		if (tasks & TASK_PDP)
		{
			insert_algorithm_record(fp, "TABU-RAND", result / pdp_result, running_time);
			if (result / pdp_result < 1)
				dump_critical_error("PDP RESULT LARGER THAN TABU");
		}
		else
			insert_algorithm_record(fp, "TABU-RAND", result, running_time);
	}

}

void start_task(FILE* record_file, int* M_array, int M_size, int* N_array, int N_size, double* B_array, int B_size, int case_number, int tasks)
{
	write_record_header(record_file);
	for (int m_idx = 0; m_idx < M_size; m_idx++)
	{
		current_config.M = M_array[m_idx];
		for (int n_idx = 0; n_idx < N_size; n_idx++)
		{
			current_config.N = N_array[n_idx];
			for (int b_idx = 0; b_idx < B_size; b_idx++)
			{
				for (int i = 0; i < case_number; i++)
				{
					current_config.b = B_array[b_idx];
					generate_test_data();

					run_algorithm(record_file, tasks);
				}
			}
		}
	}
}

void pdp_load_test()
{
	int tasks = TASK_PDP, instances_per_case = 20;
	current_config.b = 3;
	FILE* fp = fopen("result.csv", "w");

	current_config.M = 2;
	for (current_config.N = 16; current_config.N <= 20; current_config.N++)
	{
		for (int i = 0; i < instances_per_case; i++)
		{
			generate_test_data();
			run_algorithm(fp, tasks);
		}
	}

	current_config.M = 3;
	for (current_config.N = 12; current_config.N <= 16; current_config.N++)
	{
		for (int i = 0; i < instances_per_case; i++)
		{
			generate_test_data();
			run_algorithm(fp, tasks);
		}
	}

	current_config.M = 5;
	for (current_config.N = 8; current_config.N <= 12; current_config.N++)
	{
		for (int i = 0; i < instances_per_case; i++)
		{
			generate_test_data();
			run_algorithm(fp, tasks);
		}
	}
	fclose(fp);
}

void large_instances()
{
	int Ms[] = { 30, 40, 50 };
	int NpMs[] = { 3, 5, 7 };

	// dont forget to modify maxn value in job.h

	TabuOptimizer tabuOptimizer_orig(false);
	TabuOptimizer tabuOptimizer_rand(true);

	FILE *fp = fopen("large-result.csv", "w");
	write_record_header(fp);

	for (int i = 0; i < 3; i++)
	{
		current_config.M = Ms[i];
		for (int j = 0; j < sizeof(NpMs) / sizeof(int); j++)
		{
			current_config.N = Ms[i] * NpMs[j];
			current_config.b = 3;

			for (int k = 0; k < 10; k++)
			{
				generate_test_data();

				double result = evaluate(tabuOptimizer_orig.minimize());
				insert_algorithm_record(fp, "TABU", negative_improve_rate(tabuOptimizer_orig.get_inital_cost(), result), tabuOptimizer_orig.get_running_time());

				result = evaluate(tabuOptimizer_rand.minimize());
				insert_algorithm_record(fp, "TABU-RAND", negative_improve_rate(tabuOptimizer_rand.get_inital_cost(), result), tabuOptimizer_rand.get_running_time());
			}
		}
	}

	fclose(fp);
}

void small_instances()
{
	int tasks = TASK_PDP | TASK_TABU | TASK_TABU_RAND, instances_per_case = 10;
	current_config.b = 3;
	FILE* fp = fopen("small-result.csv", "w");

	write_record_header(fp);

	current_config.M = 2;
	for (current_config.N = 16; current_config.N <= 20; current_config.N++)
	{
		for (int i = 0; i < instances_per_case; i++)
		{
			generate_test_data();
			run_algorithm(fp, tasks);
		}
	}

	current_config.M = 3;
	for (current_config.N = 12; current_config.N <= 16; current_config.N++)
	{
		for (int i = 0; i < instances_per_case; i++)
		{
			generate_test_data();
			run_algorithm(fp, tasks);
		}
	}

	current_config.M = 5;
	for (current_config.N = 8; current_config.N <= 11; current_config.N++)
	{
		for (int i = 0; i < instances_per_case; i++)
		{
			generate_test_data();
			run_algorithm(fp, tasks);
		}
	}
	fclose(fp);
}

int main()
{
	srand(time(0));

	large_instances();

	//pdp_load_test();
	printf("program finished\n");

	return 0;
}