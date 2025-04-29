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
int min(int, int);

// processes
typedef enum _P_Type { CPU_BOUND = 0, MIXED = 1, IO_BOUND = 2 } P_Type;
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

P_Type Generate_P_Type(const int*);
Process Generate_Process(P_Type, int);
Process* Generate_Process_List(const int*, int, int);
void Free_All_Processes(Process*, int);

// priority queue
// each node of P_Queue.data
typedef struct __PQ_Node{
    int pid;
    int key;
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
void PQ_Push(P_Queue *pq, int _pid, int _key);
int PQ_Pop(P_Queue *pq);
bool PQ_isEmpty(P_Queue *pq);

// scheduler
#define MAX_CHART_LENGTH 1000
#define MAX_SIMULATION_TIME 1000

typedef enum _Sch_Alg {
    FCFS,
    SJF_NONPREMP,
    SJF_PREMP,
    PRIORITY_NONPREMP,
    PRIORITY_PREMP,
    RR
} Sch_Alg;

// definition of data records
typedef struct __Chart_Node{
    int *pid_arrived, *pid_ready, *pid_wait;
    int size_arrived, size_ready, size_wait;
    int pid_in_cpu, pid_in_io;
} Chart_Node;

typedef struct __Record_Node{
    int progress; // indicate current burst
    int burst_remained; // current burst time remained
    int session_remained; // current session remained
    int turnaround; // turnaround time
    int waiting; // waiting time
} Record_Node;

typedef struct __Report{
    int total_time;
    Chart_Node *chart;
    Record_Node *record;
}Report;

void Increment_Waiting_Time(Report *r, P_Queue *ready);
Chart_Node Capture(P_Queue* arrival, P_Queue* ready, P_Queue* wait, int pid_in_cpu, int pid_in_io);
void Push_Ready_Queue(Report *r, P_Queue *ready, Process* ps, int pid, int time, Sch_Alg sch_alg, int time_quantum);
void Check_Preemption(Report *r, P_Queue *ready, Process *ps, int *pid_in_cpu, Sch_Alg sch_alg);
void Scheduler_Subroutine(Report *r, P_Queue* arrival, P_Queue* ready, P_Queue* wait, Process* ps, int n_process, Sch_Alg sch_alg, int time_quantum);
Report Scheduler(Process* ps, int n_process, Sch_Alg sch_alg, int time_quantum);
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

// debugging
void Debug_Print_Processes(Process* ps, int n_process);
void Debug_Print_P_Queue(P_Queue pq);
void Debug_Print_Chart_Node(Chart_Node n, int t);
void Debug_Print_Report(Report r);
void Debug_Print_Gantt_Chart(Gantt g);
void Debug_Print_Record(Report r, int n_process);

#endif