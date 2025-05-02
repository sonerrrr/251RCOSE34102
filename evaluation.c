#include "myheader.h"

void Open_Node(Gantt *g, int pid, int time){
    g->chart[g->size].pid = pid;
    g->chart[g->size].start = time;
}

void Close_Node(Gantt *g, int time){
    g->chart[g->size].end = time;
    g->size += 1;
}

Gantt Create_Gantt_Chart(Report r){
    Gantt g;
    g.chart = malloc(sizeof(Gantt_Node) * MAX_GANTT_LENGTH);
    g.size = 0;

    if(r.chart[0].pid_in_cpu != -1) Open_Node(&g, 0, 0);
    for(int i=1; i<r.total_time - 1; i++){
        int p1 = r.chart[i].pid_in_cpu, p2 = r.chart[i+1].pid_in_cpu;

        if(p1 == -1 && p2 != -1) Open_Node(&g, p2, i+1);
        else if(p1 != -1 && p2 == -1) Close_Node(&g, i+1);
        else if(p1 != p2){
            Close_Node(&g, i+1);
            Open_Node(&g, p2, i+1);
        }
    }

    return g;
}

// analysis of each data (turnaround time, waiting time)
void Generate_Analysis_Item(Analysis_Item *ai, int *data, Report r){
    int* count = malloc(sizeof(int) * (PRIORITY_RANGE + 1));
    ai->avg_clf = malloc(sizeof(double) * (PRIORITY_RANGE + 1));
    for(int pri = 0; pri < PRIORITY_RANGE + 1; pri++){
        count[pri] = 0;
        ai->avg_clf[pri] = 0;
    }
    ai->avg = 0; ai->std = 0;

    for(int i=0; i<r.n_process; i++){
        int pri = r.record[i].priority;

        ai->avg_clf[pri] += data[i];
        count[pri]++;

        ai->std += data[i]*data[i];
    }

    for(int pri = 0; pri < PRIORITY_RANGE + 1; pri++){ ai->avg += ai->avg_clf[pri]; }

    for(int pri = 0; pri < PRIORITY_RANGE + 1; pri++){
        if(count[pri] == 0) ai->avg_clf[pri] = -1;
        else ai->avg_clf[pri] /= count[pri];
    }
    ai->avg /= r.n_process;

    ai->std = pow((ai->std / r.n_process) - (ai->avg)*(ai->avg), 0.5);

    free(count);
}

// analysis of given report
Analysis Analyze(Report r){
    Analysis a;
    a.n_process = r.n_process;
    int* tas = malloc(sizeof(int) * r.n_process);
    int* ws = malloc(sizeof(int) * r.n_process);

    for(int i=0; i<r.n_process; i++){
        tas[i] = r.record[i].turnaround;
        ws[i] = r.record[i].waiting;
    }

    Generate_Analysis_Item(&a.turnaround, tas, r);
    Generate_Analysis_Item(&a.waiting, ws, r);

    free(tas);
    free(ws);

    return a;
}

void Release_Gantt_Chart(Gantt *g){
    free(g->chart);
}

void Release_Analysis(Analysis *a){
    free(a->turnaround.avg_clf);
    free(a->waiting.avg_clf);
}