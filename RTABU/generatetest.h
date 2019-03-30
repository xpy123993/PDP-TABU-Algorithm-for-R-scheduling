#include <stdio.h>
#include <stdlib.h>

#include "job.h"

//Parameters for test data
const char* JOB_FILE = "test.dat";
double r = 0.5, t = 0.6;

void store_test_data()
{
	FILE *fp = fopen(JOB_FILE, "wb");
	if (!fp) return;

	fwrite(test_data, sizeof(Job), current_config.N, fp);
	fclose(fp);
}

void load_test_data()
{
	FILE *fp = fopen(JOB_FILE, "rb");
	if (!fp) return;

	fread(test_data, sizeof(Job), current_config.N, fp);
	fclose(fp);
}

/*
2017-5-9
数据生成方法
参考论文
Two machine flow shop scheduling to minimize total late work
*/
void generate_test_data_2()
{
	int mapper_index_1, mapper_index_2;
	int mapper[ALLOCATE_MAX_N];
	double min_due_date, max_due_date;
	for (int i = 0; i < current_config.N; i++)
	{
		mapper[i] = i;
		for (int j = 0; j < current_config.M; j++)
		{
			test_data[i].proc_time[j] = u(JOB_PROCTIME_MIN, JOB_PROCTIME_MAX);
		}
	}

	sort(mapper, mapper + current_config.N, cmp_avg_proc);

	for (int i = 0; i < current_config.N; i++)
	{
		mapper_index_1 = mapper[i];
		min_due_date = avg(test_data[mapper_index_1].proc_time, current_config.M);
		max_due_date = min_due_date;

		for (int k = 0; k < i; k++)
		{
			mapper_index_2 = mapper[k];
			max_due_date += avg(test_data[mapper_index_2].proc_time, current_config.M) / current_config.b;
		}

		test_data[mapper_index_1].due_date = u(min_due_date, max_due_date);
	}
	store_test_data();
}

/*
2017-2-27
生成数据参考论文
Parallel-machine scheduling to minimize tardiness penalty and power cost
*/
void generate_test_data()
{


	int proc_time_sum = 0, min_due_date, max_due_date, due_date_range;

	double avg_proc_time;
	double avg_machine_load = 0;

	for (int i = 0; i < current_config.N; i++)
	{
		//for every test-job
		proc_time_sum = 0;
		for (int j = 0; j < current_config.M; j++)
		{
			// generate individual speed for every machine
			test_data[i].proc_time[j] = u(JOB_PROCTIME_MIN, JOB_PROCTIME_MAX);
			proc_time_sum += test_data[i].proc_time[j];
		}
		avg_proc_time = 1.0 * proc_time_sum / current_config.M;
		avg_machine_load += avg_proc_time;
	}

	avg_machine_load = avg_machine_load / current_config.M;

	due_date_range = avg_machine_load * r;

	min_due_date = avg_machine_load * (1.0 - t - r / 2);
	max_due_date = min_due_date + due_date_range;

	for (int i = 0; i < current_config.N; i++)
	{
		// generate due date
		test_data[i].due_date = u(min_due_date, max_due_date);
		//print_job_info(test_data[i]);
	}

	//printf("due date: [%d, %d)\n", min_due_date, max_due_date);

	//store_test_data();
}
