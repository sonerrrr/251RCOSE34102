#ifndef MYHEADER_H
#define MYHEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// general functions
void try_and_abort(bool, char*);
int rand_int(int, int);
int get_parent(int);
int get_first_child(int);
int min(int, int);

// processes
// composition scenario of p_types 
extern const int PRIORITY_RANGE;
extern const int P_TYPE_DIST[3][3];

typedef struct __Process {
    int pid;
    int *bursts; // CPU burst, I/O burst time alternating
    int io_time; // number of I/O burst time
    int arrival_time;
    int priority;
} Process;

typedef struct __Process_list {
    int n_process;
    Process* p_list;
} Process_List;

Process_List Generate_Process_List(const int* p_type_dist, int n_process, int random_seed);
void Release_Process_List(Process_List *pl);

// priority queue
// each node of P_Queue.data
typedef struct __PQ_Node{
    int pid;
    int key[3];
} PQ_Node;

typedef struct __P_Queue{
    int size;
    int last; // positiion where to push new element
    PQ_Node *data;
} P_Queue;

P_Queue PQ_Create(int n_process);
void PQ_Delete(P_Queue *pq);
void PQ_Push(P_Queue *pq, int _pid, int* _key);
int PQ_Pop(P_Queue *pq);
bool PQ_isEmpty(P_Queue pq);

// scheduler
#define MAX_CHART_LENGTH 10000
#define MAX_SIMULATION_TIME 10000
#define N_ALGORITHM 6

typedef enum _Sch_Alg {
    FCFS = 0,
    SJF_NONPREMP = 1,
    SJF_PREMP = 2,
    PRIORITY_NONPREMP = 3,
    PRIORITY_PREMP = 4,
    RR = 5
} Sch_Alg;

// Time-serial chart
typedef struct __Chart_Node{
    int *pid_arrived, *pid_ready, *pid_wait;
    int size_arrived, size_ready, size_wait;
    int pid_in_cpu, pid_in_io;
    int cpu_burst_remained, io_burst_remained;
} Chart_Node;

// PCB
typedef struct __Record_Node{
    int priority; // priority
    int progress; // indicate current burst
    int burst_remained; // current burst time remained
    int session_remained; // current session remained
    int turnaround; // turnaround time
    int waiting; // waiting time
} Record_Node;

typedef struct __Report{
    int total_time;
    int n_process;
    Chart_Node *chart;
    Record_Node *record;
}Report;

Report Schedule(Process_List pl, Sch_Alg sch_alg, int time_quantum, bool need_capture);
void Release_Report(Report *r, bool need_capture);

// evaluation
#define MAX_GANTT_LENGTH 10000

// Gantt chart
typedef struct __Gantt_Node{
    int pid, start, end;
} Gantt_Node;

typedef struct __Gantt{
    int size;
    Gantt_Node *chart;
} Gantt;

typedef struct _Analysis_Item{
    double *avg_clf;
    double avg;
    double std;
} Analysis_Item;

typedef struct _Analysis{
    int n_process;
    Analysis_Item turnaround;
    Analysis_Item waiting;
} Analysis;

Gantt Create_Gantt_Chart(Report r);
Analysis Analyze(Report r);
void Release_Gantt_Chart(Gantt *g);
void Release_Analysis(Analysis *a);

// display function
void Display_Processes(Process_List pl);
void Display_P_Queue(P_Queue pq);
void Display_Chart_Node(Chart_Node n, int t);
void Display_Chart(Report r, int m, int M);
void Display_Gantt_Chart(Gantt g, int m, int M);
void Display_Record(Report r);
void Display_Analysis_item(Analysis_Item ai);
void Display_Analysis(Analysis a);

// interface
bool Select_Again();
int Select_Number(char* message, int m, int M);
int Individual_Test();
int Multiple_Test();
#endif