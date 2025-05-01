#include "myheader.h"

char* p_dist_type_s[] = {"CPU-bound", "Mixed", "I/O-bound"};
char* sch_alg_s[] = {"FCFS", "Nonpreemptive SJF", "Preemptive SJF", "Nonpreemptive Priority-based", "Preemptive Priority-based", "RR"};

void Print_Strings (char* strings[], int n){
    for(int i=0; i<n; i++) printf("%d. %s\n", i+1, strings[i]);
}

bool Select_Again(){
    char c;

    printf("You've selected number beyond scope. Input y if you want to continue: ");
    scanf(" %c", &c);
    if(c == 'y') return true;
    else return false;
}

bool Select_Termination(){
    char c;

    printf("Input y if you want to terminate: ");
    scanf(" %c", &c);
    if(c == 'y') return true;
    else return false;
}

void Press_ENTER_to_continue(){
    int t;

    printf("Press [Enter] to continue");
    while((t = getchar()) != '\n' && t != EOF); // flush input buffer
    getchar();
}

int Select_Number(char* message, int m, int M){
    int d;

    while(true){
        printf("%s", message);
        scanf(" %d", &d);
        if(d < m || d > M){
            if(Select_Again()) continue;
            else return -1;
        }
        return d;
    }
}

int Select_P_Dist_Type(){
    int d;

    while(true){
        Print_Strings(p_dist_type_s, 3);
        printf("Select distribution type of processes: ");
        scanf(" %d", &d);
        if(d < 1 || d > 3){
            if(Select_Again()) continue;
            else return -1;
        }
        return d - 1;
    }
}


int Select_Scheduling_Algorithm(){
    int d;

    while(true){
        Print_Strings(sch_alg_s, N_ALGORITHM);
        printf("Select distribution type of processes: ");
        scanf(" %d", &d);
        if(d < 1 || d > N_ALGORITHM){
            if(Select_Again()) continue;
            else return -1;
        }
        return d - 1;
    }
}

void Page_Select_Print(Report r, Gantt g, bool isGantt){
    char c;
    const int line_per_page = 40;
    int max_page;
    int now_page = 0;
                
    if(isGantt) max_page = g.size / line_per_page;
    else max_page = r.total_time / line_per_page;

    while(true){
        bool escape = false;

        system("clear");
        if(isGantt) Display_Gantt_Chart(g, now_page*line_per_page, min((now_page + 1)*line_per_page, g.size));
        else Display_Chart(r, now_page*line_per_page, min((now_page + 1)*line_per_page, r.total_time));
        printf("[ Previous Page: a | Next Page: d | Quit: q ]");
        scanf(" %c", &c);
        switch(c){
            case 'a':
                if(now_page > 0) now_page--;
                break;
            case 'd':
                if(now_page < max_page) now_page++;
                break;
            case 'q':
                escape = true;
                break;
            }
        if(escape) break;
    }
}

int Individual_Test(){
    system("clear");

    char c;
    int p_dist_type, n_process, sch_alg;
    int time_quantum = 0;

    p_dist_type = Select_P_Dist_Type();
    if(p_dist_type == -1) return -1;
    n_process = Select_Number("Input number of processes to generate (1 ~ 50): ", 1, 50);
    if(n_process == -1) return -1;
    sch_alg = Select_Scheduling_Algorithm();
    if(sch_alg == -1) return -1;
    if(sch_alg == RR){
        time_quantum = Select_Number("RR scheduling: Input size of time quantum (3 ~ 50): ", 3, 50);
        if(time_quantum == -1) return -1;
    }

    Process_List pl = Generate_Process_List(P_TYPE_DIST[p_dist_type], n_process, time(NULL));
    printf("process generated\n");
    Report r = Scheduler(pl, (Sch_Alg) sch_alg, time_quantum, true);
    printf("shceduling simulation done\n");
    Gantt g = Create_Gantt_Chart(r);
    printf("gantt chart generated\n");
    Analysis a = Analyze(r);
    printf("report analyzed\n");
    Press_ENTER_to_continue();
    while(true){
        int d;

        system("clear");
        printf("now scheduling algorithm: %s", sch_alg_s[sch_alg]);
        if(sch_alg == RR) printf("(time quantum: %d)", time_quantum);
        printf("\n1. View process list\n2. View time-serial report\n3. View Gantt chart\n4. View turnaround time and wating time statistics");
        printf("\n5. View summarized analysis\n6. Start new simulation with the same list of processes\n7. Terminate\n");
        printf("Input number of menu you want to proceed (anything else to terminate): ");
        scanf(" %d", &d);
 
        system("clear");
        switch(d){
            case 1:
                Display_Processes(pl);
                Press_ENTER_to_continue();
                break;
            case 2:
                Page_Select_Print(r, g, false);
                break;
            case 3:
                Page_Select_Print(r, g, true);
                break;
            case 4:
                Display_Record(r);
                Press_ENTER_to_continue();
                break;
            case 5:
                Display_Analysis(a);
                Press_ENTER_to_continue();
                break;
            case 6:
                sch_alg = Select_Scheduling_Algorithm();
                if(sch_alg == -1) return -1;
                if(sch_alg == RR){
                    time_quantum = Select_Number("RR scheduling: Input size of time quantum (3 ~ 50): ", 3, 50);
                    if(time_quantum == -1) return -1;
                }

                Release_Report(&r, true);
                Release_Gantt_Chart(&g);
                Release_Analysis(&a);
                printf("successfully release resoure of previous simualtion\n");
                r = Scheduler(pl, (Sch_Alg) sch_alg, time_quantum, true);
                printf("shceduling simulation done\n");
                g = Create_Gantt_Chart(r);
                printf("gantt chart generated\n");
                a = Analyze(r);
                printf("report analyzed\n");
                Press_ENTER_to_continue();
                break;
            case 7:
                return 0;
            default:
                if(Select_Termination()) return 0;
                break;
        }
    }

    Release_Report(&r, true);
    Release_Gantt_Chart(&g);
    Release_Analysis(&a);
    Release_Process_List(&pl);
}

