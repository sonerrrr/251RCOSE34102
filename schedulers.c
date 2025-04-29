#include "myheader.h"

// increment waiting time of processes waiting in ready queue (not (I/O) wait queue)
void Increment_Waiting_Time(Report *r, P_Queue *ready){
    for(int i=0; i<ready->last; i++){
        int pid = ready->data[i].pid;
        r->record[pid].waiting++;
    }
}

// capture current situation
Chart_Node Capture(P_Queue* arrival, P_Queue* ready, P_Queue* wait, int pid_in_cpu, int pid_in_io){
    Chart_Node n;

    n.size_arrived = arrival -> last;
    n.size_ready = ready -> last;
    n.size_wait = wait -> last;
    n.pid_arrived = malloc(sizeof(int)* (n.size_arrived));
    n.pid_ready = malloc(sizeof(int) * (n.size_ready));
    n.pid_wait = malloc(sizeof(int) * (n.size_wait));
    for(int i=0; i<n.size_arrived; i++) { n.pid_arrived[i] = arrival->data[i].pid; }
    for(int i=0; i<n.size_ready; i++) { n.pid_ready[i] = ready->data[i].pid; }
    for(int i=0; i<n.size_wait; i++) { n.pid_wait[i] = wait->data[i].pid; }
    n.pid_in_cpu = pid_in_cpu;
    n.pid_in_io = pid_in_io;

    return n;
}

// select key and session_remained value according to scheduling algorithm
void Push_Ready_Queue(Report *r, P_Queue *ready, Process* ps, int pid, int time, Sch_Alg sch_alg, int time_quantum){
    switch(sch_alg){
        case FCFS:
            PQ_Push(ready, pid, time);
            break;
        case SJF_NONPREMP:
        case SJF_PREMP:
            PQ_Push(ready, pid, r->record[pid].burst_remained);
            break;
        case PRIORITY_NONPREMP:
        case PRIORITY_PREMP:
            PQ_Push(ready, pid, ps[pid].priority);
            break;
        case RR:
            PQ_Push(ready, pid, time);
            break;
    }

    switch(sch_alg){
        case RR:
            r->record[pid].session_remained = min(r->record[pid].burst_remained, time_quantum);
            return;
        default:
            r->record[pid].session_remained = r->record[pid].burst_remained;
            return;
    }
}

// check and perform preemption
void Check_Preemption(Report *r, P_Queue *ready, Process *ps, int *pid_in_cpu, Sch_Alg sch_alg){
    if(*pid_in_cpu == -1 || PQ_isEmpty(ready)) return;

    // p1: process running on CPU, p2: process on the top of ready queue
    int p1 = *pid_in_cpu;
    int p2 = ready->data[0].pid;

    if(sch_alg == SJF_PREMP){
        if(r->record[p1].burst_remained > r->record[p2].burst_remained){
            PQ_Pop(ready);
            *pid_in_cpu = p2;
            r->record[p2].session_remained = r->record[p2].burst_remained;
            PQ_Push(ready, p1, r->record[p1].burst_remained);
        }
    }

    if(sch_alg == PRIORITY_PREMP){
        if(ps[p1].priority > ps[p2].priority){
            PQ_Pop(ready);
            *pid_in_cpu = p2;
            r->record[p2].session_remained = r->record[p2].burst_remained;
            PQ_Push(ready, p1, ps[p1].priority);
        }
    }
}

