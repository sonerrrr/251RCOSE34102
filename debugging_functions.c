#include "myheader.h"

// Debugging functions
// Print Processes
void Debug_Print_Processes(Process_List pl){
    for(int i=0; i<pl.n_process; i++){
        Process p = pl.p_list[i];
        printf("pid: %d | io_time: %d | arrival_time: %d | priority: %d | bursts: ", p.pid, p.io_time, p.arrival_time, p.priority);
        for(int j=0; j < (p.io_time) * 2 + 1; j++){printf("%d ", p.bursts[j]);}
        printf("\n");
    }
}

void Debug_Print_P_Queue(P_Queue pq){
    for(int i=0; i<pq.size; i++){
        printf("[Node %d] pid: %d | key: %d\n", i, pq.data[i].pid, pq.data[i].key);
    }
}

void Debug_Print_Chart_Node(Chart_Node n, int t){
    printf("[Time %d] Arrive: ", t);
    for(int i=0; i<n.size_arrived; i++) printf("%d ", n.pid_arrived[i]);
    printf("| CPU: %d (%2d) | Ready: ", n.pid_in_cpu, n.cpu_burst_left);
    for(int i=0; i<n.size_ready; i++) printf("%d ", n.pid_ready[i]);
    printf("| I/O: %d (%2d) | Wait: ", n.pid_in_io, n.io_burst_left);
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
    printf("Report:\nTotal time: %d\nRecord:\n", r.total_time);
    printf("            pid  ");
    for(int i=0; i<n_process; i++) printf("| %3d ", i);
    printf("\nTurnaround time  ");
    for(int i=0; i<n_process; i++) printf("| %3d ", r.record[i].turnaround);
    printf("\nWaiting time     ");
    for(int i=0; i<n_process; i++) printf("| %3d ", r.record[i].waiting);
    printf("\n");
}