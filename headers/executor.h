#ifndef PROIEKTUA_EXECUTOR_H
#define PROIEKTUA_EXECUTOR_H

#include "cpu_data_s.h"
#include <inttypes.h>

#define OP_LD 0
#define OP_ST 1
#define OP_ADD 2
#define OP_SUB 3
#define OP_MUL 4
#define OP_DIV 5
#define OP_AND 6
#define OP_OR 7
#define OP_XOR 8
#define OP_MOV 9
#define OP_CMP 10
#define OP_B 11
#define OP_BEQ 12
#define OP_BGT 13
#define OP_BLT 14
#define OP_EXIT 15

#define EXIT_CODE 0xF0000000

typedef struct executor_t{
    cpu_s *cpu_arr;
    int cpu_arr_size;
} executor_t;

uint32_t get_nibble_range(uint32_t item, int leftmost_nibble, int rightmost_nibble);
int executor_init(executor_t *exec, cpu_s cpu_arr[], int cpu_arr_size);
void executor_exec(executor_t *exec);
int execute_all(cpu_s cpu_arr[], int cpu_arr_size);
int execute_current(struct thread_s *cpu_context);
void __ex_MEM(struct thread_s *cpu_context, uint32_t plain_command);
int __execute(struct thread_s *cpu_context, uint32_t plain_command);
void __ex_LD(struct thread_s *cpu_context, uint8_t reg, uint32_t address);
void __ex_ST(struct thread_s *cpu_context, uint8_t reg, uint32_t address);
void __ex_ALU(struct thread_s *cpu_context, uint32_t plain_command);
void __ex_arithm_add(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2);
void __ex_arithm_sub(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2);
void __ex_arithm_mul(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2);
void __ex_arithm_div(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2);
void __ex_logic_and(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2);
void __ex_logic_or(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2);
void __ex_logic_xor(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg1, uint8_t source_reg2);
void __ex_mov(struct thread_s *cpu_context, uint8_t target_reg, uint8_t source_reg);
void __ex_cmp(struct thread_s *cpu_context, uint8_t source_reg1, uint8_t source_reg2);

void __ex_CU(struct thread_s *cpu_context, uint32_t plain_command);
void __ex_b(struct thread_s *cpu_context, uint32_t plain_command);
void __ex_beq(struct thread_s *cpu_context, uint32_t plain_command);
void __ex_bgt(struct thread_s *cpu_context, uint32_t plain_command);
void __ex_blt(struct thread_s *cpu_context, uint32_t plain_command);
void __ex_exit(struct thread_s *cpu_context, uint32_t plain_command);


#endif //PROIEKTUA_EXECUTOR_H
