#include "myheader.h"

// increment waiting time of processes waiting in ready queue (not (I/O) wait queue)
void Increment_Waiting_Time(Report *r, P_Queue ready){
    for(int i=0; i<ready.last; i++){
        int pid = ready.data[i].pid;
        r->record[pid].waiting++;
    }
}

// capture current situation
Chart_Node Capture(P_Queue arrival, P_Queue ready, P_Queue wait, Report r, int pid_in_cpu, int pid_in_io){
    Chart_Node n;

    n.size_arrived = arrival.last;
    n.size_ready = ready.last;
    n.size_wait = wait.last;
    n.pid_arrived = malloc(sizeof(int)* (n.size_arrived));
    n.pid_ready = malloc(sizeof(int) * (n.size_ready));
    n.pid_wait = malloc(sizeof(int) * (n.size_wait));
    for(int i=0; i<n.size_arrived; i++) { n.pid_arrived[i] = arrival.data[i].pid; }
    for(int i=0; i<n.size_ready; i++) { n.pid_ready[i] = ready.data[i].pid; }
    for(int i=0; i<n.size_wait; i++) { n.pid_wait[i] = wait.data[i].pid; }
    n.pid_in_cpu = pid_in_cpu;
    n.pid_in_io = pid_in_io;
    if(pid_in_cpu != -1) n.cpu_burst_left = r.record[pid_in_cpu].burst_remained;
    else n.cpu_burst_left = 0;
    if(pid_in_io != -1) n.io_burst_left = r.record[pid_in_io].burst_remained;
    else n.io_burst_left = 0;

    return n;
}

// select key and session_remained value according to scheduling algorithm
void Push_Ready_Queue(Report *r, P_Queue *ready, Process_List pl, int pid, int time, Sch_Alg sch_alg, int time_quantum){
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
            PQ_Push(ready, pid, pl.p_list[pid].priority);
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
void Check_Preemption(Report *r, P_Queue *ready, Process_List pl, int *pid_in_cpu, Sch_Alg sch_alg){
    if(*pid_in_cpu == -1 || PQ_isEmpty(*ready)) return;

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
        if(pl.p_list[p1].priority > pl.p_list[p2].priority){
            PQ_Pop(ready);
            *pid_in_cpu = p2;
            r->record[p2].session_remained = r->record[p2].burst_remained;
            PQ_Push(ready, p1, pl.p_list[p1].priority);
        }
    }
}

// initialize main variables
void Initialize_Scheduler(Report *r, P_Queue *arrival, P_Queue *ready, P_Queue *wait, Process_List pl){
    r->total_time = 0;
    r->chart = malloc(sizeof(Chart_Node)*MAX_CHART_LENGTH);
    r->record = malloc(sizeof(Record_Node)*pl.n_process);
    for(int i=0; i<pl.n_process; i++) {
        r->record[i].progress = 0;
        r->record[i].burst_remained = pl.p_list[i].bursts[0];
        r->record[i].session_remained = 0;
        r->record[i].turnaround = -pl.p_list[i].arrival_time;
        r->record[i].waiting = 0;
    }
    *arrival = PQ_Create(pl.n_process);
    *ready = PQ_Create(pl.n_process);
    *wait = PQ_Create(pl.n_process);
}

Report Scheduler(Process_List pl, Sch_Alg sch_alg, int time_quantum){
    Report r;
    // arrival: presenting arrival of each process
    P_Queue arrival, ready, wait;

    // initialize report & queues
    Initialize_Scheduler(&r, &arrival, &ready, &wait, pl);

    // generate arrival queue
    for(int i=0; i<pl.n_process; i++){ PQ_Push(&arrival, i, pl.p_list[i].arrival_time); }

    // main routine
    int time = 0; // current time
    int terminated = 0; // number of terminated process
    int pid_in_cpu = -1, pid_in_io = -1; // pid of now running on CPU / I/O device

    // until all process terminate or exceed simulation limit
    while(terminated < pl.n_process && time < MAX_SIMULATION_TIME){
        /* control flow:
        1) move out from CPU
        - prevent preemption of a process which has to be removed from CPU (session_remained = 0) at the first place
        - CPU -> ready queue(session expired only), wait queue(switch to I/O burst), termination(all bursts end)
        2) I/O -> ready queue(all processes should have CPU burst in the end): preemption might occur
        3) push newly arrived processes into ready queue : preemption might occur
        4) load new process to CPU and I/O if needed 
        5) capture & time progress
        */

        // check migration of now executing process
        if(pid_in_cpu != -1 && r.record[pid_in_cpu].session_remained == 0){
            // cpu burst ends
            if(r.record[pid_in_cpu].burst_remained == 0){
                int bs_length = ( pl.p_list[pid_in_cpu].io_time * 2 ) + 1;
                // last burst ends: terminate
                if(r.record[pid_in_cpu].progress >= bs_length - 1){
                    r.record[pid_in_cpu].turnaround += time;
                    terminated++;
                }
                // moves to I/O burst
                else { 
                    int progress = ++r.record[pid_in_cpu].progress;
                    int burst = pl.p_list[pid_in_cpu].bursts[progress];

                    r.record[pid_in_cpu].burst_remained = burst;
                    r.record[pid_in_cpu].session_remained = burst;
                    PQ_Push(&wait, pid_in_cpu, time);
                }
            }
            // session ends: push to ready queue again
            else Push_Ready_Queue(&r, &ready, pl, pid_in_cpu, time, sch_alg, time_quantum);
            pid_in_cpu = -1;
        }

        // check migration of now I/O operating process
        // I/O operation: FCFS only
        if(pid_in_io != -1 && r.record[pid_in_io].session_remained == 0){
            int progress = ++r.record[pid_in_io].progress;
            int burst = pl.p_list[pid_in_io].bursts[progress];

            r.record[pid_in_io].burst_remained = burst;
            Push_Ready_Queue(&r, &ready, pl, pid_in_io, time, sch_alg, time_quantum);
            Check_Preemption(&r, &ready, pl, &pid_in_cpu, sch_alg);
            pid_in_io = -1;
        }

         // migration from arrival to ready (condsider simulataneously arrived processes) 
         while(!PQ_isEmpty(arrival) && arrival.data[0].key == time){
            int arrived_pid = PQ_Pop(&arrival);
            Push_Ready_Queue(&r, &ready, pl, arrived_pid, time, sch_alg, time_quantum);
            Check_Preemption(&r, &ready, pl, &pid_in_cpu, sch_alg);
        }

        // load from ready/wait queue
        if(pid_in_cpu == -1 && !PQ_isEmpty(ready)) pid_in_cpu = PQ_Pop(&ready);
        if(pid_in_io == -1 && !PQ_isEmpty(wait)) pid_in_io = PQ_Pop(&wait);

        // capture current situation
        r.chart[time] = Capture(arrival, ready, wait, r, pid_in_cpu, pid_in_io);

        // time progress
        if(pid_in_cpu != -1){
            r.record[pid_in_cpu].burst_remained--;
            r.record[pid_in_cpu].session_remained--;
        }
        if(pid_in_io != -1) {
            r.record[pid_in_io].burst_remained--;
            r.record[pid_in_io].session_remained--;
        }
        Increment_Waiting_Time(&r, ready);
        time++;
    }
    r.total_time = time;
    
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