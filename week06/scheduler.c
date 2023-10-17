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
    int ID;         // process ID (ID)
    int arr_time;   // arrival time
    int burst_time; // burst time
    int resp_time;  // response time
    int wait_time;  // waiting time
    int end_time;   // completion time
    int turn_time;  // turnaround time
    int rem_burst;  // remaining burst (this should decrement when the process is being executed)
} ProcessData;

int current_process_ID = -1; // the ID of the running process, -1 means no running processes

unsigned total_time; // the total time of the timer

ProcessData procs[PS_MAX]; // array of process data. maps index to process
unsigned procs_len;        // size of procs array

pid_t pids[PS_MAX]; // array of all process pids, zero valued pids - means the process is terminated or not created yet. map ID (not index!!!) to pid

void read_file(FILE *file)
{
    fscanf(file, "%*[^\n]"); // skip first line
    for (procs_len = 0; fscanf(file, "%d", &procs[procs_len].ID) > 0; procs_len++)
    {
        fscanf(file, "%d", &procs[procs_len].arr_time);
        fscanf(file, "%d", &procs[procs_len].burst_time);

        procs[procs_len].end_time = 0;
        procs[procs_len].wait_time = 0;
        procs[procs_len].rem_burst = procs[procs_len].burst_time;
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
void create_process(int new_process_ID)
{
    // stop the running process
    if (current_process_ID != -1)
    {
        suspend(pids[current_process_ID]);
    }

    // fork a new process and add it to pids array
    pid_t new_process_pid = fork();
    if (new_process_pid != 0)
    {
        pids[new_process_ID] = new_process_pid;
        // Now the ID of the running process is new_process_ID
        current_process_ID = new_process_ID;
        return;
    }

    char buffer[15];
    sprintf(buffer, "%d", new_process_ID);
    char *argv[] = {"./worker", buffer, NULL};

    // The process runs the program "./worker {new_process_ID}"
    exit(execvp(argv[0], argv));
}

// find next process for running
ProcessData find_next_process()
{

    // ID of next process in {processes} array
    int next_process_index = 0;
    int min_arr_time = __INT_MAX__;

    for (int i = 0; i < procs_len; i++)
    {
        if (
            procs[i].arr_time < min_arr_time &&
            procs[i].rem_burst > 0)
        {
            next_process_index = i;
            min_arr_time = procs[i].arr_time;
        }
    }

    // if next_process did not arrive so far,
    // then we recursively call this function after incrementing total_time
    if (procs[next_process_index].arr_time > total_time)
    {
        printf("Scheduler: Runtime: %u seconds.\nProcess %d: has not arrived yet.\n", total_time, next_process_index);

        // increment the time
        total_time++;
        return find_next_process();
    }

    // return the processes of next process
    return procs[next_process_index];
}

// reports the metrics and simulation results
void report()
{
    printf("Simulation results.....\n");
    int sum_wt = 0;
    int sum_tat = 0;
    for (int i = 0; i < procs_len; i++)
    {
        printf("process %d: \n", i);
        printf("	at=%d\n", procs[i].arr_time);
        printf("	bt=%d\n", procs[i].burst_time);
        printf("	ct=%d\n", procs[i].end_time);
        printf("	wt=%d\n", procs[i].wait_time);
        printf("	tat=%d\n", procs[i].turn_time);
        printf("	rt=%d\n", procs[i].resp_time);
        sum_wt += procs[i].wait_time;
        sum_tat += procs[i].turn_time;
    }

    printf("processes size = %d\n", procs_len);
    float avg_wt = (float)sum_wt / procs_len;
    float avg_tat = (float)sum_tat / procs_len;
    printf("Average results for this run:\n");
    printf("	avg_wt=%f\n", avg_wt);
    printf("	avg_tat=%f\n", avg_tat);
}

void check_burst()
{

    for (int i = 0; i < procs_len; i++)
    {
        if (procs[i].rem_burst > 0)
        {
            return;
        }
    }

    // report simulation results
    report();

    // terminate the scheduler :)
    exit(EXIT_SUCCESS);
}

int index_by_ID(int ID)
{
    for (int i = 0; i < procs_len; i++)
    {
        if (procs[i].ID == ID)
        {
            return i;
        }
    }
    return -1;
}

// This function is called every one second as handler for SIGALRM signal
void schedule_handler(int signum)
{
    // increment the total time
    total_time++;

    int current_index = index_by_ID(current_process_ID);

    if (current_process_ID != -1)
    {
        printf("Scheduler: Runtime: %d seconds\n", total_time);

        procs[current_index].rem_burst--;
        printf("Process %d is running with %d seconds left\n", procs[current_index].ID, procs[current_index].rem_burst);

        if (procs[current_index].rem_burst == 0)
        {
            terminate(pids[current_process_ID]);
            pids[current_process_ID] = 0;
            printf("Scheduler: Terminating Process %d (Remaining Time: %d)\n", procs[current_index].ID, procs[current_index].rem_burst);
            waitpid(pids[current_process_ID], NULL, 0);
            // Since the process is terminated, we can calculate its metrics {end_time, turn_time, wait_time}
            procs[current_index].end_time = total_time;
            procs[current_index].turn_time = procs[current_index].end_time - procs[current_index].arr_time;
            procs[current_index].wait_time = procs[current_index].turn_time - procs[current_index].burst_time;
            current_process_ID = -1;
        }
    }

    ProcessData next_process = find_next_process();
    check_burst(); // exit if there are no processes;

    if (current_process_ID != next_process.ID)
    {
        if (current_process_ID != -1)
        {
            suspend(pids[current_process_ID]);
            printf("Scheduler: Stopping Process %d (Remaining Time: %d)\n", procs[current_index].ID, procs[current_index].rem_burst);
        }

        current_process_ID = next_process.ID;
        current_index = index_by_ID(current_process_ID);
    }

    if (pids[current_process_ID] == 0)
    {
        create_process(current_process_ID);
        printf("Scheduler: Starting Process %d (Remaining Time: %d)\n", procs[current_index].ID, procs[current_index].rem_burst);
        procs[current_index].resp_time = total_time;
    }

    int status;
    if (waitpid(pids[current_process_ID], &status, WNOHANG | WUNTRACED) > 0 && WIFSTOPPED(status))
    {
        resume(pids[current_process_ID]);
        printf("Scheduler: Resuming Process %d (Remaining Time: %d)", procs[current_index].ID, procs[current_index].rem_burst);
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