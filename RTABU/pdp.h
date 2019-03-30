#pragma once
#include "job.h"
#include "utils.h"
#include <time.h>

const int INF = 10000007;

int assigned_machine[ALLOCATE_MAX_N];    //use to record the machine to process each job
int best_case_cost = INF;

int machine[ALLOCATE_MAX_M][ALLOCATE_MAX_N];
int job_num[ALLOCATE_MAX_M] = { 0 };

int dp_p1_dp_table_b[(ALLOCATE_MAX_N > 40 ? 40 : ALLOCATE_MAX_N)];
int dp_p1_dp_table_f[(ALLOCATE_MAX_N > 40 ? 40 : ALLOCATE_MAX_N) + 1][((ALLOCATE_MAX_N > 40 ? 40 : ALLOCATE_MAX_N) + 10) * JOB_PROCTIME_MAX];

double pdp_running_time;

bool order_job_cmp_due_date(int j1, int j2)
{
    return test_data[j1].due_date < test_data[j2].due_date;
}

int dp_p1_single_dp(int machine_id, int job_count)
{
    int proc_sum = 0, max_dp = 0, ans = INF;
    sort(machine[machine_id], machine[machine_id] + job_count, order_job_cmp_due_date);
    for(int i = 0; i < job_count; i ++)
    {
        Job& current_job = test_data[machine[machine_id][i]];
        max_dp = max(max_dp, current_job.proc_time[machine_id] + current_job.due_date);
        proc_sum += current_job.proc_time[machine_id];
        dp_p1_dp_table_b[i] = min(proc_sum, max_dp - 1);
        for(int j = 0; j < ALLOCATE_MAX_N * JOB_PROCTIME_MAX; j ++)
            dp_p1_dp_table_f[i][j] = INF;
    }
    dp_p1_dp_table_f[0][0] = 0;
    for(int i = 0; i < job_count; i ++)
    {
		Job& current_job = test_data[machine[machine_id][i]];
        for(int t = 0; t <= dp_p1_dp_table_b[i]; t ++)
        {
            dp_p1_dp_table_f[i + 1][t] = dp_p1_dp_table_f[i][t] + current_job.proc_time[machine_id];
            if(current_job.proc_time[machine_id] <= t && t < current_job.proc_time[machine_id] + current_job.due_date)
                dp_p1_dp_table_f[i + 1][t] = min(dp_p1_dp_table_f[i + 1][t], dp_p1_dp_table_f[i][t - current_job.proc_time[machine_id]] + max(t - current_job.due_date, 0));
            if(i == job_count - 1)
                ans = min(ans, dp_p1_dp_table_f[i + 1][t]);
        }
    }
    return ans;
}

int pdp_estimate()
{
	int sum_proc[ALLOCATE_MAX_M] = { 0 }, max_due[ALLOCATE_MAX_M] = { 0 };
	int machine_id = -1, max_due_date_process_time = 0, late_work = 0, lb = 0, total_late = 0;

	for (int i = 0; i < ALLOCATE_MAX_M; i++)
		job_num[i] = 0;

	for (int i = 0; i < current_config.N; i++)
	{
		machine_id = (assigned_machine[i]) % current_config.M;
		machine[machine_id][job_num[machine_id]++] = i;
		sum_proc[machine_id] += test_data[i].proc_time[machine_id];
		max_due[machine_id] = max(max_due[machine_id], test_data[i].due_date);
	}

	for (int i = 0; i < current_config.M; i++)
		lb += max(0, sum_proc[i] - max_due[i]);
	if (lb >= best_case_cost) return INF;

	for (int i = 0; i < current_config.M; i++)
	{
	    if(job_num[i] == 0) continue;
		total_late += dp_p1_single_dp(i, job_num[i]);
		if (total_late >= best_case_cost) return INF;
	}
	return total_late;
}

void dp_dfs_solve(int current_job_index)
{
	if (current_job_index == current_config.N)
	{
		int cost = pdp_estimate();
		if (cost < best_case_cost)
			best_case_cost = cost;
	}
	else
	{
		for (int i = 0; i < current_config.M; i++)
		{
			assigned_machine[current_job_index] = i;
			dp_dfs_solve(current_job_index + 1);
		}
	}
}

int pdp_solve()
{
	LARGE_INTEGER cpu_freq, t1, t2;

	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);

	best_case_cost = INF;

	memset(assigned_machine, 0, sizeof(int) * ALLOCATE_MAX_N);

	dp_dfs_solve(0);

	QueryPerformanceCounter(&t2);

	pdp_running_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;

	return best_case_cost;
}
