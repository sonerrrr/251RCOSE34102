#include "myheader.h"

Gantt Create_Gantt_Chart(Report r){
    Gantt gantt;
    gantt.chart = malloc(sizeof(Gantt_Node) * MAX_GANTT_LENGTH);
    gantt.size = 0;

    int pos = 0;
    for(int i=0; i<r.total_time - 1; i++){
        int p1 = r.chart[i].pid_in_cpu, p2 = r.chart[i+1].pid_in_cpu;

        if(p1 == -1 && p2 != -1){
            gantt.chart[gantt.size].pid = p2;
            gantt.chart[gantt.size].start = i+1;
        }
        else if(p1 != -1 && p2 == -1){
            gantt.chart[gantt.size].end = i+1;
            gantt.size += 1;
        }
        else if(p1 != p2){
            gantt.chart[gantt.size].end = i+1;
            gantt.size += 1;
            gantt.chart[gantt.size].pid = p2;
            gantt.chart[gantt.size].start = i+1;
        }
    }

    return gantt;
}

void Release_Gantt_Chart(Gantt *g){
    free(g->chart);
}