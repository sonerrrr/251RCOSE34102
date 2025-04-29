#include "myheader.h"

// Get CPU or IO burst time
int Get_Burst_Time(Report *r, Process p){
    int _pid = p.pid;
    int burst_time = p.bursts[r->record[_pid].progress];
    return burst_time;
}

// move to next burst time
bool Progress_Burst(Report *r, Process p){
    int _pid = p.pid;
    int burst_length = p.io_time*2 + 1;
    if(r->record[_pid].progress >= burst_length - 1) return false; // last burst
    else{
        r->record[_pid].progress++;
        return true;
    }
}

// increment waiting time of processes waiting in ready queue (not (I/O) wait queue)
void Increment_Waiting_Time(Report *r, P_Queue *ready){
    for(int i=1; i<ready->last; i++){
        int _pid = ready->data[i].p.pid;
        r->record[_pid].waiting++;
    }
}

// capture current situation
Chart_Node Capture(P_Queue* arrival, P_Queue* ready, P_Queue* wait){
    Chart_Node n;

    n.size_arrived = arrival -> last;
    n.size_ready = ready -> last;
    n.size_wait = wait -> last;
    n.pid_arrived = malloc(sizeof(int)* (n.size_arrived));
    n.pid_ready = malloc(sizeof(int) * (n.size_ready));
    n.pid_wait = malloc(sizeof(int) * (n.size_wait));
    for(int i=0; i<n.size_arrived; i++) { n.pid_arrived[i] = arrival->data[i].p.pid; }
    for(int i=0; i<n.size_ready; i++) { n.pid_ready[i] = ready->data[i].p.pid; }
    for(int i=0; i<n.size_wait; i++) { n.pid_wait[i] = wait->data[i].p.pid; }

    return n;
}

void Scheduler_Subroutine(Report *r, P_Queue* arrival, P_Queue* ready, P_Queue* wait, Process* ps, int n_process){
    int time = 0, terminated = 0;

    while(terminated < n_process && time < MAX_SIMULATION_TIME){
        Process from_arrival, from_ready, from_wait;
        bool isReady_popped = false, isWait_popped = false;
        
        // migrate from arrival to ready (condsider simulataneously arrived processes) 
        while(!PQ_isEmpty(arrival) && arrival->data[0].key == time){
            from_arrival = PQ_Pop(arrival);
            PQ_Push(ready, from_arrival, time, Get_Burst_Time(r, from_arrival));
        }
        
        // check each queue for migration
        if(!PQ_isEmpty(ready) && ready->data[0].lasting == 0) {from_ready = PQ_Pop(ready); isReady_popped = true;}
        if(!PQ_isEmpty(wait) && wait->data[0].lasting == 0) {from_wait = PQ_Pop(wait); isWait_popped = true;}

        // process migration
        if(isReady_popped){
            if(Progress_Burst(r, from_ready)) PQ_Push(wait, from_ready, time, Get_Burst_Time(r, from_ready));
            else{
                r->record[from_ready.pid].turnaround += time;
                terminated++;
            }
        }
        if(isWait_popped){
            Progress_Burst(r, from_wait);
            PQ_Push(ready, from_wait, time, Get_Burst_Time(r, from_wait));
        }

        // capture current situation
        r->chart[time] = Capture(arrival, ready, wait);

        // progress time
        Increment_Waiting_Time(r, ready);
        ready->data[0].lasting--;
        wait->data[0].lasting--;
        time++;
    }
    r->total_time = time;
}

Report Scheduler(Process* ps, int n_process){
    Report r;
    // arrival: presenting arrival of each process
    P_Queue arrival, ready, wait;

    // initialize report & queues
    r.total_time = 0;
    r.chart = malloc(sizeof(Chart_Node)*MAX_CHART_LENGTH);
    r.record = malloc(sizeof(Record_Node)*n_process);
    for(int i=0; i<n_process; i++) {
        r.record[i].progress = 0;
        r.record[i].turnaround = -ps[i].arrival_time;
        r.record[i].waiting = 0;
    }
    arrival = PQ_Create(n_process);
    ready = PQ_Create(n_process);
    wait = PQ_Create(n_process);

    // generate arrival queue
    for(int i=0; i<n_process; i++){ PQ_Push(&arrival, ps[i], ps[i].arrival_time, 0); }

    // simulation
    Scheduler_Subroutine(&r, &arrival, &ready, &wait, ps, n_process);
    
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