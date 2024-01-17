#include "memory_s.h"
#include "linkedlist_s.h"

#include <assert.h>
#include <stdlib.h>
#include <err.h>

uint32_t pm_read_word(struct physical_memory *ps, uint32_t address){
    uint32_t word_start_address_mask = (BUS_ADDRESS_SPACE - 1) & ~(BUS_DATA_BYTES_SIZE-1);
    uint32_t word_start_address = address & word_start_address_mask;
    uint32_t data = 0x0;
    for (int byte_i = 0; byte_i < BUS_DATA_BYTES_SIZE; byte_i++) {
        data = data << DATA_CELL_STORE_BYTES * 8;
        uint8_t current_byte = ps->memory[word_start_address + byte_i];
        data = data | (current_byte & 0xFF);
    }
    return data;
}

int pm_write_word(struct physical_memory *ps, uint32_t address, uint32_t word) {
    uint32_t word_start_address_mask = (BUS_ADDRESS_SPACE - 1) & ~(BUS_DATA_BYTES_SIZE - 1);
    uint32_t word_start_address = address & word_start_address_mask;
    uint32_t data = word;

    for (int byte_i = BUS_DATA_BYTES_SIZE - 1; byte_i >= 0; byte_i--) {
        uint32_t current_byte = data & ((1 << DATA_CELL_STORE_BITS) - 1);
        ps->memory[word_start_address + byte_i] = current_byte;
        data = data >> DATA_CELL_STORE_BITS;
    }
    return 0;
}

int __search_space(void *free_block, void *search_args) {
    struct free_block_search_args *args = (struct free_block_search_args *) search_args;
    int default_frame_kop = 0;
    if (!search_args) search_args = &default_frame_kop;
    struct free_block *fb = (struct free_block *) free_block;
    if (!fb) return 0;
    if (fb->end_address < args->start_limit_address || fb->start_address > args->end_limit_address) return 0;
    uint32_t valid_start = fb->start_address;
    uint32_t valid_end = fb->end_address;
    if (fb->start_address < args->start_limit_address) valid_start = args->start_limit_address;
    if (fb->end_address > args->end_limit_address) valid_end = args->end_limit_address;
    return (valid_start - valid_end + 1) >= args->min_address_kon;
}

int __search_address(void * free_block, void *search_args){
    uint32_t *address = (int *) search_args;
    if(!search_args) return 0;
    struct free_block *fb = (struct free_block *) free_block;
    if(!fb) return 0;
    if(fb->start_address <= *address && fb->end_address >= *address) return 1;
    return 0;
}

int pm_m_frame_malloc(struct physical_memory *ps, uint32_t *address){
    struct free_block_search_args args = {
            .min_address_kon = FRAME_SIZE,
            .start_limit_address = MEM_SPACE_START_ADDRESS,
            .end_limit_address = MEM_SPACE_END_ADDRESS
    };

    uint32_t result;
    if(!pm_memory_allocation_request(ps, &args, &result)){
        err(1, "pm_m_frame_malloc: pm_memory_allocation_request failed");
        return -1;
    }
    *address = result;
    return 0;
}

int pm_m_frame_free(struct physical_memory *ps, uint32_t physical_address){
    uint32_t frame_start_address = physical_address & FRAME_ADDRESS_MASK;
    if (linkedlist_get_first_match_from_rear(&ps->free_blocks, __search_address, &frame_start_address)) {
        err(1, "pm_m_frame_free: frame not found");
        return -1;
    }
    struct free_block *new_fb;
    new_fb = (struct free_block *) malloc(sizeof(struct free_block));
    new_fb->start_address = frame_start_address;
    new_fb->end_address = frame_start_address + FRAME_SIZE - 1;
    linkedlist_push(&ps->free_blocks, (void *) new_fb);
    return 0;
}

