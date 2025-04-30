#include "myheader.h"

void main(){
    int n_process = 10;

    Process_List pl = Generate_Process_List(P_TYPE_DIST[1], n_process, time(NULL));
    Report r = Scheduler(pl, SJF_PREMP, 5);
    Gantt g = Create_Gantt_Chart(r);
    Debug_Print_Processes(pl);
    //Debug_Print_Report(r);
    Debug_Print_Gantt_Chart(g);
    Debug_Print_Record(r, n_process);
    
    Release_Gantt_Chart(&g);
    Release_Report(&r);
    Release_Process_List(&pl);
}