#include "myheader.h"

Gantt Create_Gantt_Chart(Report r){
    Gantt gantt;
    gantt.chart = malloc(sizeof(Gantt_Node) * MAX_GANTT_LENGTH);
    gantt.size = 0;

    int pos = 0;
    for(int i=0; i<r.total_time - 1; i++){
        Chart_Node c1 = r.chart[i], c2 = r.chart[i+1];

        if(c1.size_ready == 0 && c2.size_ready != 0){
            gantt.chart[gantt.size].pid = c2.pid_ready[0];
            gantt.chart[gantt.size].start = i+1;
        }
        else if(c1.size_ready != 0 && c2.size_ready == 0){
            gantt.chart[gantt.size].end = i+1;
            gantt.size += 1;
        }
        else if(c1.pid_ready[0] != c2.pid_ready[0]){
            gantt.chart[gantt.size].end = i+1;
            gantt.size += 1;
            gantt.chart[gantt.size].pid = c2.pid_ready[0];
            gantt.chart[gantt.size].start = i+1;
        }
    }

    return gantt;
}

void Release_Gantt_Chart(Gantt *g){
    free(g->chart);
}