int pm_m_frame_malloc_bulk(struct physical_memory *ps, uint32_t frame_address_array[], int frame_count){
    uint32_t frame_address;
    int current_entry;
    for(current_entry = 0; current_entry < frame_count; current_entry++){
        if(!pm_m_frame_malloc(ps, &frame_address_array[current_entry])){
            err(1, "pm_m_frame_malloc_bulk: pm_m_frame_malloc failed");
            return -1;
        }
    }
    if(current_entry != frame_count){
        for(int i = 0; i < current_entry; i++){
            pm_m_frame_free(ps, frame_address_array[i]);
        }
        return 0;
    }
    return 0;
}

int pm_pt_page_table_malloc(struct physical_memory *ps, uint32_t *address, uint32_t entries){
    struct free_block_search_args args = {
            .min_address_kon = PAGETABLE_CORE_ADDRESS_KOP + PAGETABLE_ENTRY_ADDRESS_KOP * entries,
            .start_limit_address = KERNEL_SPACE_START_ADDRESS,
            .end_limit_address = KERNEL_SPACE_END_ADDRESS
    };
    uint32_t result;
    if(!pm_memory_allocation_request(ps, &args, &result)){
        err(1, "pm_pt_page_table_malloc: pm_memory_allocation_request failed");
        return -1;
    }
    uint32_t frame_address_array[entries];
    if(!pm_m_frame_malloc_bulk(ps, frame_address_array, entries)){
        err(1, "pm_pt_page_table_malloc: pm_m_frame_malloc_bulk failed");
        return -1;
    }
    pm_write_word(ps, result, entries);
    uint32_t page_entry_address = result + PAGETABLE_CORE_ADDRESS_KOP;
    for(int current_entry = 0; current_entry < entries; current_entry++){
        pm_write_word(ps, page_entry_address, frame_address_array[current_entry]);
        page_entry_address += PAGETABLE_ENTRY_ADDRESS_KOP;
    }
    *address = result;
    return 0;
}

int pm_pt_page_table_free(struct physical_memory *ps, uint32_t address) {
    uint32_t table_start_address = address;
    if (linkedlist_get_first_match_from_rear(&ps->free_blocks, __search_address, &table_start_address)) {
        err(1, "pm_pt_page_table_free: table not found");
        return -1;
    }
    uint32_t entries = pm_read_word(ps, table_start_address);
    struct free_block *new_fb;
    new_fb = (struct free_block *) malloc(sizeof(struct free_block));
    new_fb->start_address = table_start_address;
    new_fb->end_address = table_start_address + PAGETABLE_CORE_ADDRESS_KOP + PAGETABLE_ENTRY_ADDRESS_KOP * entries - 1;
    linkedlist_push(&ps->free_blocks, (void *) new_fb);
    uint32_t current_pte_address = table_start_address + PAGETABLE_CORE_ADDRESS_KOP;
    for (int i = 0; i < entries; i++) {
        uint32_t frame_address = pm_read_word(ps, current_pte_address);
        pm_m_frame_free(ps, frame_address);
        current_pte_address += PAGETABLE_ENTRY_ADDRESS_KOP;
    }
    return 0;
}

int pm_memory_allocation_request(struct physical_memory *ps, struct free_block_search_args *request, uint32_t *return_address){
    if(!ps || !request || !return_address){
        err(1, "pm_memory_allocation_request: invalid arguments");
        return -1;
    }
    if(!linkedlist_get_first_match_from_rear(&ps->free_blocks, __search_space, request)){
        err(1, "pm_memory_allocation_request: no free space found");
        return -1;
    }
    struct free_block *fb = (struct free_block *) linkedlist_pop(&ps->free_blocks);
    if(!fb){
        err(1, "pm_memory_allocation_request: linkedlist_pop failed");
        return -1;
    }
    uint32_t free_block_start_address = fb->start_address;
    uint32_t free_block_end_address = fb->end_address;
    free(fb);
    if(free_block_start_address < request->start_limit_address){
        struct free_block *new_fb;
        new_fb = (struct free_block *) malloc(sizeof(struct free_block));
        new_fb->start_address = free_block_start_address;
        new_fb->end_address = request->start_limit_address - 1;
        linkedlist_push(&ps->free_blocks, (void *) new_fb);
    }
    if(free_block_end_address > request->end_limit_address){
        struct free_block *new_fb;
        new_fb = (struct free_block *) malloc(sizeof(struct free_block));
        new_fb->start_address = request->end_limit_address + 1;
        new_fb->end_address = free_block_end_address;
        linkedlist_push(&ps->free_blocks, (void *) new_fb);
    }
    *return_address = request->start_limit_address;
    return 0;
}

