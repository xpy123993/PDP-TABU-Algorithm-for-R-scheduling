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

to run algorithms only, use
```c++
run_algorithm(FILE *fp, int tasks)
```
the result will be dumped to fp handle.
