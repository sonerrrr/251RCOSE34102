#include "myheader.h"

void main(){
    int n_process = 10;

    Process* ps = Generate_Process_List(P_TYPE_DIST[1], n_process, time(NULL));
    Report r = Scheduler(ps, n_process, PRIORITY_PREMP, 5);
    Gantt g = Create_Gantt_Chart(r);
    Debug_Print_Processes(ps, n_process);
    //Debug_Print_Report(r);
    Debug_Print_Gantt_Chart(g);
    Debug_Print_Record(r, n_process);
    
    Release_Gantt_Chart(&g);
    Release_Report(&r);
    Free_All_Processes(ps, n_process);
}