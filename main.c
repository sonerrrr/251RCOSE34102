#include "myheader.h"

void main(){
    int d;

    while(true){
        system("clear");

        printf("CPU Scheduling simulator\n\n1. Individual test: playaround with one scenario.\n2. Multiple test: get statistical data from multiple experiment.\n");
        d = Select_Number("Select mode: ", 1, 2);
        if(d == -1){
            if(Select_Again()) continue;
            else return;
        }
        else break;
    }
    switch(d){
        case 1:
            Individual_Test();
            break;
        case 2:
            Multiple_Test();
            break;
    }
}