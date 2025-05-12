#include "c_gen.h"
#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>
#include <iterator>

code_gen::code_gen(vector<uint8_t>& bin, vector<symbol_data>& sym) : binary(bin), symbols(sym), label_counter(0) {}

int code_gen::new_label()
{
    return label_counter++;
}

void code_gen::place_label(int label_id)
{
    if (label_addresses.count(label_id))
    {
        cerr << "Codegen Warning: Label " << label_id << " placed multiple times." << endl;
    }
    label_addresses[label_id] = binary.size();
}

void code_gen::add_jump_patch(size_t instruction_offset, int target_id)
{
    jump_patch_locations[instruction_offset] = target_id;
}

void code_gen::append_int32(int value)
{
    binary.push_back(static_cast<uint8_t>(value & 0xFF));
    binary.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    binary.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    binary.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

void code_gen::mov_eax_imm(int value)
{
    binary.push_back(0xB8);
    append_int32(value);
}

void code_gen::mov_eax_var(int symbol_index)
{
    if (symbol_index < 0 || symbol_index >= symbols.size())
    {
        cerr << "Codegen Error: Invalid symbol index " << symbol_index << " for load." << endl;
        binary.push_back(0xCC); return;
    }
    int offset = -4 * (symbol_index + 1);
    binary.push_back(0x8B);
    binary.push_back(0x85);
    append_int32(offset);
}

void code_gen::mov_var_eax(int symbol_index)
{
    if (symbol_index < 0 || symbol_index >= symbols.size())
    {
        cerr << "Codegen Error: Invalid symbol index " << symbol_index << " for store." << endl;
        binary.push_back(0xCC); return;
    }
    int offset = -4 * (symbol_index + 1);
    binary.push_back(0x89);
    binary.push_back(0x85);
    append_int32(offset);
}

void code_gen::push_eax()
{
    binary.push_back(0x50);
}
void code_gen::pop_eax()
{
    binary.push_back(0x58);
}
void code_gen::push_ebx()
{
    binary.push_back(0x53);
}
void code_gen::pop_ebx()
{
    binary.push_back(0x5B);
}
void code_gen::add_eax_ebx()
{
    binary.push_back(0x01);
    binary.push_back(0xD8);
}
void code_gen::sub_eax_ebx()
{
    binary.push_back(0x29);
    binary.push_back(0xD8);
}
void code_gen::imul_eax_ebx()
{
    binary.push_back(0x0F);
    binary.push_back(0xAF);
    binary.push_back(0xC3);
}
void code_gen::cdq()
{
    binary.push_back(0x99);
}
void code_gen::idiv_ebx()
{
    binary.push_back(0xF7);
    binary.push_back(0xFB);
}
void code_gen::xchg_eax_ebx()
{
    binary.push_back(0x93);
}
void code_gen::cmp_eax_ebx()
{
    binary.push_back(0x39);
    binary.push_back(0xD8);
}
void code_gen::test_al_imm8(uint8_t imm)
{
    binary.push_back(0xA8);
    binary.push_back(imm);
}
void code_gen::test_al_al()
{
    binary.push_back(0x84);
    binary.push_back(0xC0);
}
void code_gen::xor_al_imm8(uint8_t imm)
{
    binary.push_back(0x34);
    binary.push_back(imm);
}
void code_gen::movzx_eax_al()
{
    binary.push_back(0x0F);
    binary.push_back(0xB6);
    binary.push_back(0xC0);
}
void code_gen::neg_eax()
{
    binary.push_back(0xF7);
    binary.push_back(0xD8);
}
void code_gen::mov_eax_edx()
{
    binary.push_back(0x89);
    binary.push_back(0xD0);
}

void code_gen::setcc_al(token_id op)
{
    binary.push_back(0x0F);
    uint8_t setcc_opcode;
    switch (op)
    {
    case TOKEN_EQUAL:
        setcc_opcode = 0x94;
        break;
    case TOKEN_NOT_EQUAL:
        setcc_opcode = 0x95;
        break;
    case TOKEN_LESS:
        setcc_opcode = 0x9C;
        break;
    case TOKEN_LESS_EQ:
        setcc_opcode = 0x9E;
        break;
    case TOKEN_GREATER:
        setcc_opcode = 0x9F;
        break;
    case TOKEN_GREATER_EQ:
        setcc_opcode = 0x9D;
        break;
    default:
        binary.push_back(0xCC);
        return;
    }
    binary.push_back(setcc_opcode);
    binary.push_back(0xC0);
}

void code_gen::jcc_rel32(token_id op_for_condition, bool jump_if_true, int target_label)
{
    binary.push_back(0x0F);
    uint8_t condition_code;
    bool negate_condition = !jump_if_true;

    switch (op_for_condition)
    {
    case TOKEN_EQUAL:
        condition_code = negate_condition ? 0x85 : 0x84;
        break;
    case TOKEN_NOT_EQUAL:
        condition_code = negate_condition ? 0x84 : 0x85;
        break;
    case TOKEN_LESS:
        condition_code = negate_condition ? 0x8D : 0x8C;
        break;
    case TOKEN_LESS_EQ:
        condition_code = negate_condition ? 0x8F : 0x8E;
        break;
    case TOKEN_GREATER:
        condition_code = negate_condition ? 0x8E : 0x8F;
        break;
    case TOKEN_GREATER_EQ:
        condition_code = negate_condition ? 0x8C : 0x8D;
        break;
    case TOKEN_AND: case TOKEN_OR: case TOKEN_NOT: case TOKEN_TRUE: case TOKEN_FALSE:
        condition_code = jump_if_true ? 0x85 : 0x84;
        break;
    default: 
        binary.push_back(0xCC);
        return;
    }
    binary.push_back(condition_code);
    add_jump_patch(binary.size(), target_label);
    append_int32(0);
}

void code_gen::jmp_rel32(int target_label)
{
    binary.push_back(0xE9);
    add_jump_patch(binary.size(), target_label);
    append_int32(0);
}

void code_gen::jump()
{
    for (std::map<size_t, int>::const_iterator it = jump_patch_locations.begin(); it != jump_patch_locations.end(); ++it)
    {
        size_t addr = it->first;
        int target_id = it->second;

        if (label_addresses.count(target_id))
        {
            size_t target_addr = label_addresses[target_id];
            size_t after_jump_addr;

            if ((addr > 1) && (binary[addr - 1] >= 0x80 && binary[addr - 1] <= 0x8F) && binary[addr - 2] == 0x0F)
            {
                after_jump_addr = addr + 4;
            }
            else if ((addr > 0) && (binary[addr - 1] == 0xE9))
            {
                after_jump_addr = addr + 4;
            }
            else
            {
                cerr << "Codegen Warning: Unknown instruction type preceding jump patch address " << addr << endl;
                after_jump_addr = addr + 4;
            }

            int relative_offset = static_cast<int>(target_addr - after_jump_addr);

            binary[addr] = static_cast<uint8_t>(relative_offset & 0xFF);
            binary[addr + 1] = static_cast<uint8_t>((relative_offset >> 8) & 0xFF);
            binary[addr + 2] = static_cast<uint8_t>((relative_offset >> 16) & 0xFF);
            binary[addr + 3] = static_cast<uint8_t>((relative_offset >> 24) & 0xFF);
        }
        else
        {
            binary[addr] = 0xCC;
            binary[addr + 1] = 0xCC;
            binary[addr + 2] = 0xCC;
            binary[addr + 3] = 0xCC;
        }
    }
}

void generate_node_code(node* n, code_gen& ctx)
{
    if (!n)
    {
        return;
    }
    switch (n->token.id)
    {
    case TOKEN_INTEGER:
        ctx.mov_eax_imm(stoi(n->token.val));
        break;
    case TOKEN_TRUE:
        ctx.binary.push_back(0xB0);
        ctx.binary.push_back(0x01);
        break;
    case TOKEN_FALSE:
        ctx.binary.push_back(0xB0);
        ctx.binary.push_back(0x00);
        break;
    case TOKEN_STRING:
        ctx.mov_eax_imm(0);
        break;
    case TOKEN_IDENT:
        ctx.mov_eax_var(n->symbol_table_index);
        break;

    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_MULT:
    case TOKEN_DIV:
    case TOKEN_MOD:
    {
        generate_node_code(n->right, ctx);
        ctx.push_eax();
        generate_node_code(n->left, ctx);
        ctx.pop_ebx();
        if (n->token.id == TOKEN_PLUS)
        {
            ctx.add_eax_ebx();
        }
        else if (n->token.id == TOKEN_MINUS)
        {
            ctx.sub_eax_ebx();
        }
        else if (n->token.id == TOKEN_MULT)
        {
            ctx.imul_eax_ebx();
        }
        else if (n->token.id == TOKEN_DIV || n->token.id == TOKEN_MOD)
        {
            ctx.cdq();
            ctx.idiv_ebx();
            if (n->token.id == TOKEN_MOD)
            {
                ctx.mov_eax_edx();
            }
        }
    }
    break;
    case TOKEN_LESS:
    case TOKEN_LESS_EQ:
    case TOKEN_GREATER:
    case TOKEN_GREATER_EQ:
    case TOKEN_EQUAL:
    case TOKEN_NOT_EQUAL:
    {
        generate_node_code(n->right, ctx);
        ctx.push_eax();
        generate_node_code(n->left, ctx);
        ctx.pop_ebx();
        ctx.cmp_eax_ebx();
        ctx.setcc_al(n->token.id);
    }
    break;
    case TOKEN_NOT:
        generate_node_code(n->left, ctx);
        ctx.xor_al_imm8(1);
        break;
    case TOKEN_AND:
    {
        int end_label = ctx.new_label();
        generate_node_code(n->left, ctx);
        ctx.test_al_al();
        ctx.jcc_rel32(TOKEN_FALSE, true, end_label);
        generate_node_code(n->right, ctx);
        ctx.place_label(end_label);
    }
    break;
    case TOKEN_OR:
    {
        int end_label = ctx.new_label();
        generate_node_code(n->left, ctx);
        ctx.test_al_al();
        ctx.jcc_rel32(TOKEN_TRUE, true, end_label);
        generate_node_code(n->right, ctx);
        ctx.place_label(end_label);
    }
    break;

    case TOKEN_ASSIGN:
    {
        if (n->left == nullptr || n->left->token.id != TOKEN_IDENT)
        {
            return;
        }
        generate_node_code(n->right, ctx);
        ctx.mov_var_eax(n->left->symbol_table_index);
    }
    break;

    case TOKEN_PRINT:
        if (n->left)
        {
            generate_node_code(n->left, ctx);
        }
        break;
    case TOKEN_READ:
        if (n->left && n->left->token.id == TOKEN_IDENT)
        {
        }
        break;
    case TOKEN_INT4:
        break;

    case TOKEN_IF:
    {
        int else_label = ctx.new_label();
        int end_if_label = ctx.new_label();
        bool has_else = (n->next != nullptr);

        generate_node_code(n->left, ctx);
        ctx.test_al_al();
        ctx.jcc_rel32(TOKEN_FALSE, true, has_else ? else_label : end_if_label);

        generate_node_code(n->right, ctx);

        if (has_else)
        {
            ctx.jmp_rel32(end_if_label);
            ctx.place_label(else_label);
            generate_node_code(n->next, ctx);
        }
        ctx.place_label(end_if_label);
    }
    break;
    case TOKEN_WHILE:
    {
        int loop_start_label = ctx.new_label();
        int loop_test_label = ctx.new_label();

        ctx.jmp_rel32(loop_test_label);
        ctx.place_label(loop_start_label);
        generate_node_code(n->right, ctx);
        ctx.place_label(loop_test_label);
        generate_node_code(n->left, ctx);
        ctx.test_al_al();
        ctx.jcc_rel32(TOKEN_TRUE, true, loop_start_label);
    }
    break;
    case TOKEN_BLOCK:
        generate_node_code(n->left, ctx);
        break;

    default:
        cerr << "Error: node type: " << n->token.id << endl;
        ctx.binary.push_back(0xCC);
    }

    if (n->next != nullptr && n->token.id != TOKEN_IF)
    {
        generate_node_code(n->next, ctx);
    }
}

void generate_program_code(node* program_ast_head, vector<uint8_t>& binary, vector<symbol_data>& symbols)
{
    binary.clear();
    code_gen ctx(binary, symbols);

    ctx.push_ebx();
    binary.push_back(0x55);
    binary.push_back(0x89); binary.push_back(0xE5);

    int local_var_count = 0;
    for (const auto& sym : symbols)
    {
        if (sym.sym_type == symbol_var)
        {
            local_var_count++;
        }
    }
    int local_var_space = local_var_count * 4;
    if (local_var_space > 0)
    {
        binary.push_back(0x81); binary.push_back(0xEC);
        ctx.append_int32(local_var_space);
    }
    generate_node_code(program_ast_head, ctx);

    ctx.jump();

    binary.push_back(0x89); binary.push_back(0xEC);
    binary.push_back(0x5D);
    ctx.pop_ebx();
    binary.push_back(0xC3);
}