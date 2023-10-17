#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#define PS_MAX 10

// holds the scheduling processes of one process
typedef struct
{
    int index;      // process index (index)
    int arr_time;   // arrival time
    int burst_time; // burst time
    int resp_time;  // response time
    int wait_time;  // waiting time
    int end_time;   // completion time
    int turn_time;  // turnaround time
    int rem_burst;  // remaining burst (this should decrement when the process is being executed)
} ProcessData;

int running_process_index = -1; // the index of the running process, -1 means no running processes

unsigned total_time; // the total time of the timer

ProcessData processes[PS_MAX]; // array of process data
unsigned processes_len;        // size of processes array

pid_t pids[PS_MAX]; // array of all process pids, zero valued pids - means the process is terminated or not created yet

void read_file(FILE *file)
{
    fscanf(file, "%*[^\n]"); // skip first line
    for (processes_len = 0; fscanf(file, "%d", &processes[processes_len].index) > 0; processes_len++)
    {
        fscanf(file, "%d", &processes[processes_len].arr_time);
        fscanf(file, "%d", &processes[processes_len].burst_time);

        processes[processes_len].end_time = 0;
        processes[processes_len].wait_time = 0;
        processes[processes_len].rem_burst = processes[processes_len].burst_time;
    }

    for (int i = 0; i < PS_MAX; i++)
    {
        pids[i] = 0;
    }
}

// send signal SIGCONT to the worker process
void resume(pid_t process)
{
    if (process != 0)
    {
        kill(process, SIGCONT);
    }
}

// send signal SIGTSTP to the worker process
void suspend(pid_t process)
{
    if (process != 0)
    {
        kill(process, SIGTSTP);
    }
}

// send signal SIGTERM to the worker process
void terminate(pid_t process)
{
    if (process != 0)
    {
        kill(process, SIGTERM);
    }
}

// create a process using fork
void create_process(int new_process_index)
{
    // stop the running process
    if (running_process_index != -1)
    {
        suspend(pids[running_process_index]);
    }

    // fork a new process and add it to pids array
    pid_t new_process_pid = fork();
    if (new_process_pid != 0)
    {
        pids[new_process_index] = new_process_pid;
        // Now the index of the running process is new_process_index
        running_process_index = new_process_index;
        return;
    }

    char buffer[15];
    sprintf(buffer, "%d", new_process_index);
    char *argv[] = {"./worker", buffer, NULL};

    // The process runs the program "./worker {new_process_index}"
    exit(execvp(argv[0], argv));
}

// find next process for running
ProcessData find_next_process()
{

    // index of next process in {processes} array
    int next_process_index = 0;
    int min_arr_time = __INT_MAX__;

    if (processes[running_process_index].rem_burst != 0)
    {
        return processes[running_process_index];
    }

    for (int i = 0; i < processes_len; i++)
    {
        if (
            processes[i].arr_time < min_arr_time &&
            processes[i].rem_burst > 0)
        {
            next_process_index = i;
            min_arr_time = processes[i].arr_time;
        }
    }

    // if next_process did not arrive so far,
    // then we recursively call this function after incrementing total_time
    if (processes[next_process_index].arr_time > total_time)
    {
        printf("Scheduler: Runtime: %u seconds.\nProcess %d: has not arrived yet.\n", total_time, next_process_index);

        // increment the time
        total_time++;
        return find_next_process();
    }

    // return the processes of next process
    return processes[next_process_index];
}

// reports the metrics and simulation results
void report()
{
    printf("Simulation results.....\n");
    int sum_wt = 0;
    int sum_tat = 0;
    for (int i = 0; i < processes_len; i++)
    {
        printf("process %d: \n", i);
        printf("	at=%d\n", processes[i].arr_time);
        printf("	bt=%d\n", processes[i].burst_time);
        printf("	ct=%d\n", processes[i].end_time);
        printf("	wt=%d\n", processes[i].wait_time);
        printf("	tat=%d\n", processes[i].turn_time);
        printf("	rt=%d\n", processes[i].resp_time);
        sum_wt += processes[i].wait_time;
        sum_tat += processes[i].turn_time;
    }

    printf("processes size = %d\n", processes_len);
    float avg_wt = (float)sum_wt / processes_len;
    float avg_tat = (float)sum_tat / processes_len;
    printf("Average results for this run:\n");
    printf("	avg_wt=%f\n", avg_wt);
    printf("	avg_tat=%f\n", avg_tat);
}

void check_burst()
{

    for (int i = 0; i < processes_len; i++)
    {
        if (processes[i].rem_burst > 0)
        {
            return;
        }
    }

    // report simulation results
    report();

    // terminate the scheduler :)
    exit(EXIT_SUCCESS);
}

// This function is called every one second as handler for SIGALRM signal
void schedule_handler(int signum)
{
    // increment the total time
    total_time++;

    if (running_process_index != -1)
    {
        printf("Scheduler: Runtime: %d seconds\n", total_time);

        processes[running_process_index].rem_burst--;
        printf("Process %d is running with %d seconds left\n", running_process_index, processes[running_process_index].rem_burst);

        if (processes[running_process_index].rem_burst == 0)
        {
            terminate(pids[running_process_index]);
            pids[running_process_index] = 0;
            printf("Scheduler: Terminating Process %d (Remaining Time: %d)\n", running_process_index, processes[running_process_index].rem_burst);
            waitpid(pids[running_process_index], NULL, 0);
            // Since the process is terminated, we can calculate its metrics {end_time, turn_time, wait_time}
            processes[running_process_index].end_time = total_time;
            processes[running_process_index].turn_time = processes[running_process_index].end_time - processes[running_process_index].arr_time;
            processes[running_process_index].wait_time = processes[running_process_index].turn_time - processes[running_process_index].burst_time;
            running_process_index = -1;
        }
    }

    ProcessData next_process = find_next_process();

    check_burst(); // exit if there are no processes;

    if (running_process_index != next_process.index)
    {
        if (running_process_index != -1)
        {
            suspend(pids[running_process_index]);
            printf("Scheduler: Stopping Process %d (Remaining Time: %d)\n", running_process_index, processes[running_process_index].rem_burst);
        }

        running_process_index = next_process.index;
    }

    if (pids[running_process_index] == 0)
    {
        create_process(next_process.index);
        printf("Scheduler: Starting Process %d (Remaining Time: %d)\n", running_process_index, processes[running_process_index].rem_burst);
        processes[running_process_index].resp_time = total_time;
    }

    int status;
    if (waitpid(pids[running_process_index], &status, WNOHANG | WUNTRACED) > 0 && WIFSTOPPED(status))
    {
        resume(pids[running_process_index]);
        printf("Scheduler: Resuming Process %d (Remaining Time: %d)", running_process_index, processes[running_process_index].rem_burst);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Provide filepath to read data from\n");
        exit(EXIT_FAILURE);
    }
    // read the processes file
    FILE *in_file = fopen(argv[1], "r");
    if (in_file == NULL)
    {
        printf("File is not found or cannot open it!\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        read_file(in_file);
    }

    // set a timer
    struct itimerval timer;

    // the timer goes off 1 second after reset
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    // timer increments 1 second arr_time a time
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    // this counts down and sends SIGALRM to the scheduler process after expiration.
    setitimer(ITIMER_REAL, &timer, NULL);

    // register the handler for SIGALRM signal
    signal(SIGALRM, schedule_handler);

    // Wait till all processes finish
    for (;;)
        ; // infinite loop
}