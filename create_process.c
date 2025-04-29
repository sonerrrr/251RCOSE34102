#include "myheader.h"

// diverse configuration with process type
const int CPU_BURST_RANGE[3] = {30, 10, 5}; 
const int IO_BURST_RANGE[3] = {0, 10, 30};
const int IO_TIME_LOWER_RANGE[3] = {0, 1, 3};
const int IO_TIME_UPPER_RANGE[3] = {0, 2, 5};
// composition scenario of p_types 
const int P_TYPE_DIST[3][3] = {{100, 0, 0}, {50, 50, 0}, {0, 50, 50}};

// generate p_type with given scenario
P_Type Generate_P_Type(const int* dist){
    try_and_abort(((dist[0] == 0 && dist[1] == 0 && dist[2] == 0) || dist[0] < 0 || dist[1] < 0 || dist[2] < 0),
    "Generate_P_Type(): invalid scenario");

    int d = rand_int(0, (dist[0] + dist[1] + dist[2]) - 1);
    if (d < dist[0]) return CPU_BOUND;
    else if (d < dist[0] + dist[1]) return MIXED;
    else return IO_BOUND;
}

// generate process with given p_type, pid
Process Generate_Process(P_Type p_type, int _pid){
    Process p;

    p.pid = _pid;
    p.io_time = rand_int(IO_TIME_LOWER_RANGE[p_type], IO_TIME_UPPER_RANGE[p_type]);
    p.arrival_time = rand_int(0, ARRIVAL_RANGE);
    p.priority = rand_int(0, PRIORITY_RANGE);

    // generate burst times
    int burst_length = (p.io_time * 2) + 1;
    p.bursts = malloc(sizeof(int) * burst_length);
    for (int i=0; i<burst_length; i++){
        // alternating CPU burst time and IO burst time
        if(i%2 == 0) p.bursts[i] = rand_int(1, CPU_BURST_RANGE[p_type]);
        else p.bursts[i] = rand_int(1, IO_BURST_RANGE[p_type]);
    }

    return p;
}

// generate multiple processes with p_type composition scenario
Process* Generate_Process_List(const int* p_type_dist, int n_process, int random_seed){
    try_and_abort((n_process < 0), "Generate_Process_List(): n_process < 0");

    Process* ps = malloc(sizeof(Process) * n_process);
    
    srand(random_seed);
    for(int i=0; i<n_process; i++){
        P_Type p_type = Generate_P_Type(p_type_dist);
        ps[i] = Generate_Process(p_type, i);
    }
    return ps;
}

/*Process Deep_Copy_Process(Process from){
    Process p;

    p.pid = from.pid;
    p.io_time = from.io_time;
    p.arrival_time = from.arrival_time;
    p.priority = from.priority;
    int burst_length = (p.io_time * 2) + 1;
    p.bursts = malloc(sizeof(int) * burst_length);
    for (int i=0; i<burst_length; i++) { p.bursts[i] = from.bursts[i];}

    return p;
}

Process* Deep_Copy_Process_List(Process* from, int n_process){
    Process* ps;

    ps = malloc(sizeof(Process)* n_process);
    for(int i=0; i<n_process; i++){ ps[i] = Deep_Copy_Process(from[i]); }

    return ps;
}*/

// free all memory allocation during process generation
void Free_All_Processes(Process* ps, int n_process){
    for(int i=0; i<n_process; i++){
        free(ps[i].bursts);
    }
    free(ps);
}