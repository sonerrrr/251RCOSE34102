#include "myheader.h"

// exception handler
void try_and_abort(bool cond, char* message){
    if(cond){
        perror(message);
        abort();
    }
}

// generate random integer
int rand_int(int min, int max){
    try_and_abort((min > max), "rand_int(): min > max");
    return (rand()%(max - min + 1))+ min;
}

// get parent, child index of full binary tree: starting from index 0
int get_parent(int i) {return (i + 1) / 2 - 1;}
int get_first_child(int i) {return (i + 1) * 2 - 1;}