#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include "clock_system.h"


#define MAX_BUF 8192
struct timespec res;

/**
 * Purpose of this Code section:
 *  This code defines a struct called ExecutionData, which contains an array of 1000 double values representing execution times.  
 * 
 * What functions are included below:
 *  1. calculate_Average: function takes a pointer to an ExecutionData struct as input and calculates the average of the execution times stored in the struct.
 *  2. calculate_Standard_Difference: function takes a pointer to an ExecutionData struct as input, along with the average calculated by the calculate_Average function.
**/


typedef struct 
{
    double executionTime[3000];
} ExecutionData;


typedef struct 
{
    int voluntary;
    int nonvoluntary;
} ContextSwitches;


double calculate_Average(const ExecutionData* data, int iterations) 
{
    double sum = 0.0;
    int i;
    for (i = 0; i < iterations; ++i) {
        sum += data->executionTime[i];
    }
    return sum / iterations;
}


double calculate_Variance(const ExecutionData* data, double average,  int iterations) 
{
    double sum_of_squared_differences = 0.0;
    int i;
    for (i = 0; i < iterations; ++i) {
        double difference = data->executionTime[i] - average;
        sum_of_squared_differences += difference * difference;
    }
    double variance = sum_of_squared_differences / iterations;
    return variance;
}  /* the code section to do Average and Standard calculation ends.*/


/** 
 * Purpose of this Code section:
 *  The code provided is a set of functions for testing and analyzing the execution time of the  "clock_gettime"  function in C. 
 * 
 * What functions are included below:
 *  01. get_Time_Res
 *  02. record_Max_execution_time
 *  03. record_Min_execution_time
 *  04. count_if_More_than_100ns
 *  05. each_iteration_time_get_func
 *  06. initial_MinMax_value
 *  07. test_Time_get_func
 *  08. compute_Avg_execution_time
 *  09. compute_Variance
 *  10. test_Var_get_func
 *  11. compute_Variance_Average
 *  12. test_clock_gettime_func_avg_Full_Process
 *  13. test_clock_gettime_func_avgvar_Full_Process
 *  14. return_reasonable_or_not
**/


long max_execution_time;
long min_execution_time;
int count_more_than_100ns;

/**
 *  I uses the  clock_getres  function to retrieve the resolution of two different clocks:  CLOCK_MONOTONIC  and  CLOCK_REALTIME . 
**/
void get_Time_Res(struct timespec time_res)
{
    printf("-----------------------------------------\n");
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


long record_Max_execution_time(long execution_time) 
{
    if (execution_time > max_execution_time) {
        max_execution_time = execution_time;
    }
    return max_execution_time;
}


long record_Min_execution_time(long execution_time) 
{
    if (min_execution_time == 0 || execution_time < min_execution_time) {
        min_execution_time = execution_time;
    }
    return min_execution_time;
}


int count_if_More_than_100ns(long execution_time, int count_more_than_100ns)
{
    if (execution_time > 100)
    {
        count_more_than_100ns++;
    }
    return count_more_than_100ns;
}


long each_Iteration_Time_get_func()
{   
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time); // Get start time        
    clock_gettime(CLOCK_MONOTONIC, &end_time); // Get end time

    long execution_time = (end_time.tv_sec - start_time.tv_sec) * 1000000000L +
                            (end_time.tv_nsec - start_time.tv_nsec);    
    return execution_time;
}


void initial_MinMax_value()
{
    max_execution_time = 0;
    min_execution_time = 2147483647;
}


long test_Time_get_func(int iterations)
{
    struct timespec start_time, end_time;    
    long execution_time ,total_execution_time;
    total_execution_time = 0;
    count_more_than_100ns = 0;
    int i;  // Cause only c99 support declare var in for loop initialization phase
    for (i = 0; i < iterations; i++) 
    {
        execution_time = each_Iteration_Time_get_func();        
        max_execution_time = record_Max_execution_time(execution_time);
        min_execution_time = record_Min_execution_time(execution_time);
        count_more_than_100ns = count_if_More_than_100ns(execution_time, count_more_than_100ns);
        total_execution_time += execution_time;        
    }
    return total_execution_time;
}


long compute_Avg_execution_time(long total_execution_time, int iterations)
{    
    long average_execution_time = total_execution_time / iterations;
    return average_execution_time;
}


long compute_Variance(long difference)
{
    long variance;
    variance += difference * difference;
    return variance;
}