FILE* Initialize_Fp(char* fname){
    FILE *fp = fopen(fname, "w");

    try_and_abort(fp==NULL, "file open error");
    fprintf(fp, "Attempt");
    for(int i=0; i<PRIORITY_RANGE+1; i++) fprintf(fp, ", avg[%d]", i);
    fprintf(fp, ", avg, std\n");

    return fp;
}

void Print_File(FILE *fp, Analysis_Item ai, int index){
    fprintf(fp, "%d", index);
    for(int i=0; i<PRIORITY_RANGE+1; i++) fprintf(fp, ", %lf", ai.avg_clf[i]);
    fprintf(fp, ", %lf, %lf\n", ai.avg, ai.std);
}

void Create_File_Directory(char *fname, char* dir_name, int sch_alg, bool isTa){
    strcat(fname, "./"); strcat(fname, dir_name); strcat(fname, "/"); strcat(fname, sch_alg_s[sch_alg]);
    if(isTa) strcat(fname, "_ta.csv"); else strcat(fname, "_w.csv");
}

int Multiple_Test(){
    int p_dist_type, n_process, n_iteration, time_quantum;
    char dir_name[100];
    FILE **fps;
    system("clear");

    p_dist_type = Select_P_Dist_Type();
    if(p_dist_type == -1) return -1;
    n_process = Select_Number("Input number of processes to generate (1 ~ 50): ", 1, 50);
    if(n_process == -1) return -1;
    n_iteration = Select_Number("Input number of iteration (1 ~ 100): ", 1, 100);
    if(n_iteration == -1) return -1;
    time_quantum = Select_Number("RR scheduling: Input size of time quantum (3 ~ 50): ", 3, 50);

    printf("Input directory name you want to save test result: ");
    scanf(" %s", dir_name);
    try_and_abort(mkdir(dir_name, 0755) != 0, "mkdir failed");
    fps = malloc(sizeof(FILE*) * N_ALGORITHM * 2);
    for(int sch_alg = 0; sch_alg < N_ALGORITHM; sch_alg++){
        char fname1[100] = "", fname2[100] = "";
        Create_File_Directory(fname1, dir_name, sch_alg, true);
        fps[2*sch_alg] = Initialize_Fp(fname1);

        Create_File_Directory(fname2, dir_name, sch_alg, false);
        fps[2*sch_alg + 1] = Initialize_Fp(fname2);
    }

    for(int i=0; i<n_iteration; i++){
        Process_List pl = Generate_Process_List(P_TYPE_DIST[p_dist_type], n_process, time(NULL)+i);

        for(int sch_alg=0; sch_alg<N_ALGORITHM; sch_alg++){
            Report r = Scheduler(pl, (Sch_Alg)sch_alg, time_quantum, false);
            Analysis a = Analyze(r);
            
            Print_File(fps[2*sch_alg], a.turnaround, i);
            Print_File(fps[2*sch_alg + 1], a.waiting, i);

            Release_Report(&r, false);
            Release_Analysis(&a);
        }

        Release_Process_List(&pl);
        if((i+1)%10 == 0) printf("[Iteration %d] complete\n", i+1);
    }

    for(int i=0; i<N_ALGORITHM*2; i++) fclose(fps[i]);
    free(fps);

    return 0;
}