int __pm_memory_allocation_procedure(struct physical_memory *ps, struct free_block_search_args *request, uint32_t *ret_address){
    struct free_block *fb = linkedlist_pop_first_match_from_rear(&ps->free_blocks, __search_space, request);
    if(!fb || !ret_address || !request){
        err(1, "__pm_memory_allocation_procedure: invalid arguments");
        return -1;
    }
    uint32_t start_resulting_address = fb->start_address;
    if(fb->start_address < request->start_limit_address){
        start_resulting_address = request->start_limit_address;
        struct free_block *mem_space_block = (struct free_block *) malloc(sizeof(struct free_block));
        mem_space_block->start_address = fb->start_address;
        mem_space_block->end_address = request->start_limit_address - 1;
        linkedlist_push(&ps->free_blocks, (void *) mem_space_block);
    }
    else{
        start_resulting_address = fb->start_address;
    }

    fb->start_address = start_resulting_address + request->min_address_kon;
    if(fb->start_address <= fb->end_address){
        linkedlist_push(&ps->free_blocks, (void *) fb);
    }
    else{
        free(fb);
    }
    *ret_address = start_resulting_address;
    return 0;
}

int __comparator_start_address(void *a, void *b){
    struct free_block *fb_a = (struct free_block *) a;
    struct free_block *fb_b = (struct free_block *) b;
    if(!fb_a || !fb_b){
        err(1, "__comparator_start_address: invalid arguments");
        return -1;
    }
    if(fb_a->start_address < fb_b->start_address) return -1;
    if(fb_a->start_address > fb_b->start_address) return 1;
    return 0;
}

int __optimize_free_block_list(struct physical_memory *ps){
    merge_sort(&ps->free_blocks, __comparator_start_address);
    struct linkedlist_node *current_node = ps->free_blocks.first;
    struct free_block *current_fb, *next_fb;

    while(current_node && current_node->next){
        current_fb = (struct free_block *) current_node->data;
        next_fb = (struct free_block *) current_node->next->data;
        int delete_next = 0;
        if(current_fb->end_address + 1 >= next_fb->start_address){
            current_fb->end_address = next_fb->end_address;
            delete_next = 1;
        } else if (next_fb->start_address <= current_fb->end_address) {
            next_fb->start_address = current_fb->start_address;
            current_node->data = (void *) next_fb;
            next_fb = current_fb;
            delete_next = 1;
        }
        if (delete_next) {
            struct linkedlist_node *next_node = current_node->next;
            unlink_procedure_of_current(&ps->free_blocks, current_node, next_node);
            free(next_fb);
            free(next_node);
        }
        else{
            current_node = current_node->next;
        }
    }
    return 0;
}

uint32_t mmu_resolve(struct mmu *p_mmu, uint32_t ptbr, uint32_t virtual_address){
    struct pte *match_pte;
    uint32_t frame_address;
    if(tlb_get_match(p_mmu, &match_pte, ptbr, virtual_address)){
        frame_address = match_pte->physical_address;
    }
    else{
        mmu_resolve_frame_root_address_from_mem(p_mmu, &frame_address, ptbr, virtual_address);
        tlb_add_entry(p_mmu, ptbr, virtual_address, frame_address);
    }
    uint32_t effective_address = frame_address & ~FRAME_OFFSET_MASK | (virtual_address & FRAME_OFFSET_MASK);
    return effective_address;
}