long test_Var_get_func(int iterations, long average_execution_time)
{
    struct timespec start_time, end_time;
    long difference, variance_total;
    int i;  // Cause only c99 support declare var in for loop initialization phase
    for (i = 0; i < iterations; i++) 
    {        
        difference = each_Iteration_Time_get_func() - average_execution_time;
        variance_total += compute_Variance(difference);        
    }
    return variance_total;
}


long compute_Variance_Average(long variance, int iterations)
{
    long variance_average = variance / iterations;
    return variance_average;
}


long test_clock_gettime_func_avg_Full_Process(int iterations, ExecutionData* data)
{   
    long total_execution_time = test_Time_get_func(iterations);
    long average_execution_time = compute_Avg_execution_time(total_execution_time, iterations);    
    printf("Average Execution Time: %ld ns\n", average_execution_time);
    printf("Max Execution Time this run: %ld ns\n", max_execution_time);
    printf("Min Execution Time this run: %ld ns\n", min_execution_time);
    printf("There are {%d} of {%d} which takes more than 100ns this run\n", count_more_than_100ns, iterations);
    return average_execution_time;
}


/**
 * Write down the output into file
 * The FLAG to enable this is LOG_API
*/

#ifdef LOG_API
void vPrint_Write_to_file(FILE *fp, const char* format, ...)
{
    // Create a va_list
    va_list args;
    va_start(args, format);

    // Print to Console    
    vprintf(format, args);
    // Write to file
    va_list args_copy;
    va_copy(args_copy, args);
    vfprintf(fp, format, args_copy);

    // Release
    va_end(args);
    va_end(args_copy);

    fclose(fp);
}


void vfPrint_sprintf(FILE *fp, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    // assume the buffer size 1024
    vprintf(format, args);
    vfprintf(fp, format, args);
    va_end(args);
}
#endif


#if 0
void test_clock_gettime_func_avgvar_Full_Process(int iterations, long average_executioon_time)
{
    max_execution_time = 0;
    min_execution_time = 2147483647;
    long variance = test_Var_get_func(iterations, average_executioon_time);
    long variance_average = compute_Variance_Average(variance, iterations);
    printf("Variance: %ld ns\n", variance_average);
}
#endif


/**
 * This section is to get thread info in Linux 
*/

// get PID of current thread
pid_t gettid()
{
    return syscall(SYS_gettid);
}


// after changing the method of fetching system-call outputs, the code section below is deprecated at 2023/7/20
#if 0
// store the "grep ctxt /proc/..." output into the data struct
ContextSwitches parse_context_switches(char *output)
{
    ContextSwitches switches;
    int matched = sscanf(
                            output,
                            "voluntary_ctxt_switches: %d\nnonvoluntary_ctxt_switches: %d",
                            &switches.voluntary, &switches.nonvoluntary
                        );
    if (matched == 2)
    {
        printf("Both values were successfully matched and assigned.\n");
    }
    else
    {
        printf("sscanf matched and assigned %d values.\n", matched);
    }
    
    return switches;
}
#endif


// call Linux command "grep ctxt /proc/..."
// input parameter is the PROCESS_PID currently executing
ContextSwitches grep_system_call(pid_t tid)
{
    // to store the "grep ctxt /proc/..." output]
    ContextSwitches switches;
    char cmd[MAX_BUF];
    sprintf(cmd, "grep ctxt /proc/%d/status", tid);

    FILE *fp_tmp = popen(cmd, "r");
    if (fp_tmp == NULL) 
    {
        printf("Failed to run commands\n");
        exit(1);
    }

    char buf[MAX_BUF];
    // read the system info from the text file which was made by system-call
    while   ( fgets(buf, sizeof(buf)-1, fp_tmp) != NULL )
    {
        // record the type of fetched information 
        int voluntary_matched, nonvoluntary_matched = 0;
        if ( sscanf(buf, "voluntary_ctxt_switches: %d", &switches.voluntary) == 1 )
        {
            voluntary_matched = 1;
        }
        else if ( sscanf(buf, "nonvoluntary_ctxt_switches: %d", &switches.nonvoluntary) == 1 )
        {
            nonvoluntary_matched = 1;
        }
#ifdef LOG_API
        vPrint_Write_to_file(fp_record, "%s", buf);
#else
        if (!voluntary_matched && !nonvoluntary_matched)
        {
            printf("Did not fetch ctxt_INF.\n");
        }
#if 0
        else if (!voluntary_matched)
        {
            printf("Fetch nonvoluntary_ctxt_INF. certainly.\n");
        }
        else if (!nonvoluntary_matched)
        {
            printf("Fetch voluntary_ctxt_INF. certainly.\n");
        }
        printf("%s", buf);
#endif
#endif
    }

    pclose(fp_tmp);

    return switches;
}

