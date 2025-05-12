#ifndef C_GEN_H
#define C_GEN_H

#include <vector>
#include <map>
#include <string>
#include <cstdint>
#include "token.h"
#include "s_table.h"
#include "c_tree.h"

struct code_gen
{
    vector<uint8_t>& binary;
    int label_counter = 0;
    vector<symbol_data>& symbols;

    map<size_t, int> jump_patch_locations;
    map<int, size_t> label_addresses;

    code_gen(vector<uint8_t>& bin, vector<symbol_data>& sym);

    int new_label();
    void place_label(int label_id);
    void add_jump_patch(size_t instruction_offset, int target_label_id);
    void append_int32(int value);
    void jump();

    void mov_eax_imm(int value);
    void mov_eax_var(int symbol_index);
    void mov_var_eax(int symbol_index);
    void push_eax();
    void pop_eax();
    void push_ebx();
    void pop_ebx();
    void add_eax_ebx();
    void sub_eax_ebx();
    void imul_eax_ebx();
    void cdq();
    void idiv_ebx();
    void xchg_eax_ebx();
    void cmp_eax_ebx();
    void test_al_imm8(uint8_t imm);
    void test_al_al();
    void xor_al_imm8(uint8_t imm);
    void movzx_eax_al();
    void setcc_al(token_id op);
    void jcc_rel32(token_id op_for_condition, bool jump_if_true, int target_label);
    void jmp_rel32(int target_label);
    void neg_eax();
    void mov_eax_edx();

};
void generate_node_code(node* n, code_gen& ctx);
void generate_program_code(node* program_ast_head, vector<uint8_t>& binary, vector<symbol_data>& symbols);


#endif