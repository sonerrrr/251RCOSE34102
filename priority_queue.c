#include "myheader.h"

// define priority queue: min heap
P_Queue PQ_Create(int n_process){
    P_Queue pq;
    
    pq.size = n_process;
    pq.last = 0;
    pq.data = malloc(sizeof(PQ_Node) * n_process);
    return pq;
}

void PQ_Delete(P_Queue *pq){free(pq -> data);}

void PQ_Node_Deep_Copy(PQ_Node *to, PQ_Node *from){
    to -> p = from -> p;
    to -> key = from -> key;
    to -> lasting = from -> lasting;
}

void PQ_Swap(P_Queue *pq, int i1, int i2){
    PQ_Node temp;

    PQ_Node_Deep_Copy(&temp, &(pq -> data[i1]));
    PQ_Node_Deep_Copy(&(pq -> data[i1]), &(pq -> data[i2]));
    PQ_Node_Deep_Copy(&(pq -> data[i2]), &temp);
}

// push new (process, key) to priority queue
void PQ_Push(P_Queue *pq, Process _p, int _key, int _lasting){
    // full queue exception
    try_and_abort((pq -> last >= pq -> size), "PQ_push(): full queue");

    PQ_Node new_node; new_node.p = _p; new_node.key = _key; new_node.lasting = _lasting;
    int pos = pq -> last;
    pq -> data[pos] = new_node;
    while(pos > 0){
        int parent = get_parent(pos);

        if(pq -> data[pos].key < pq -> data[parent].key){
            PQ_Swap(pq, pos, parent);
            pos = parent;
        }
        else break;
    }
    (pq -> last)++;
}

// pop process from priority queue
Process PQ_Pop(P_Queue *pq){
    // empty queue: return invalid process - outside function should handle
    if (pq -> last == 0){ Process p; p.pid = -1; return p; }

    (pq -> last)--;
    PQ_Swap(pq, 0, pq -> last);
    int pos = 0;
    int last_parent = get_parent((pq->last) - 1); // data[last]: return value, data[last - 1]: last node remaining
    while(pos <= last_parent){
        int child1 = get_first_child(pos), child2 = child1 + 1;
        int min_child = child1;
        
        if(pq -> data[child1].key > pq -> data[child2].key && child2 <= pq->last - 1) min_child = child2;
        if(pq -> data[min_child].key < pq -> data[pos].key){
            PQ_Swap(pq, pos, min_child);
            pos = min_child;
        }
        else break;
    }

    return pq -> data[pq -> last].p;
}

bool PQ_isEmpty(P_Queue *pq) {return (pq->last == 0); }