int mmu_resolve_frame_root_address_from_mem(struct mmu *p_mmu, uint32_t *frame_address, uint32_t ptbr, uint32_t virtual_address){
    uint32_t selected_page_numb = virtual_address & ~FRAME_OFFSET_MASK;
    selected_page_numb = selected_page_numb >> FRAME_OFFSET;
    if(pm_read_word(p_mmu->ps, ptbr) < selected_page_numb){
        err(1, "mmu_resolve_frame_root_address_from_mem: invalid page number");
        return -1;
    }
    uint32_t pte_address = ptbr + PAGETABLE_CORE_ADDRESS_KOP + selected_page_numb * PAGETABLE_ENTRY_ADDRESS_KOP;
    *frame_address = pm_read_word(p_mmu-> ps, pte_address) & ~FRAME_OFFSET_MASK;
    return 0;
}

int tlb_get_match(struct mmu *p_mmu, struct pte **match_pte, uint32_t ptbr, uint32_t virtual_address){
    struct pte * potential_match_pte;
    uint32_t page_number = virtual_address >> FRAME_OFFSET;
    int hash = page_number % p_mmu->tlb_max_space;
    potential_match_pte = &p_mmu->tlb_hash_arr[hash];
    *match_pte = NULL;
    if(potential_match_pte->active && (potential_match_pte->virtual_address & ~FRAME_OFFSET_MASK) == (virtual_address & ~FRAME_OFFSET_MASK)){
        *match_pte = potential_match_pte;
        return 1;
    }
    else{
        return 0;
    }
}

int tlb_add_entry(struct mmu *p_mmu, uint32_t ptbr, uint32_t virtual_address, uint32_t physical_address){
    uint32_t page_number = virtual_address >> FRAME_OFFSET;
    int hash = page_number % p_mmu->tlb_max_space;
    struct pte *new_pte = &p_mmu->tlb_hash_arr[hash];
    new_pte->active = 1;
    new_pte->physical_address = physical_address & ~FRAME_OFFSET_MASK;
    new_pte->virtual_address = virtual_address & ~FRAME_OFFSET_MASK;
}

int tlb_flush(struct mmu *p_mmu){
    struct pte *current_pte;
    for(int i = 0; i < p_mmu->tlb_max_space; i++){
        current_pte = &p_mmu->tlb_hash_arr[i];
        current_pte->active = 0;
        current_pte->physical_address = 0;
        current_pte->virtual_address = 0;
    }
}

int pmemo_init(struct physical_memory *pm){
    pm->memory = (char *) (sizeof(char)*BUS_ADDRESS_SPACE);
    linkedlist_init(&pm->free_blocks);
    struct free_block *fb;
    fb = (struct free_block *) malloc(sizeof(struct free_block));
    fb->start_address = ALL_MEM_SPACE_START_ADDRESS;
    fb->end_address = ALL_MEM_SPACE_END_ADDRESS;
    linkedlist_push(&pm->free_blocks, (void *) fb);
}

int mmu_init(struct mmu *mmu, struct physical_memory *pm, int tlb_max_space){
    mmu->ps = pm;
    mmu->tlb_max_space = tlb_max_space;
    mmu->tlb_hash_arr = (struct pte *) malloc(sizeof(struct pte) * tlb_max_space);
    tlb_flush(mmu);
}

int mmu_malloc(struct mmu *target_mmu, uint32_t *ptbr, int32_t word_kop){
    int frames_needed = word_kop * 4 / FRAME_SIZE;
    if(word_kop * 4 % FRAME_SIZE) frames_needed++;
    return pm_pt_page_table_malloc(target_mmu->ps, ptbr, (uint32_t) frames_needed);
}

int mmu_free(struct mmu *target_mmu, uint32_t ptbr){
    return pm_pt_page_table_free(target_mmu->ps, ptbr);
}