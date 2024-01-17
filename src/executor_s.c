#define PC_INCREMENT 4

#include "executor.h"
#include "memory_s.h"

uint32_t get_nibble_range(uint32_t item, int leftmost_nibble, int rightmost_nibble){
    item = item >> (rightmost_nibble*4);
    item = item & ((1 << (leftmost_nibble - rightmost_nibble + 1)*4) - 1);
    return item;
}

int executor_init(executor_t *exec, cpu_s cpu_arr[], int cpu_arr_size){
    exec->cpu_arr = cpu_arr;
    exec->cpu_arr_size = cpu_arr_size;
    return 0;
}

void executor_exec(executor_t *exec){
    execute_all(exec->cpu_arr, exec->cpu_arr_size);
}

int execute_all(cpu_s cpu_arr[], int cpu_arr_size) {
    for (int i = 0; i < cpu_arr_size; i++) {
        int current_core_kop = cpu_arr[i].core_kop;
        struct core_s **current_core_arr = cpu_arr[i].core_arr;
        for (int j = 0; j < current_core_kop; j++) {
            int current_hari_kop = current_core_arr[j]->thread_kop;
            for (int k = 0; k < current_hari_kop; k++) {
                execute_current(current_core_arr[j]->thread_arr[k]);
            }
        }
    }
}

int execute_current(struct thread_s *cpu_context){
    uint32_t plain_command_address = mmu_resolve(&cpu_context->mem_manag_unit, cpu_context->PTBR, cpu_context->PC);
    uint32_t plain_command = pm_read_word(cpu_context->mem_manag_unit.ps, plain_command_address);
    cpu_context->IR = plain_command;
    return __execute(cpu_context, plain_command);
}

int __execute(struct thread_s *cpu_context, uint32_t plain_command){
    uint8_t command = get_nibble_range(plain_command, 7, 7);
    switch(command){
        case OP_LD:
        case OP_ST:
            cpu_context->PC += PC_INCREMENT;
            __ex_MEM(cpu_context, plain_command);
            break;
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_AND:
        case OP_OR:
        case OP_XOR:
        case OP_MOV:
        case OP_CMP:
            cpu_context->PC += PC_INCREMENT;
            __ex_ALU(cpu_context, plain_command);
            break;
        case OP_B:
        case OP_BEQ:
        case OP_BGT:
        case OP_BLT:
            cpu_context->PC += PC_INCREMENT;
        case OP_EXIT:
            __ex_CU(cpu_context, plain_command);
            break;
        default:
            return -1;
    }
    return 0;
}

void __ex_LD(struct thread_s *cpu_context, uint8_t reg, uint32_t address){
    uint32_t physical_address = mmu_resolve(&cpu_context->mem_manag_unit, cpu_context->PTBR, address);
    uint32_t word = pm_read_word(cpu_context->mem_manag_unit.ps, physical_address);
    cpu_context->R[reg] = (int32_t)word;
}

void __ex_ST(struct thread_s *cpu_context, uint8_t reg, uint32_t virtual_address){
    uint32_t physical_address = mmu_resolve(&cpu_context->mem_manag_unit, cpu_context->PTBR, virtual_address);
    pm_write_word(cpu_context->mem_manag_unit.ps, physical_address, cpu_context->R[reg]);
}

void __ex_MEM(struct thread_s *cpu_context, uint32_t plain_command){
    uint8_t command = get_nibble_range(plain_command, 7, 7);
    uint8_t target_reg = get_nibble_range(plain_command, 6, 6);
    uint32_t address = get_nibble_range(plain_command, 5, 0);
    switch(command){
        case OP_LD:
            __ex_LD(cpu_context, target_reg, address);
            break;
        case OP_ST:
                __ex_ST(cpu_context, target_reg, address);
                break;
        default:
            break;
    }
}

