#include "myheader.h"

// Debugging functions
// Print Processes
void Debug_Print_Processes(Process* ps, int n_process){
    for(int i=0; i<n_process; i++){
        Process p = ps[i];
        printf("pid: %d | io_time: %d | arrival_time: %d | priority: %d | bursts: ", p.pid, p.io_time, p.arrival_time, p.priority);
        for(int j=0; j < (p.io_time) * 2 + 1; j++){printf("%d ", p.bursts[j]);}
        printf("\n");
    }
}

void Debug_Print_P_Queue(P_Queue pq){
    for(int i=0; i<pq.size; i++){
        printf("[Node %d] pid: %d | key: %d\n", i, pq.data[i].p.pid, pq.data[i].key);
    }
}

void Debug_Print_Chart_Node(Chart_Node n, int t){
    printf("[Time %d] Arrive: ", t);
    for(int i=0; i<n.size_arrived; i++) printf("%d ", n.pid_arrived[i]);
    printf("| Ready: ");
    for(int i=0; i<n.size_ready; i++) printf("%d ", n.pid_ready[i]);
    printf("| Wait: ");
    for(int i=0; i<n.size_wait; i++) printf("%d ", n.pid_wait[i]);
    printf("\n");
}

void Debug_Print_Report(Report r){
    printf("Scheduling Report:\nChart:\n");
    for(int i=0; i<r.total_time; i++){
        Debug_Print_Chart_Node(r.chart[i], i);
    }
}

void Debug_Print_Gantt_Chart(Gantt g){
    printf("Gantt chart:\n");
    for(int i=0; i<g.size; i++){
        printf("pid: %d | start: %d | end: %d\n", g.chart[i].pid, g.chart[i].start, g.chart[i].end);
    }
}

void Debug_Print_Record(Report r, int n_process){
    printf("Report - Record:\n");
    printf("            pid  ");
    for(int i=0; i<n_process; i++) printf("| %3d ", i);
    printf("\nTurnaround time  ");
    for(int i=0; i<n_process; i++) printf("| %3d ", r.record[i].turnaround);
    printf("\nWaiting time     ");
    for(int i=0; i<n_process; i++) printf("| %3d ", r.record[i].waiting);
    printf("\n");
}

void main(){
    int n_process = 5;

    Process* ps = Generate_Process_List(P_TYPE_DIST[2], n_process, time(NULL));
    Report r = Scheduler(ps, n_process);
    Gantt g = Create_Gantt_Chart(r);
    Debug_Print_Processes(ps, n_process);
    //Debug_Print_Report(r);
    Debug_Print_Gantt_Chart(g);
    Debug_Print_Record(r, n_process);
    
    Release_Gantt_Chart(&g);
    Release_Report(&r);
    Free_All_Processes(ps, n_process);
}