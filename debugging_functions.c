#include "myheader.h"

// Debugging functions
// Print Processes
void Display_Processes(Process_List pl){
    for(int i=0; i<pl.n_process; i++){
        Process p = pl.p_list[i];
        printf("pid: %d | io_time: %d | arrival_time: %d | priority: %d | bursts: ", p.pid, p.io_time, p.arrival_time, p.priority);
        for(int j=0; j < (p.io_time) * 2 + 1; j++){printf("%d ", p.bursts[j]);}
        printf("\n");
    }
}

void Display_P_Queue(P_Queue pq){
    for(int i=0; i<pq.size; i++){
        printf("[Node %d] pid: %d | key:", i, pq.data[i].pid);
        for(int j=0; j<3; j++) printf("[%d] %d ", j, pq.data[i].key[j]);
        printf("\n");
    }
}

void Display_Chart_Node(Chart_Node n, int t){
    printf("[Time %d] Arrive: ", t);
    for(int i=0; i<n.size_arrived; i++) printf("%d ", n.pid_arrived[i]);
    printf("| CPU: %d (%2d) | Ready: ", n.pid_in_cpu, n.cpu_burst_left);
    for(int i=0; i<n.size_ready; i++) printf("%d ", n.pid_ready[i]);
    printf("| I/O: %d (%2d) | Wait: ", n.pid_in_io, n.io_burst_left);
    for(int i=0; i<n.size_wait; i++) printf("%d ", n.pid_wait[i]);
    printf("\n");
}

void Display_Chart(Report r, int m, int M){
    printf("Time-Serial Chart (time %d - %d / %d):\n", m, M - 1, r.total_time - 1);
    for(int i=m; i<M; i++){
        Display_Chart_Node(r.chart[i], i);
    }
}

void Display_Gantt_Chart(Gantt g, int m, int M){
    printf("Gantt chart (line %d - %d / %d):\n", m, M - 1, g.size - 1);
    for(int i=m; i<M; i++){
        printf("pid: %d | start: %d | end: %d\n", g.chart[i].pid, g.chart[i].start, g.chart[i].end);
    }
}

void Display_Record(Report r){
    const int p_per_line = 10;
    int max_line = (r.n_process - 1) / p_per_line + 1;

    printf("Total time: %d\n", r.total_time - 1);
    for(int i=0; i<max_line; i++){
        printf("            pid  ");
        for(int j=i*p_per_line; j<min((i+1)*p_per_line, r.n_process); j++) printf("| %3d ", j);
        printf("\nTurnaround time  ");
        for(int j=i*p_per_line; j<min((i+1)*p_per_line, r.n_process); j++) printf("| %3d ", r.record[j].turnaround);
        printf("\nWaiting time     ");
        for(int j=i*p_per_line; j<min((i+1)*p_per_line, r.n_process); j++) printf("| %3d ", r.record[j].waiting);
        printf("\n");
    }
}

void Display_Analysis_item(Analysis_Item ai){
    printf("Average (Classified by priority): ");
    for(int i=0; i<PRIORITY_RANGE + 1; i++){
        printf("[%d] %lf |", i, ai.avg_clf[i]);
    }
    printf("\nTotal average: %lf\nStandard deviation: %lf\n", ai.avg, ai.std);
}

void Display_Analysis(Analysis a){
    printf("Summarized analysis:\n");
    printf("Turnaround time:\n");
    Display_Analysis_item(a.turnaround);
    printf("Waiting time:\n");
    Display_Analysis_item(a.waiting);
}