void Scheduler_Subroutine(Report *r, P_Queue* arrival, P_Queue* ready, P_Queue* wait, Process* ps, int n_process, Sch_Alg sch_alg, int time_quantum){
    int time = 0, terminated = 0;
    int pid_in_cpu = -1, pid_in_io = -1;

    while(terminated < n_process && time < MAX_SIMULATION_TIME){
        int arrived_pid;
        bool isReady_popped = false, isWait_popped = false;
        
        // migration from arrival to ready (condsider simulataneously arrived processes) 
        while(!PQ_isEmpty(arrival) && arrival->data[0].key == time){
            arrived_pid = PQ_Pop(arrival);
            Push_Ready_Queue(r, ready, ps, arrived_pid, time, sch_alg, time_quantum);
            Check_Preemption(r, ready, ps, &pid_in_cpu, sch_alg);
        }

        // check migration of now executing process
        if(pid_in_cpu != -1 && r->record[pid_in_cpu].session_remained == 0){
            // cpu burst ends
            if(r->record[pid_in_cpu].burst_remained == 0){
                int bs_length = ( ps[pid_in_cpu].io_time * 2 ) + 1;
                // last burst ends: terminate
                if(r->record[pid_in_cpu].progress >= bs_length - 1){
                    r->record[pid_in_cpu].turnaround += time;
                    terminated++;
                }
                // moves to I/O burst
                else { 
                    int progress = ++r->record[pid_in_cpu].progress;
                    int burst = ps[pid_in_cpu].bursts[progress];

                    r->record[pid_in_cpu].burst_remained = burst;
                    r->record[pid_in_cpu].session_remained = burst;
                    PQ_Push(wait, pid_in_cpu, time);
                }
            }
            // session ends: push to ready queue again
            else Push_Ready_Queue(r, ready, ps, pid_in_cpu, time, sch_alg, time_quantum);
            pid_in_cpu = -1;
        }

        // check migration of now I/O operating process
        // I/O operation: FCFS only
        if(pid_in_io != -1 && r->record[pid_in_io].session_remained == 0){
            int progress = ++r->record[pid_in_io].progress;
            int burst = ps[pid_in_io].bursts[progress];

            r->record[pid_in_io].burst_remained = burst;
            Push_Ready_Queue(r, ready, ps, pid_in_io, time, sch_alg, time_quantum);
            Check_Preemption(r, ready, ps, &pid_in_cpu, sch_alg);
            pid_in_io = -1;
        }

        // migrate from ready/wait queue
        if(pid_in_cpu == -1 && !PQ_isEmpty(ready)) pid_in_cpu = PQ_Pop(ready);
        if(pid_in_io == -1 && !PQ_isEmpty(wait)) pid_in_io = PQ_Pop(wait);

        // capture current situation
        r->chart[time] = Capture(arrival, ready, wait, pid_in_cpu, pid_in_io);

        // time progress
        if(pid_in_cpu != -1){
            r->record[pid_in_cpu].burst_remained--;
            r->record[pid_in_cpu].session_remained--;
        }
        if(pid_in_io != -1) {
            r->record[pid_in_io].burst_remained--;
            r->record[pid_in_io].session_remained--;
        }
        Increment_Waiting_Time(r, ready);
        time++;
    }
    r->total_time = time;
}

Report Scheduler(Process* ps, int n_process, Sch_Alg sch_alg, int time_quantum){
    Report r;
    // arrival: presenting arrival of each process
    P_Queue arrival, ready, wait;

    // initialize report & queues
    r.total_time = 0;
    r.chart = malloc(sizeof(Chart_Node)*MAX_CHART_LENGTH);
    r.record = malloc(sizeof(Record_Node)*n_process);
    for(int i=0; i<n_process; i++) {
        r.record[i].progress = 0;
        r.record[i].burst_remained = ps[i].bursts[0];
        r.record[i].session_remained = 0;
        r.record[i].turnaround = -ps[i].arrival_time;
        r.record[i].waiting = 0;
    }
    arrival = PQ_Create(n_process);
    ready = PQ_Create(n_process);
    wait = PQ_Create(n_process);

    // generate arrival queue
    for(int i=0; i<n_process; i++){ PQ_Push(&arrival, i, ps[i].arrival_time); }

    // simulation
    Scheduler_Subroutine(&r, &arrival, &ready, &wait, ps, n_process, sch_alg, time_quantum);
    
    // release memory
    PQ_Delete(&arrival);
    PQ_Delete(&ready);
    PQ_Delete(&wait);

    return r;
}

void Release_Report(Report *r){
    for(int i=0; i<r->total_time; i++){
        free(r->chart[i].pid_arrived);
        free(r->chart[i].pid_ready);
        free(r->chart[i].pid_wait);
    }
    free(r->chart);
    free(r->record);
}