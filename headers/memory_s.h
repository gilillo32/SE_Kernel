#ifndef PROIEKTUA_MEMORY_S_H
#define PROIEKTUA_MEMORY_S_H

#define BUS_BIT_SIZE 24
#define BUS_ADDRESS_SPACE (1<<BUS_BIT_SIZE)
#define TLB_SIZE 64
#define FRAME_OFFSET 8
#define PAGE_BITS (BUS_BIT_SIZE- FRAME_OFFSET)
#define PAGE_SIZE (1<<PAGE_BITS)
#define BUS_DATA_BYTES_SIZE 4
#define BUS_DATA_BITS_SIZE (BUS_DATA_BYTES_SIZE*8)
#define FRAME_SIZE (1<<FRAME_OFFSET)
#define FRAME_OFFSET_MASK (FRAME_SIZE-1)
#define FRAME_ADDRESS_MASK ((BUS_ADDRESS_SPACE-1) & ~(FRAME_OFFSET_MASK))
#define PAGETABLE_CORE_ADDRESS_KOP 4
#define PAGETABLE_ENTRY_ADDRESS_KOP 4
#define DATABUS_CELL_SIZE 4
#define DATABUS_WORD_OFFSET_MASK (BUS_DATA_BYTES_SIZE-1)
#define DATABUS_WORD_ADDRESS_MASK (BUS_ADDRESS_SPACE - 1) & ~(BUS_DATA_BYTES_SIZE-1)
#define DATA_CELL_STORE_BYTES 1
#define DATA_CELL_STORE_BITS (DATA_CELL_STORE_BYTES*8)

#define KERNEL_SPACE_START_ADDRESS 0x000000
#define KERNEL_SPACE_END_ADDRESS 0x3FFFFF
#define MEM_SPACE_START_ADDRESS 0x400000
#define MEM_SPACE_END_ADDRESS 0xFFFFFF

#define ALL_MEM_SPACE_START_ADDRESS KERNEL_SPACE_START_ADDRESS
#define ALL_MEM_SPACE_END_ADDRESS MEM_SPACE_END_ADDRESS

#include <stdint.h>
#include "linkedlist_s.h"

struct physical_memory {
    char *memory;
    linkedlist_structure free_blocks;
};

struct free_block{
    uint32_t start_address;
    uint32_t end_address;
};

struct pte{
    //PAGE TABLE ENTRY
    uint32_t virtual_address;
    uint32_t physical_address;
    uint8_t active;
};

struct mmu{
    struct pte *tlb_hash_arr;
    int tlb_max_space;
    struct physical_memory *ps;
};

struct free_block_search_args{
    uint32_t start_limit_address;
    uint32_t end_limit_address;
    uint32_t min_address_kon;
};

int pmemo_init(struct physical_memory *pmemo);
int mmu_init(struct mmu *mmu, struct physical_memory *pmemo, int tlb_max_space);

int __pm_memory_allocation_procedure(struct physical_memory *ps, struct free_block_search_args *request, uint32_t *return_address);
int __search_space(void *free_block, void *search_args);
int __optimize_free_block_list(struct physical_memory *ps);
int pm_memory_allocation_request(struct physical_memory *ps, struct free_block_search_args *request, uint32_t *return_address);
int pm_m_frame_malloc(struct physical_memory *ps, uint32_t *address);
int pm_m_frame_free(struct physical_memory *ps, uint32_t address);
uint32_t pm_read_word(struct physical_memory *ps, uint32_t address);
int pm_write_word(struct physical_memory *ps, uint32_t address, uint32_t data);
int pm_pt_page_table_malloc(struct physical_memory *ps, uint32_t *address, uint32_t entries);
int pm_pt_page_table_free(struct physical_memory *ps, uint32_t address);

uint32_t mmu_resolve(struct mmu *mmu, uint32_t ptbr, uint32_t virtual_address);
int mmu_resolve_frame_root_address_from_mem(struct mmu *mmu, uint32_t *frame_address, uint32_t ptbr, uint32_t virtual_address);

int mmu_malloc(struct mmu *target_mmu, uint32_t *ptbr, int32_t word_kop);
int mmu_free(struct mmu *target_mmu, uint32_t ptbr);
int mmu_init(struct mmu *target_mmu, struct physical_memory *pm, int max_tlb_space);

int tlb_get_match(struct mmu *p_mmu, struct pte **match_pte, uint32_t ptbr, uint32_t virtual_address);
int tlb_add_entry(struct mmu *p_mmu, uint32_t ptbr, uint32_t virtual_address, uint32_t physical_address);
int tlb_flush(struct mmu *p_mmu);

#endif //PROIEKTUA_MEMORY_S_H
