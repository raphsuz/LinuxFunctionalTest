#ifndef CLOCK_SYSTEM_H
#define CLOCK_SYSTEM_H

typedef struct 
{
    double executionTime[3000];
} ExecutionData;

typedef struct 
{
    int voluntary;
    int nonvoluntary;
} ContextSwitches;

// Function
double calculate_Average(const ExecutionData* data, int iterations);
double calculate_Variance(const ExecutionData* data, double average, int iterations);
void get_Time_Res(struct timespec time_res);
long record_Max_execution_time(long execution_time);
long record_Min_execution_time(long execution_time);
int count_if_More_than_100ns(long execution_time, int count_more_than_100ns);
long each_Iteration_Time_get_func();
void initial_MinMax_value();
long test_Time_get_func(int iterations);
long compute_Avg_execution_time(long total_execution_time, int iterations);
long compute_Variance(long difference);
long test_Var_get_func(int iterations, long average_execution_time);
long compute_Variance_Average(long variance, int iterations);
long test_clock_gettime_func_avg_Full_Process(int iterations, ExecutionData* data);
#ifdef LOG_API
void vPrint_Write_to_file(FILE *fp, const char* format, ...);
void vfPrint_sprintf(FILE *fp, const char *format, ...);
#endif
pid_t gettid();
ContextSwitches grep_system_call(pid_t tid);
ContextSwitches compute_Diff_switches(ContextSwitches start, ContextSwitches end);
void print_switches_Info(ContextSwitches switches_Data);
pid_t run_test(void *ptr);


#endif