#include <stdio.h>
#include <time.h>

long max_execution_time;
long min_execution_time;
int count_more_than_100ns;

/**
 *  I uses the  clock_getres  function to retrieve the resolution of two different clocks:  CLOCK_MONOTONIC  and  CLOCK_REALTIME . 
**/
void get_time_res(struct timespec time_res)
{
    if (clock_getres(CLOCK_MONOTONIC, &time_res) == 0) 
    {
        printf("CLOCK_MONOTONIC resolution: %ld sec, %ld nsec\n", time_res.tv_sec, time_res.tv_nsec);
    } 
    else 
    {
        printf("Error getting CLOCK_MONOTONIC resolution\n");
    }

    if (clock_getres(CLOCK_REALTIME, &time_res) == 0) 
    {
        printf("CLOCK_REALTIME resolution: %ld sec, %ld nsec\n", time_res.tv_sec, time_res.tv_nsec);
    } 
    else 
    {
        printf("Error getting CLOCK_REALTIME resolution\n");
    }
    printf("-----------------------------------------\n");
}

long record_max_execution_time(long execution_time) 
{
    if (execution_time > max_execution_time) {
        max_execution_time = execution_time;
    }
    return max_execution_time;
}

long record_min_execution_time(long execution_time) 
{
    if (min_execution_time == 0 || execution_time < min_execution_time) {
        min_execution_time = execution_time;
    }
    return min_execution_time;
}

int count_if_more_than_100ns(long execution_time, int count_more_than_100ns)
{
    if (execution_time > 100)
    {
        count_more_than_100ns++;
    }
    return count_more_than_100ns;
}

long each_iteration_time_get_func()
{   
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time); // Get start time        
    clock_gettime(CLOCK_MONOTONIC, &end_time); // Get end time

    long execution_time = (end_time.tv_sec - start_time.tv_sec) * 1000000000L +
                            (end_time.tv_nsec - start_time.tv_nsec);    
    return execution_time;
}

void initial_minmax_value()
{
    max_execution_time = 0;
    min_execution_time = 2147483647;
}

long test_time_get_func(int iterations)
{
    struct timespec start_time, end_time;    
    long execution_time ,total_execution_time;
    total_execution_time = 0;
    count_more_than_100ns = 0;
    int i;  // Cause only c99 support declare var in for loop initialization phase
    for (i = 0; i < iterations; i++) 
    {
        execution_time = each_iteration_time_get_func();
        max_execution_time = record_max_execution_time(execution_time);
        min_execution_time = record_min_execution_time(execution_time);
        count_more_than_100ns = count_if_more_than_100ns(execution_time, count_more_than_100ns);
        total_execution_time += execution_time;
    }
    return total_execution_time;
}


long compute_avg_execution_time(long total_execution_time, int iterations)
{    
    long average_execution_time = total_execution_time / iterations;
    return average_execution_time;
}

long compute_variance(long difference)
{
    long variance;
    variance += difference * difference;
    return variance;
}

long test_var_get_func(int iterations, long average_execution_time)
{
    struct timespec start_time, end_time;
    long difference, variance_total;
    int i;  // Cause only c99 support declare var in for loop initialization phase
    for (i = 0; i < iterations; i++) 
    {        
        difference = each_iteration_time_get_func() - average_execution_time;
        variance_total += compute_variance(difference);        
    }
    return variance_total;
}

long compute_variance_average(long variance, int iterations)
{
    long variance_average = variance / iterations;
    return variance_average;
}

long test_clock_gettime_func_avg_full_process(int iterations)
{       
    long total_execution_time = test_time_get_func(iterations);
    long average_execution_time = compute_avg_execution_time(total_execution_time, iterations);
    printf("Average Execution Time: %ld ns\n", average_execution_time);
    printf("Max Execution Time this run: %ld ns\n", max_execution_time);
    printf("Min Execution Time this run: %ld ns\n", min_execution_time);
    printf("There are {%d} of {%d} which takes more than 100ns this run\n", count_more_than_100ns, iterations);
    return average_execution_time;
}

void test_clock_gettime_func_avgvar_full_process(int iterations, long average_executioon_time)
{
    max_execution_time = 0;
    min_execution_time = 2147483647;
    long variance = test_var_get_func(iterations, average_executioon_time);
    long variance_average = compute_variance_average(variance, iterations);
    printf("Variance: %ld ns\n", variance_average);
}

int main() {
    struct timespec res;
    get_time_res(res);

    long average_execution_time;
    int iterations = 3000;  // Number of iterations for the for loop
    int max_attempt = 15;   // Total test attempts 
    int k;
    for (k = 0; k < max_attempt; k++)
    {        
        initial_minmax_value();
        printf("This is the result of test #%d\n", k);
        average_execution_time = test_clock_gettime_func_avg_full_process(iterations);
        test_clock_gettime_func_avgvar_full_process(iterations, average_execution_time);
    }

    return 0;
}