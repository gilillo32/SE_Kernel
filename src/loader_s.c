#include "loader.h"
#include "memory_s.h"
#include "executor.h"

#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

#define LOADER_MAX_TLB 10

char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1)+strlen(s2)+1); // +1 for the null-terminator
    if (result == NULL) {
        printf("Error: malloc failed in concat\n");
        exit(1);
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int __loader_get_valid_pid(loader_s *loader){
    int new_pid;
    new_pid = loader->prid_counter++;
    if(loader->prid_counter < 0)
        loader->prid_counter = 1;
    return new_pid;
}

int __loader_get_valid_priority(loader_s *loader){
    return(rand() % loader->max_prio);
}

int loader_init(loader_s *loader, struct physical_memory *phys_mem, sched_basic_t *scheduler, int max_prio){
    loader -> prid_counter = 1;
    loader -> max_prio = max_prio;
    loader -> scheduler = scheduler;
    mmu_init(&loader->mmu, phys_mem, LOADER_MAX_TLB);
    linkedlist_init(&loader->unloaded_elf_list);
    DIR *d;
    struct dirent *dir;
    d = opendir(LOAD_DIR);
    if (d) {
        int line_number = 0;
        while((dir = readdir(d)) != NULL){
            if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;
            char *file_path = concat(LOAD_DIR, dir->d_name);
            linkedlist_push(&loader->unloaded_elf_list, file_path);
        }
        closedir(d);
    }
    merge_sort(&loader->unloaded_elf_list, (int (*)(void *, void *)) strcmp);
    return 0;
}

int loader_load_next_process(loader_s *loader){
    char * current_path = linkedlist_pop(&loader->unloaded_elf_list);
    if(current_path == NULL)
        return -1;
    struct pcb_t *pcb = malloc(sizeof(struct pcb_t));
    if(pcb == NULL){
        printf("Error: malloc failed in loader_load_next_process\n");
        exit(1);
    }
    pcb_init(pcb, __loader_get_valid_pid(loader), __loader_get_valid_priority(loader));
    __load_file(&loader->mmu, current_path, pcb);
    sched_add_to_process_queue(loader->scheduler, pcb);
    return 0;
}

int __load_file(struct mmu *p_mmu, char *d_name, struct pcb_t *pcb){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    size_t read;
    printf("%s",d_name);
    fp = fopen(d_name, "r");
    if (fp == NULL){ // Fitxategia ireki
        exit(1);
        return 0;
    }

    // Zenbatu fitxategiaren lerroak: Erreserbatu behar den word kopurua jakiteko
    int32_t line_numb = 0;
    while ((read = getline(&line, &len, fp)) != -1) {line_numb++;}
    fseek (fp, 0 , SEEK_SET );
    if(line_numb < 2){
        return 0;
    }

    //Memoria erreserbatu programak dituen lerroen (agindu zein datuen) arabera
    mmu_malloc(p_mmu, &pcb->memory_map.pgb, line_numb-2);

    // Lehenengo 2 lerroak prozesatu: Header-ak
    for(int i = 0; i < 2; i++){
        //Line 1: .text XXXXXX
        read = getline(&line, &len, fp);
        line[strcspn(line, "\r\n")] = 0;
        line = strpbrk(line, " ")+1;

        if(i == 0){
            pcb->memory_map.code = atoi(line) * 4;
            cpu_snapshot_init((struct cpu_snapshot *) &pcb->status, pcb->memory_map.code); // PCB-aren PC-a hasieratu kode helbidera

        }else{
            pcb->memory_map.data = atoi(line) * 4;
        }
    }

    // Geratzen den programaren edukia kargatu
    uint32_t virt_adress = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line %d of length %zu:\n",line_numb, read);
        //printf("%s", line);
        uint32_t current_data = 0;
        line[strcspn(line, "\r\n")] = 0;
        if(!__hex_string_to_int32(line, &current_data)){
            return 0;
        };
        pm_write_word(p_mmu->ps, mmu_resolve(p_mmu,pcb->memory_map.pgb,virt_adress), current_data);
        virt_adress+=4;
    }

    fclose(fp);
    tlb_flush(p_mmu);
    return 1;
}

int load_null_pcb(struct mmu *p_mmu, struct pcb_t *pcb){
    mmu_malloc(p_mmu, &pcb->memory_map.pgb, 1);
    pcb->memory_map.data = 0;
    pcb->memory_map.code = 0;
    pcb->current_priority = 0;
    pcb->pid = -1;
    cpu_snapshot_init((struct cpu_snapshot *) &pcb->status, 0);
    pm_write_word(p_mmu->ps, mmu_resolve(p_mmu,pcb->memory_map.pgb,0), EXIT_CODE);
    tlb_flush(p_mmu);
}

int __hex_string_to_int32(char *hex_string, uint32_t *result){
    uint8_t current_nibble = 0;
    uint32_t current_data = 0;
    for(int i = 0; i < strlen(hex_string); i++){
        uint8_t current_nibble_value = 0;
        if(!__hex_char_to_int4(hex_string[i], &current_nibble_value)){
            return 0;
        }
        current_data |= current_nibble_value << (28 - current_nibble*4);
        current_nibble++;
        if(current_nibble == 8){
            *result = current_data;
            return 1;
        }
    }
    return 0;
}

int __hex_char_to_int4(char hex_char, uint8_t *result){
    if(hex_char >= '0' && hex_char <= '9'){
        *result = hex_char - '0';
        return 1;
    }else if(hex_char >= 'A' && hex_char <= 'F'){
        *result = hex_char - 'A' + 10;
        return 1;
    }else if(hex_char >= 'a' && hex_char <= 'f'){
        *result = hex_char - 'a' + 10;
        return 1;
    }
    return 0;
}