void __ex_ALU(struct thread_s *cpu_context, uint32_t plain_command){
    uint8_t command = get_nibble_range(plain_command, 7, 7);
    uint8_t target_reg = get_nibble_range(plain_command, 6, 6);
    uint8_t source_reg1 = get_nibble_range(plain_command, 5, 5);
    uint8_t source_reg2 = get_nibble_range(plain_command, 4, 4);
    switch(command){
        case OP_ADD:
            __ex_arithm_add(cpu_context, target_reg, source_reg1, source_reg2);
            break;
        case OP_SUB:
            __ex_arithm_sub(cpu_context, target_reg, source_reg1, source_reg2);
            break;
        case OP_MUL:
            __ex_arithm_mul(cpu_context, target_reg, source_reg1, source_reg2);
            break;
        case OP_DIV:
            __ex_arithm_div(cpu_context, target_reg, source_reg1, source_reg2);
            break;
        case OP_AND:
            __ex_logic_and(cpu_context, target_reg, source_reg1, source_reg2);
            break;
        case OP_OR:
            __ex_logic_or(cpu_context, target_reg, source_reg1, source_reg2);
            break;
        case OP_XOR:
            __ex_logic_xor(cpu_context, target_reg, source_reg1, source_reg2);
            break;
        case OP_MOV:
            __ex_mov(cpu_context, target_reg, source_reg1);
            break;
        case OP_CMP:
            __ex_cmp(cpu_context, source_reg1, source_reg2);
            break;
        default:
            break;
    }
}

void __ex_arithm_add(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2){
    cpu_context->R[target_reg] = cpu_context->R[source_reg1] + cpu_context->R[source_reg2];
}

void __ex_arithm_sub(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2){
    cpu_context->R[target_reg] = cpu_context->R[source_reg1] - cpu_context->R[source_reg2];
}

void __ex_arithm_mul(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2){
    cpu_context->R[target_reg] = cpu_context->R[source_reg1] * cpu_context->R[source_reg2];
}

void __ex_arithm_div(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2){
    cpu_context->R[target_reg] = cpu_context->R[source_reg1] / cpu_context->R[source_reg2];
}

void __ex_logic_and(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2){
    cpu_context->R[target_reg] = cpu_context->R[source_reg1] & cpu_context->R[source_reg2];
}

void __ex_logic_or(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2){
    cpu_context->R[target_reg] = cpu_context->R[source_reg1] | cpu_context->R[source_reg2];
}

void __ex_logic_xor(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2){
    cpu_context->R[target_reg] = cpu_context->R[source_reg1] ^ cpu_context->R[source_reg2];
}

void __ex_mov(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1){
    cpu_context->R[target_reg] = cpu_context->R[source_reg1];
}

void __ex_cmp(struct thread_s *cpu_context, uint8_t source_reg1, uint8_t source_reg2){
    cpu_context->cc = cpu_context->R[source_reg1] - cpu_context->R[source_reg2];
}

void __ex_CU(struct thread_s *cpu_context, uint32_t plain_command){
    uint8_t command = get_nibble_range(plain_command, 7, 7);
    uint32_t address = get_nibble_range(plain_command, 5, 0);
    switch(command){
        case OP_B:
            __ex_b(cpu_context, address);
            break;
        case OP_BEQ:
            __ex_beq(cpu_context, address);
            break;
        case OP_BGT:
            __ex_bgt(cpu_context, address);
            break;
        case OP_BLT:
            __ex_blt(cpu_context, address);
            break;
        case OP_EXIT:
            __ex_exit(cpu_context, address);
            break;
        default:
            break;
    }
}

void __ex_b(struct thread_s *cpu_context, uint32_t address){
    cpu_context->PC = address;
}

void __ex_beq(struct thread_s *cpu_context, uint32_t address){
    if(cpu_context->cc == 0){
        cpu_context->PC = address;
    }
}

void __ex_bgt(struct thread_s *cpu_context, uint32_t address){
    if(cpu_context->cc > 0){
        cpu_context->PC = address;
    }
}

void __ex_blt(struct thread_s *cpu_context, uint32_t address){
    if(cpu_context->cc < 0){
        cpu_context->PC = address;
    }
}

void __ex_exit(struct thread_s *cpu_context, uint32_t address){
    cpu_context->PC = address;
    //cpu_context->;
}