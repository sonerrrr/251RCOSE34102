#ifndef MYHEADER_H
#define MYHEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// general functions
void try_and_abort(bool, char*);
int rand_int(int, int);
int get_parent(int);
int get_first_child(int);

// processes
enum P_Type { CPU_BOUND = 0, MIXED = 1, IO_BOUND = 2 };
// process generation configuration
#define PRIORITY_RANGE 5
#define ARRIVAL_RANGE 30
// diverse configuration with process type
extern const int CPU_BURST_RANGE[3]; 
extern const int IO_BURST_RANGE[3];
extern const int IO_TIME_LOWER_RANGE[3];
extern const int IO_TIME_UPPER_RANGE[3];
// composition scenario of p_types 
extern const int P_TYPE_DIST[3][3];

typedef struct __Process {
    int pid;
    int *bursts; // CPU burst, I/O burst time alternating
    int io_time;
    int arrival_time;
    int priority;
} Process;

enum P_Type Generate_P_Type(const int*);
Process Generate_Process(enum P_Type, int);
Process* Generate_Process_List(const int*, int, int);
void Free_All_Processes(Process*, int);

// priority queue
// each node of P_Queue.data
typedef struct __PQ_Node{
    Process p;
    int key;
    int lasting;
} PQ_Node;

typedef struct __P_Queue{
    int size;
    int last; // positiion where to push new element
    PQ_Node *data;
} P_Queue;

P_Queue PQ_Create(int n_process);
void PQ_Delete(P_Queue *pq);
void PQ_Node_Deep_Copy(PQ_Node *to, PQ_Node *from);
void PQ_Swap(P_Queue *pq, int i1, int i2);
void PQ_Push(P_Queue *pq, Process _p, int _key, int _lasting);
Process PQ_Pop(P_Queue *pq);
bool PQ_isEmpty(P_Queue *pq);

// scheduler
#define MAX_CHART_LENGTH 1000
#define MAX_SIMULATION_TIME 1000

enum SCH_ALG {
    FCFS,
    SJF_NONPREMP,
    SJF_PREMP,
    PRIORITY_NONPREMP,
    PRIORITY_PREMP,
    RR
};

// definition of data records
typedef struct __Chart_Node{
    int *pid_arrived, *pid_ready, *pid_wait;
    int size_arrived, size_ready, size_wait;
} Chart_Node;

// progress: indicate burst progress
typedef struct __Record_Node{
    int progress, turnaround, waiting;
} Record_Node;

typedef struct __Report{
    int total_time;
    Chart_Node *chart;
    Record_Node *record;
}Report;

int Get_Burst_Time(Report *r, Process p);
bool Progress_Burst(Report *r, Process p);
void Increment_Waiting_Time(Report *r, P_Queue *ready);
Chart_Node Capture(P_Queue* arrival, P_Queue* ready, P_Queue* wait);
void Scheduler_Subroutine(Report *r, P_Queue* arrival, P_Queue* ready, P_Queue* wait, Process* ps, int n_process);
Report Scheduler(Process* ps, int n_process);
void Release_Report(Report *r);

// evaluation
#define MAX_GANTT_LENGTH 100

// Gantt chart
typedef struct __Gantt_Node{
    int pid, start, end;
} Gantt_Node;

typedef struct __Gantt{
    int size;
    Gantt_Node *chart;
} Gantt;

Gantt Create_Gantt_Chart(Report r);
void Release_Gantt_Chart(Gantt *g);
#endif