// compute the differences of switches in single test 
ContextSwitches compute_Diff_switches(ContextSwitches start, ContextSwitches end)
{
    ContextSwitches diff;
    diff.voluntary =  end.voluntary - start.voluntary;
    diff.nonvoluntary = end.nonvoluntary - start.nonvoluntary;
    return diff;
}

void print_switches_Info(ContextSwitches switches_Data)
{
    printf("----------------print info---------------\n");
    printf("voluntary_ctxt_switches:\t%d\nnonvoluntary_ctxt_switches:\t%d\n", switches_Data.voluntary, switches_Data.nonvoluntary);
    printf("----------------info end-----------------\n");
}


pid_t run_test(void *ptr)
{
    // create a file where the results is stored 
    char filename[50];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(filename, sizeof(filename), "%Y-%m-%d_%H-%M-%S", tm);
    FILE *fp_record = fopen(filename, "w");
    if (fp_record == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    pid_t tid = gettid();
    printf("-----------------------------------------\n");

#ifdef LOG_API
    vPrint_Write_to_file(fp_record, "Thread ID: %d\n", tid);
#else
    printf("Thread ID: %d\n", tid);
#endif


    get_Time_Res(res);

    ContextSwitches start_switches, end_switches, diff_switches, total_switches;
    long average_execution_time;
    int iterations = 3000;  // Number of iterations for the for loop
    int max_attempt = 15;   // Total test attempts 
    int k;
     

    for (k = 0; k < max_attempt; k++)
    {
        printf("-------------test #%2d--------------------\n", k);
        tid = gettid();
#ifdef LOG_API
        vPrint_Write_to_file(fp_record, "Thread ID: %d\n", tid);
#else
        printf("Thread ID: %d\n", tid);
#endif

        // Record the ctxt switch status at the test START
        start_switches = grep_system_call(tid);
#if 0
        print_switches_Info(start_switches);
#endif
        printf("------------thread info #%2d--------------\n", k);

        ExecutionData data = {0};
        // Fill in the execution time data into the struct
        initial_MinMax_value();

#if 0
#ifdef LOG_API
        vPrint_Write_to_file(fp_record, "This is the result of test #%d:\n", k);
#else
        printf("This is the result of test #%d:\n", k);
#endif
#endif

        average_execution_time = test_clock_gettime_func_avg_Full_Process(iterations, &data);
        double standardVariance = calculate_Variance(&data, average_execution_time, iterations);
        double standardDifference = sqrt(standardVariance);

#ifdef LOG_API
        vPrint_Write_to_file(fp_record, "Standard Variance: %.1fns\n", standardVariance);
        vPrint_Write_to_file(fp_record, "Standard Difference: %.1fns\n", standardDifference);        
        vPrint_Write_to_file(fp_record, "-----------------------------------------\n");
#else
        printf("Standard Variance: %.1fns\n", standardVariance);
        printf("Standard Difference: %.1fns\n", standardDifference);
        printf("-------------test #%2d end----------------\n", k);
#endif

        // Record the ctxt switch status at the test END
        end_switches = grep_system_call(tid);
        printf("-------------thread info #%2d-------------\n", k);

        // Compute the diff. of switches in single test
        diff_switches = compute_Diff_switches(start_switches, end_switches);
        print_switches_Info(diff_switches);
    }

    total_switches = grep_system_call(tid);
    print_switches_Info(total_switches);

    fclose(fp_record);

    return tid;
}



int main() {
    pthread_t thread;
    pid_t tid;

    if ( pthread_create(&thread, NULL, (void *)run_test, NULL)  ) 
    {
        fprintf(stderr, "Error creating thread!\n");
        return 1;
    }
    
    if ( pthread_join(thread, (void **)&tid) )
    {
        fprintf(stderr, "Error joining thread!\n");
        return 2;
    }

    printf("-----------------------------------------\n");
    int return_value = system("lscpu");
    if (return_value == -1)
    {
        perror("system");
        return EXIT_FAILURE;
    }

    printf("Thread ID at the end of the pogram: %d\n", tid);
    printf("-----------------------------------------\n");
    
    return 0;
}