#include <stdlib.h>
#include "pcb_s.h"

void pcb_init(pcb_t *pcb, int pid, int current_prio){
   pcb->pid = pid;
   pcb->current_priority = current_prio;
   pcb->status = PR_UNDEFINED;
}

void pcb_destroy(pcb_t *pcb){
    free(pcb);
}

int memory_map_init(struct memory_map *map, uint32_t data, uint32_t code, uint32_t pgb){
    map->data = data;
    map->code = code;
    map->pgb = pgb;
    return 0;
}

void cpu_snapshot_init(struct cpu_snapshot *snapshot, uint32_t PC){
    snapshot->PC = PC;
    snapshot->IR = 0;
    snapshot->cc = 0;
    for(int i = 0; i < CPU_REGISTERS; i++){
        snapshot->R[i] = 0;
    }
}


