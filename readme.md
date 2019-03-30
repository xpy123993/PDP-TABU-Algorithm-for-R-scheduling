## PDP & TABU Algorithm for R scheduling


### Code files description
*generatetest.h*: defines data generation methods   
*job.h*: defines data structures and algorithms related to job   
*listalgo.h*: defines list scheduling algorithms   
*pch.h*: program header file   
*pdp.h*: defines pdp algorithm   
*pso.h*: defines particle swarm optimization   
*solution.h*: defines data structures and algorithms related to solution (except pdp, the procedure to save the scheduling is undefined)    
*tabu.h*: defines tabu search algorithm   
*utils.h*: defines helper functions   
Main entrance: *rtabu.cpp*

### Some coding examples:
to enumerate all possible M in ms, N in ns and B in bs (might not work depending on test generation method)
```c++
int Ms[] = {2, 3, 5};
int Ns[] = {6, 8, 10, 12};
double Bs[] = {3, 5, 7};
int case_number = 10;

int tasks = TASK_PDP | TASK_TABU | TASK_TABU_RAND; //all algorithms should be concated by '|'

FILE* fp = fopen("result.csv", "w");
start_task(fp, Ms, sizeof(Ms) / sizeof(int), Ns, sizeof(Ns) / sizeof(int), Bs, sizeof(Bs) / sizeof(double), case_number, tasks);
fclose(fp);
```

to design customized test case, first open a file for recording results:
```c++
FILE *fp = fopen("result.csv", "w");
write_record_header(fp); // csv column header
```

to run one test case (m=2, n=10, b=3)

```c++
current_config.M = 2; // set the number of machines
current_config.N = 10;// set the number of tasks
current_config.b = 3; // set lateness value of test generation (might be useless if use the first method)

generate_test_data();

int tasks = TASK_PDP | TASK_TABU | TASK_PSO; // select certain algorithms to run: pdp, tabu and pso.
run_algorithm(FILE *fp, int tasks)
```
the result will be dumped to fp handle.
