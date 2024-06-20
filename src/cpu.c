#include "cpu.h"
#include "memory.h"
#include <stdint.h>
#include <stdio.h>

#define MULTI_MACRO(m) m(a) m(f) m(b) m(c) m(d) m(e) m(h) m(l)
#define INC(r)                                                                 \
    void inc_##r() { registers.r++; }
#define DEC(r)                                                                 \
    void dec_##r() { registers.r--; }
#define ADD(r)                                                                 \
    void add_##r() { registers.a = add8(registers.a, registers.r); }
#define SUB(r)                                                                 \
    void sub_##r() { registers.a = sub8(registers.a, registers.r); }
#define ADC(r)                                                                 \
    void adc_##r() {                                                             \
        uint8_t c = (registers.f & FLAGS_CARRY) == FLAGS_CARRY;                    \
        registers.a = adc8(registers.a, registers.r, c);                           \
    }
#define SBC(r)                                                                 \
    void sbc_##r() {                                                             \
        uint8_t c = (registers.f & FLAGS_CARRY) == FLAGS_CARRY;                    \
        registers.a = subc8(registers.a, registers.r, c);                          \
    }
#define CP(r)                                                                  \
    void cp_##r() { sub8(registers.a, registers.r); }
#define AND(r)                                                                 \
    void and_##r() {                                                             \
        uint8_t result;                                                            \
        registers.a &= registers.r;                                                \
        set_logic_flags(result);                                                   \
    }
#define OR(r)                                                                  \
    void or_##r() {                                                              \
        uint8_t result;                                                            \
        registers.a |= registers.r;                                                \
        set_logic_flags(result);                                                   \
    }
#define XOR(r)                                                                 \
    void xor_##r() {                                                             \
        uint8_t result;                                                            \
        registers.a ^= registers.r;                                                \
        set_logic_flags(result);                                                   \
    }

uint8_t m8 = 0;
uint16_t m16 = 0;
struct registers registers;
struct instruction instructions[256] = {
    {"NOP", 0, nop},               // 0x00
    {"LD BC d16", 2, ld_bc_nn},    // 0x01
    {"LD (BC) A", 0, ld_bcm_a},    // 0x02
    {"INC BC", 0, inc_bc},         // 0x03
    {"INC B", 0, inc_b},           // 0x04
    {"DEC B", 0, dec_b},           // 0x05
    {"LD B d8", 1, ld_b_n8},       // 0x06
    {"RLCA", 0, rlca},             // 0x07
    {"LD (a16) SP", 2, ld_nn_sp},  // 0x08
    {"ADD HL BC", 0, add_bc},        // 0x09
    {"LD A (BC)", 0, ld_a_bcm},    // 0x0A
    {"DEC BC", 0, dec_bc},         // 0x0B
    {"INC C", 0, inc_c},           // 0x0C
    {"DEC C", 0, dec_c},           // 0x0D
    {"LD C d8", 1, ld_c_n8},       // 0x0E
    {"RRCA", 0, rrca},             // 0x0F
    {"STOP 0", 0, stop},           // 0x10
    {"LD DE d16", 2, ld_de_nn},    // 0x11
    {"LD (DE) A", 0, ld_dem_a},    // 0x12
    {"INC DE", 0, inc_de},         // 0x13
    {"INC D", 0, inc_d},           // 0x14
    {"DEC D", 0, dec_d},           // 0x15
    {"LD D d8", 1, ld_d_n8},       // 0x16
    {"RLA", 0, rla},              // 0x17
    {"JR r8", 1, jr_e},            // 0x18
    {"ADD HL DE", 0, add_de},        // 0x19
    {"LD A (DE)", 0, ld_a_dem},    // 0x1A
    {"DEC DE", 0, dec_de},         // 0x1B
    {"INC E", 0, inc_e},           // 0x1C
    {"DEC E", 0, dec_e},           // 0x1D
    {"LD E d8", 1, ld_e_n8},       // 0x1E
    {"RRA", 0, rra},              // 0x1F
    {"JR NZ r8", 1, jp_nz_nn},         // 0x20
    {"LD HL d16", 2, ld_hl_nn},    // 0x21
    {"LD (HL+) A", 0, ld_hli_a},   // 0x22
    {"INC HL", 0, inc_hl},         // 0x23
    {"INC H", 0, inc_h},           // 0x24
    {"DEC H", 0, dec_h},           // 0x25
    {"LD H d8", 1, ld_h_n8},          // 0x26
    {"DAA", 0, daa},               // 0x27
    {"JR Z r8", 1, jr_z_e},          // 0x28
    {"ADD HL HL", 0, add_hl},        // 0x29
    {"LD A (HL+)", 0, ld_a_hli},   // 0x2A
    {"DEC HL", 0, dec_hl},         // 0x2B
    {"INC L", 0, inc_l},           // 0x2C
    {"DEC L", 0, dec_l},           // 0x2D
    {"LD L d8", 1, ld_l_n8},       // 0x2E
    {"CPL", 0, cpl},               // 0x2F
    {"JR NC r8", 1, jr_nc_e},         // 0x30
    {"LD SP d16", 2, ld_sp_n16},   // 0x31
    {"LD (HL-) A", 0, ld_hld_a},   // 0x32
    {"INC SP", 0, inc_sp},         // 0x33
    {"INC (HL)", 0, inc_hlm},      // 0x34
    {"DEC (HL)", 0, dec_hlm},      // 0x35
    {"LD (HL) d8", 1, ld_hl_n8},       // 0x36
    {"SCF", 0, scf},               // 0x37
    {"JR C r8", 1, jr_c_e},          // 0x38
    {"ADD HL SP", 0, add_sp},        // 0x39
    {"LD A (HL-)", 0, ld_a_hld},   // 0x3A
    {"DEC SP", 0, dec_sp},         // 0x3B
    {"INC A", 0, inc_a},           // 0x3C
    {"DEC A", 0, dec_a},           // 0x3D
    {"LD A d8", 1, ld_a_n8},       // 0x3E
    {"CCF", 0, ccf},               // 0x3F
    {"LD B B", 0, ld_b_b},         // 0x40
    {"LD B C", 0, ld_b_c},         // 0x41
    {"LD B D", 0, ld_b_d},         // 0x42
    {"LD B E", 0, ld_b_e},         // 0x43
    {"LD B H", 0, ld_b_h},         // 0x44
    {"LD B L", 0, ld_b_l},         // 0x45
    {"LD B (HL)", 0, ld_b_hl},     // 0x46
    {"LD B A", 0, ld_b_a},         // 0x47
    {"LD C B", 0, ld_c_b},         // 0x48
    {"LD C C", 0, ld_c_c},         // 0x49
    {"LD C D", 0, ld_c_d},         // 0x4A
    {"LD C E", 0, ld_c_e},         // 0x4B
    {"LD C H", 0, ld_c_h},         // 0x4C
    {"LD C L", 0, ld_c_l},         // 0x4D
    {"LD C (HL)", 0, ld_c_hl},     // 0x4E
    {"LD C A", 0, ld_c_a},         // 0x4F
    {"LD D B", 0, ld_d_b},         // 0x50
    {"LD D C", 0, ld_d_c},         // 0x51
    {"LD D D", 0, ld_d_d},         // 0x52
    {"LD D E", 0, ld_d_e},         // 0x53
    {"LD D H", 0, ld_d_h},         // 0x54
    {"LD D L", 0, ld_d_l},         // 0x55
    {"LD D (HL)", 0, ld_d_hl},     // 0x56
    {"LD D A", 0, ld_d_a},         // 0x57
    {"LD E B", 0, ld_e_b},         // 0x58
    {"LD E C", 0, ld_e_c},         // 0x59
    {"LD E D", 0, ld_e_d},         // 0x5A
    {"LD E E", 0, ld_e_e},         // 0x5B
    {"LD E H", 0, ld_e_h},         // 0x5C
    {"LD E L", 0, ld_e_l},         // 0x5D
    {"LD E (HL)", 0, ld_e_hl},     // 0x5E
    {"LD E A", 0, ld_e_a},         // 0x5F
    {"LD H B", 0, ld_h_b},         // 0x60
    {"LD H C", 0, ld_h_c},         // 0x61
    {"LD H D", 0, ld_h_d},         // 0x62
    {"LD H E", 0, ld_h_e},         // 0x63
    {"LD H H", 0, ld_h_h},         // 0x64
    {"LD H L", 0, ld_h_l},         // 0x65
    {"LD H (HL)", 0, ld_h_hl},     // 0x66
    {"LD H A", 0, ld_h_a},         // 0x67
    {"LD L B", 0, ld_l_b},         // 0x68
    {"LD L C", 0, ld_l_c},         // 0x69
    {"LD L D", 0, ld_l_d},         // 0x6A
    {"LD L E", 0, ld_l_e},         // 0x6B
    {"LD L H", 0, ld_l_h},         // 0x6C
    {"LD L L", 0, ld_l_l},         // 0x6D
    {"LD L (HL)", 0, ld_l_hl},     // 0x6E
    {"LD L A", 0, ld_l_a},         // 0x6F
    {"LD (HL) B", 0, ld_hl_b},     // 0x70
    {"LD (HL) C", 0, ld_hl_c},     // 0x71
    {"LD (HL) D", 0, ld_hl_d},     // 0x72
    {"LD (HL) E", 0, ld_hl_e},     // 0x73
    {"LD (HL) H", 0, ld_hl_h},     // 0x74
    {"LD (HL) L", 0, ld_hl_l},     // 0x75
    {"HALT", 0, halt},             // 0x76
    {"LD (HL) A", 0, ld_hl_a},     // 0x77
    {"LD A B", 0, ld_a_b},         // 0x78
    {"LD A C", 0, ld_a_c},         // 0x79
    {"LD A D", 0, ld_a_d},         // 0x7A
    {"LD A E", 0, ld_a_e},         // 0x7B
    {"LD A H", 0, ld_a_h},         // 0x7C
    {"LD A L", 0, ld_a_l},         // 0x7D
    {"LD A (HL)", 0, ld_a_hl},     // 0x7E
    {"LD A A", 0, ld_a_a},         // 0x7F
    {"ADD A B", 0, add_b},         // 0x80
    {"ADD A C", 0, add_c},         // 0x81
    {"ADD A D", 0, add_d},         // 0x82
    {"ADD A E", 0, add_e},         // 0x83
    {"ADD A H", 0, add_h},         // 0x84
    {"ADD A L", 0, add_l},         // 0x85
    {"ADD A (HL)", 0, add_hlm},    // 0x86
    {"ADD A A", 0, add_a},         // 0x87
    {"ADC A B", 0, adc_b},         // 0x88
    {"ADC A C", 0, adc_c},         // 0x89
    {"ADC A D", 0, adc_d},         // 0x8A
    {"ADC A E", 0, adc_e},         // 0x8B
    {"ADC A H", 0, adc_h},         // 0x8C
    {"ADC A L", 0, adc_l},         // 0x8D
    {"ADC A (HL)", 0, adc_hl},     // 0x8E
    {"ADC A A", 0, adc_a},         // 0x8F
    {"SUB B", 0, sub_b},           // 0x90
    {"SUB C", 0, sub_c},           // 0x91
    {"SUB D", 0, sub_d},           // 0x92
    {"SUB E", 0, sub_e},           // 0x93
    {"SUB H", 0, sub_h},           // 0x94
    {"SUB L", 0, sub_l},           // 0x95
    {"SUB (HL)", 0, sub_hl},       // 0x96
    {"SUB A", 0, sub_a},           // 0x97
    {"SBC A B", 0, scb_b},         // 0x98
    {"SBC A C", 0, scb_c},         // 0x99
    {"SBC A D", 0, scb_d},         // 0x9A
    {"SBC A E", 0, scb_e},         // 0x9B
    {"SBC A H", 0, scb_h},         // 0x9C
    {"SBC A L", 0, scb_l},         // 0x9D
    {"SBC A (HL)", 0, scb_hl},     // 0x9E
    {"SBC A A", 0, scb_a},         // 0x9F
    {"AND B", 0, and_b},           // 0xA0
    {"AND C", 0, and_c},           // 0xA1
    {"AND D", 0, and_d},           // 0xA2
    {"AND E", 0, and_e},           // 0xA3
    {"AND H", 0, and_h},           // 0xA4
    {"AND L", 0, and_l},           // 0xA5
    {"AND (HL)", 0, and_hl},       // 0xA6
    {"AND A", 0, and_a},           // 0xA7
    {"XOR B", 0, xor_b},           // 0xA8
    {"XOR C", 0, xor_c},           // 0xA9
    {"XOR D", 0, xor_d},           // 0xAA
    {"XOR E", 0, xor_e},           // 0xAB
    {"XOR H", 0, xor_h},           // 0xAC
    {"XOR L", 0, xor_l},           // 0xAD
    {"XOR (HL)", 0, xor_hl},       // 0xAE
    {"XOR A", 0, xor_a},           // 0xAF
    {"OR B", 0, or_b},             // 0xB0
    {"OR C", 0, or_c},             // 0xB1
    {"OR D", 0, or_d},             // 0xB2
    {"OR E", 0, or_e},             // 0xB3
    {"OR H", 0, or_h},             // 0xB4
    {"OR L", 0, or_l},             // 0xB5
    {"OR (HL)", 0, or_hl},         // 0xB6
    {"OR A", 0, or_a},             // 0xB7
    {"CP B", 0, cp_b},             // 0xB8
    {"CP C", 0, cp_c},             // 0xB9
    {"CP D", 0, cp_d},             // 0xBA
    {"CP E", 0, cp_e},             // 0xBB
    {"CP H", 0, cp_h},             // 0xBC
    {"CP L", 0, cp_l},             // 0xBD
    {"CP (HL)", 0, cp_hl},         // 0xBE
    {"CP A", 0, cp_a},             // 0xBF
    {"RET NZ", 0, ret_nz},           // 0xC0
    {"POP BC", 0, pop_bc},         // 0xC1
    {"JP NZ a16", 2, jp_nz_nn},        // 0xC2
    {"JNULLP a16", 2, jp_nn},           // 0xC3
    {"CALL NZ a16", 2, call_nz_nn},      // 0xC4
    {"PUSH BC", 0, push_bc},       // 0xC5
    {"ADD A d8", 1, add_n},        // 0xC6
    {"RST 00H", 0, rst_00},          // 0xC7
    {"RET Z", 0, ret_z},            // 0xC8
    {"RET", 0, ret},              // 0xC9
    {"JP Z a16", 2, jp_z_nn},         // 0xCA
    {"PREFIX CB", 0, NULL},        // 0xCB
    {"CALL Z a16", 2, call_z_nn},       // 0xCC
    {"CALL a16", 2, call_nn},         // 0xCD
    {"ADC A d8", 1, adc_n},         // 0xCE
    {"RST 08H", 0, rst_08},          // 0xCF
    {"RET NC", 0, ret_nc},           // 0xD0
    {"POP DE", 0, pop_de},         // 0xD1
    {"JP NC a16", 2, jp_nc_nn},        // 0xD2
    {"CALL NC a16", 2, call_nc_nn},      // 0xD4
    {"PUSH DE", 0, push_de},       // 0xD5
    {"SUB d8", 1, sub_n},          // 0xD6
    {"RST 10H", 0, rst_10},          // 0xD7
    {"RET C", 0, ret_c},            // 0xD8
    {"RETI", 0, reti},             // 0xD9
    {"JP C a16", 2, jp_c_nn},         // 0xDA
    {"CALL C a16", 2, call_c_nn},       // 0xDC
    {"SBC A d8", 1, scb_n},        // 0xDE
    {"RST 18H", 0, rst_18},          // 0xDF
    {"LDH (a8) A", 1, ldh_n_a},    // 0xE0
    {"POP HL", 0, pop_hl},         // 0xE1
    {"LD (C) A", 0, ldh_c_a},      // 0xE2
    {"PUSH HL", 0, push_hl},       // 0xE5
    {"AND d8", 1, and_n},          // 0xE6
    {"RST 20H", 0, rst_20},          // 0xE7
    {"ADD SP r8", 1, add_sp_n},        // 0xE8
    {"JP (HL)", 0, jp_hl},          // 0xE9
    {"LD (a16) A", 2, ld_nn_a},    // 0xEA
    {"XOR d8", 1, xor_n},           // 0xEE
    {"RST 28H", 0, rst_28},          // 0xEF
    {"LDH A (a8)", 1, ldh_a_n},    // 0xF0
    {"POP AF", 0, pop_af},         // 0xF1
    {"LD A (C)", 0, ldh_a_c},      // 0xF2
    {"DI", 0, di},               // 0xF3
    {"PUSH AF", 0, push_af},       // 0xF5
    {"OR d8", 1, or_n},            // 0xF6
    {"RST 30H", 0, rst_30},          // 0xF7
    {"LD HL SP+r8", 1, ld_hl_spe}, // 0xF8
    {"LD SP HL", 0, ld_sp_hl},         // 0xF9
    {"LD A (a16)", 2, ld_a_nn},    // 0xFA
    {"EI", 0, ei},               // 0xFB
    {"CP d8", 1, cp_n},            // 0xFE
    {"RST 38H", 0, rst_38},          // 0xFF
};

bool execute() {
    uint8_t op = read8(pc);
    struct instruction instruction = instructions[op];

    if (instruction.operand_size == 1)
        m8 = read8(++pc);
    if (instruction.operand_size == 2) {
        pc += 2;
        m16 = read16(pc);
    }

    if (instruction.fn != NULL) {
        instruction.fn();
    } else {
        printf("error reading instruction: %s\n", instruction.dissassembly);
        return false;
    }
    pc++;
    return true;
}

uint8_t add8(uint8_t a, uint8_t b) {
    uint8_t result;
    result = a + b;
    set_add_flags(result, a, b);
    return result;
}

uint8_t adc8(uint8_t a, uint8_t b, uint8_t c) {
    uint8_t result;
    result = a + b + c;
    set_add_flags(result, a, b);
    return result;
}

uint8_t sub8(uint8_t a, uint8_t b) {
    uint8_t result;
    result = a - b;
    set_sub_flags(result, a, b);
    return result;
}
uint8_t subc8(uint8_t a, uint8_t b, uint8_t c) {
    uint8_t result;
    result = a - b - c;
    set_sub_flags(result, a, b);
    return result;
}

void set_add_flags(uint8_t result, uint8_t a, uint8_t b) {
    if (result == 0) {
        registers.f |= FLAGS_ZERO;
    }
    registers.f &= ~FLAGS_NEGATIVE;
    if ((((a & 0x0F) + (b & 0x0F)) & 0x10) > 0) {
        registers.f |= FLAGS_HALFCARRY;
    }
    if (result < a || result < b) {
        registers.f |= FLAGS_CARRY;
    }
}

void set_sub_flags(uint8_t result, uint8_t a, uint8_t b) {
    if (result == 0) {
        registers.f |= FLAGS_ZERO;
    }
    registers.f &= ~FLAGS_NEGATIVE;
    if ((((a & 0x0F) - (b & 0x0F)) & 0x10) > 0) {
        registers.f |= FLAGS_HALFCARRY;
    }
    if (result < a || result < b) {
        registers.f |= FLAGS_CARRY;
    }
}

void set_logic_flags(uint8_t result) {
    registers.f |= result == 0 ? FLAGS_ZERO : 0;
    registers.f &= ~FLAGS_NEGATIVE;
    registers.f |= FLAGS_HALFCARRY;
    registers.f &= ~FLAGS_CARRY;
}

void set_add16_flags(uint16_t result, uint16_t a, uint16_t b) {
    registers.f &= ~FLAGS_NEGATIVE;
    if ((((a & 0x00FF) + (b & 0x00FF)) & 0x10) > 0) {
        registers.f |= FLAGS_HALFCARRY;
    }

    if (result < a || result < b) {
        registers.f |= FLAGS_CARRY;
    }
}

void nop() {}

MULTI_MACRO(INC)
    void inc_af() { registers.af++; }
    void inc_bc() { registers.bc++; }
    void inc_de() { registers.de++; }
    void inc_hl() { registers.hl++; }
    void inc_sp() { registers.sp++; }

    void inc_hlm() { write8(registers.hl, read8(registers.hl) - 1); }

MULTI_MACRO(DEC)
    void dec_af() { registers.af--; }
    void dec_bc() { registers.bc--; }
    void dec_de() { registers.de--; }
    void dec_hl() { registers.hl--; }
    void dec_sp() { registers.sp--; }

    void dec_hlm() { write8(registers.hl, read8(registers.hl) - 1); }

    void ld_bc_n16() { registers.bc = m16; }
    void ld_b_n8() { registers.b = m8; }
    void ld_c_n8() { registers.c = m8; }
    void ld_de_n16() { registers.de = m16; }
    void ld_d_n8() { registers.d = m8; }
    void ld_e_n8() { registers.e = m8; }
    void ld_hl_n16() { registers.hl = m16; }
    void ld_h_n8() { registers.h = m8; }
    void ld_l_n8() { registers.l = m8; }
    void ld_sp_n16() { registers.sp = m16; }
    void ld_a_n8() { registers.a = m8; }

    void ld_b_b() { registers.b = registers.b; }
    void ld_b_c() { registers.b = registers.c; }
    void ld_b_d() { registers.b = registers.d; }
    void ld_b_e() { registers.b = registers.e; }
    void ld_b_h() { registers.b = registers.h; }
    void ld_b_l() { registers.b = registers.l; }
    void ld_b_a() { registers.b = registers.a; }
    void ld_c_b() { registers.c = registers.b; }
    void ld_c_c() { registers.c = registers.c; }
    void ld_c_d() { registers.c = registers.d; }
    void ld_c_e() { registers.c = registers.e; }
    void ld_c_h() { registers.c = registers.h; }
    void ld_c_l() { registers.c = registers.l; }
    void ld_c_a() { registers.c = registers.a; }
    void ld_d_b() { registers.d = registers.b; }
    void ld_d_c() { registers.d = registers.c; }
    void ld_d_d() { registers.d = registers.d; }
    void ld_d_e() { registers.d = registers.e; }
    void ld_d_h() { registers.d = registers.h; }
    void ld_d_l() { registers.d = registers.l; }
    void ld_d_a() { registers.d = registers.a; }
    void ld_e_b() { registers.e = registers.b; }
    void ld_e_c() { registers.e = registers.c; }
    void ld_e_d() { registers.e = registers.d; }
    void ld_e_e() { registers.e = registers.e; }
    void ld_e_h() { registers.e = registers.h; }
    void ld_e_l() { registers.e = registers.l; }
    void ld_e_a() { registers.e = registers.a; }
    void ld_h_b() { registers.h = registers.b; }
    void ld_h_c() { registers.h = registers.c; }
    void ld_h_d() { registers.h = registers.d; }
    void ld_h_e() { registers.h = registers.e; }
    void ld_h_h() { registers.h = registers.h; }
    void ld_h_l() { registers.h = registers.l; }
    void ld_h_a() { registers.h = registers.a; }
    void ld_l_b() { registers.l = registers.b; }
    void ld_l_c() { registers.l = registers.c; }
    void ld_l_d() { registers.l = registers.d; }
    void ld_l_e() { registers.l = registers.e; }
    void ld_l_h() { registers.l = registers.h; }
    void ld_l_l() { registers.l = registers.l; }
    void ld_l_a() { registers.l = registers.a; }
    void ld_a_b() { registers.a = registers.b; }
    void ld_a_c() { registers.a = registers.c; }
    void ld_a_d() { registers.a = registers.d; }
    void ld_a_e() { registers.a = registers.e; }
    void ld_a_h() { registers.a = registers.h; }
    void ld_a_l() { registers.a = registers.l; }
    void ld_a_a() { registers.a = registers.a; }

    void ld_a_hl() { registers.a = read8(registers.hl); }
    void ld_b_hl() { registers.b = read8(registers.hl); }
    void ld_c_hl() { registers.c = read8(registers.hl); }
    void ld_d_hl() { registers.d = read8(registers.hl); }
    void ld_e_hl() { registers.e = read8(registers.hl); }
    void ld_h_hl() { registers.h = read8(registers.hl); }
    void ld_l_hl() { registers.l = read8(registers.hl); }

    void ld_hl_a() { write8(registers.hl, registers.a); }
    void ld_hl_b() { write8(registers.hl, registers.b); }
    void ld_hl_c() { write8(registers.hl, registers.c); }
    void ld_hl_d() { write8(registers.hl, registers.d); }
    void ld_hl_e() { write8(registers.hl, registers.e); }
    void ld_hl_h() { write8(registers.hl, registers.h); }
    void ld_hl_l() { write8(registers.hl, registers.l); }

    void ld_hl_n8() { write8(registers.hl, m8); }

    void ld_a_bcm() { registers.a = read8(registers.bc); }
    void ld_a_dem() { registers.a = read8(registers.de); }

    void ld_bcm_a() { write8(registers.bc, registers.a); }
    void ld_dem_a() { write8(registers.de, registers.a); }

    void ld_a_nn() { registers.a = read8(m16); }
    void ld_nn_a() { write8(m16, registers.a); }

    void ldh_a_c() { registers.a = read8(u16(m8, 0xFF)); }
    void ldh_c_a() { write8(u16(m8, 0xFF), registers.a); }
    void ldh_a_n() { registers.a = read8(u16(m8, 0xFF)); }
    void ldh_n_a() { write8(u16(m8, 0xFF), registers.a); }

    void ld_a_hld() { registers.a = read8(registers.hl--); }
    void ld_hld_a() { write8(registers.hl--, registers.a); }
    void ld_a_hli() { registers.a = read8(registers.hl++); }
    void ld_hli_a() { write8(registers.hl++, registers.a); }

    void ld_bc_nn() { registers.bc = m16; }
    void ld_af_nn() { registers.af = m16; }
    void ld_de_nn() { registers.de = m16; }
    void ld_hl_nn() { registers.hl = m16; }

    void ld_nn_sp() {
        write8(m16, lsb(registers.sp));
        write8(m16 + 1, msb(registers.sp));
    }
void ld_sp_hl() { registers.sp = registers.hl; }

void push_bc() {
    registers.sp--;
    write8(registers.sp--, registers.b);
    write8(registers.sp, registers.c);
}
void push_af() {
    registers.sp--;
    write8(registers.sp--, registers.a);
    write8(registers.sp, registers.f);
}
void push_de() {
    registers.sp--;
    write8(registers.sp--, registers.d);
    write8(registers.sp, registers.e);
}
void push_hl() {
    registers.sp--;
    write8(registers.sp--, registers.h);
    write8(registers.sp, registers.l);
}

void pop_bc() {
    uint8_t lsb = read8(registers.sp++);
    uint8_t msb = read8(registers.sp++);
    registers.bc = u16(lsb, msb);
}
void pop_af() {
    uint8_t lsb = read8(registers.sp++);
    uint8_t msb = read8(registers.sp++);
    registers.af = u16(lsb, msb);
}
void pop_de() {
    uint8_t lsb = read8(registers.sp++);
    uint8_t msb = read8(registers.sp++);
    registers.de = u16(lsb, msb);
}
void pop_hl() {
    uint8_t lsb = read8(registers.sp++);
    uint8_t msb = read8(registers.sp++);
    registers.hl = u16(lsb, msb);
}

void ld_hl_spe() {
    uint16_t result;
    const uint16_t a = registers.sp;
    const int8_t b = m8;
    result = a + b; 
    registers.hl = result;

    if (result & 0xFFFF0000) {
        registers.f |= FLAGS_CARRY;
    } else {
        registers.f &= ~FLAGS_CARRY;
    }

    if (((a & 0x0F) + (b & 0x0F)) > 0x0F) {
        registers.f |= FLAGS_HALFCARRY;
    } else {
        registers.f &= ~FLAGS_HALFCARRY;
    }

    registers.f &= ~(FLAGS_ZERO | FLAGS_NEGATIVE);
}

MULTI_MACRO(ADD)
    void add_hlm() { registers.a = add8(registers.a, read8(registers.hl)); }
    void add_n() { registers.a = add8(registers.a, m8); }

MULTI_MACRO(ADC)
    void adc_hl() {
        uint8_t c = (registers.f & FLAGS_CARRY) == FLAGS_CARRY;
        registers.c = adc8(registers.a, read8(registers.hl), c);
    }
void adc_n() {
    uint8_t c = (registers.f & FLAGS_CARRY) == FLAGS_CARRY;
    registers.a = adc8(registers.a, m8, c);
}

MULTI_MACRO(SUB)
    void sub_hl() { registers.a = sub8(registers.a, read8(registers.hl)); }
    void sub_n() { registers.a = sub8(registers.a, m8); }

MULTI_MACRO(SBC)
    void scb_hl() {
        uint8_t c = (registers.f & FLAGS_CARRY) == FLAGS_CARRY;
        registers.a = subc8(registers.a, read8(registers.hl), c);
    }
void scb_n() {
    uint8_t c = (registers.f & FLAGS_CARRY) == FLAGS_CARRY;
    registers.a = subc8(registers.a, m8, c);
}

MULTI_MACRO(CP)
    void cp_hl() { sub8(registers.a, read8(registers.hl)); }
    void cp_n() { sub8(registers.a, m8); }

MULTI_MACRO(AND)
    void and_hl() {
        uint8_t result;
        registers.a &= read8(registers.hl);
        set_logic_flags(result);
    }
void and_n() {
    uint8_t result;
    registers.a &= m8;
    set_logic_flags(result);
}

MULTI_MACRO(OR)
    void or_hl() {
        uint8_t result;
        result = registers.a | read8(registers.hl);
        set_logic_flags(result);
    }
void or_n() {
    uint8_t result;
    result = registers.a | m8;
    set_logic_flags(result);
}

MULTI_MACRO(XOR)
    void xor_hl() {
        uint8_t result;
        result = registers.a ^ read8(registers.hl);
        set_logic_flags(result);
    }
void xor_n() {
    uint8_t result;
    result = registers.a ^ m8;
    set_logic_flags(result);
}

void ccf() {
    registers.f &= ~FLAGS_ZERO;
    registers.f &= ~FLAGS_HALFCARRY;
    registers.f ^= FLAGS_CARRY;
}
void scf() {
    registers.f &= ~FLAGS_ZERO;
    registers.f &= ~FLAGS_HALFCARRY;
    registers.f |= FLAGS_CARRY;
}
void daa() {
    unsigned short s = registers.a;
    const bool is_halfcarry = (registers.f & FLAGS_HALFCARRY) != 0;
    const bool is_carry = (registers.f & FLAGS_CARRY) != 0;
    const bool is_negative = (registers.f & FLAGS_NEGATIVE) != 0;

    if (is_negative) {
        if (is_halfcarry) s = (s - 0x06) & 0x0FF;
        if (is_carry) s -= 0x60;
    } else {
        if (is_halfcarry || (s & 0xF) > 9) s += 0x06;
        if (is_carry || s > 0x9F) s += 0x60;
    }

    registers.a = s;
    registers.f &= ~FLAGS_HALFCARRY;

    if (registers.a) {
        registers.f &= ~FLAGS_ZERO;
    } else {
        registers.f |= FLAGS_ZERO;
    }

    if (s >= 0x100) {
        registers.f |= FLAGS_CARRY;
    }
}
void cpl() {
    registers.a = ~registers.a;
    registers.f |= FLAGS_NEGATIVE;
    registers.f |= FLAGS_HALFCARRY;
}

void add_hl() {
    uint16_t result;
    const uint16_t a = registers.hl;
    const uint16_t b = registers.hl;

    result = a + b;
    set_add16_flags(result, a, b);
}
void add_sp() {
    uint16_t result;
    const uint16_t a = registers.hl;
    const uint16_t b = registers.sp;

    result = a + b;
    set_add16_flags(result, a, b);
}
void add_bc() {
    uint16_t result;
    const uint16_t a = registers.hl;
    const uint16_t b = registers.bc;

    result = a + b;;
    set_add16_flags(result, a, b);
}
void add_de() {
    uint16_t result;
    const uint16_t a = registers.hl;
    const uint16_t b = registers.de;

    result = a + b;
    set_add16_flags(result, a, b);
}
void add_af() {
    uint16_t result;
    const uint16_t a = registers.hl;
    const uint16_t b = registers.af;

    result = a + b;
    set_add16_flags(result, a, b);
}

void add_sp_n() {
    uint16_t result;
    const uint16_t a = registers.sp;
    int8_t b = (int8_t)m8;

    result = registers.sp + b;
    registers.f &= ~FLAGS_ZERO;
    registers.f &= ~FLAGS_NEGATIVE;

    if ((((a & 0x0F) + (b & 0x0F)) & 0x10) > 0) {
        registers.f |= FLAGS_HALFCARRY;
    }
    if (result < a || result < b) {
        registers.f |= FLAGS_CARRY;
    }
}

void rlca() {
    uint8_t l = (registers.a & 0x80) >> 7;
    if (l) {
        registers.f |= FLAGS_CARRY;
    } else {
        registers.f &= ~FLAGS_CARRY;
    }

    registers.a <<= 1;
    registers.a += l;

    registers.f &= ~(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY);
}

void rrca() {
    uint8_t l = registers.a & 0x01;
    if (l) {
        registers.f |= FLAGS_CARRY;
    } else {
        registers.f &= ~FLAGS_CARRY;
    }

    registers.a >>= 1;
    if (l) {
        registers.a |= 0x80;
    }

    registers.f &= ~(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY);
}

void rla() {
    int carry = (registers.f & FLAGS_CARRY) ? 1 : 0;

    if (registers.a & 0x80) {
        registers.f |= FLAGS_CARRY;
    } else {
        registers.f &= ~FLAGS_CARRY;
    }

    registers.a <<= 1;
    registers.a += carry;

    registers.f &= ~(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY);
}

void rra() {
    int carry = (registers.f & FLAGS_CARRY) ? 1 : 0;
    carry <<= 7;

    if (registers.a & 0x01) {
        registers.f |= FLAGS_CARRY;
    } else {
        registers.f &= ~FLAGS_CARRY;
    }

    registers.a >>= 1;
    registers.a += carry;

    registers.f &= ~(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY);
}

void jp_nn() {
    pc = m16;
}
void jp_hl() {
    pc = registers.hl;
}
void jp_nz_nn() {
    if ((registers.f & FLAGS_ZERO) == 0) {
        pc = m16;
    }
}
void jp_z_nn() {
    if ((registers.f & FLAGS_ZERO) != 0) {
        pc = m16;
    }
}
void jp_nc_nn() {
    if ((registers.f & FLAGS_CARRY) == 0) {
        pc = m16;
    }
}
void jp_c_nn() {
    if ((registers.f & FLAGS_CARRY) != 0) {
        pc = m16;
    }
}

void jr_e() {
    const int8_t e = m8;
    pc++;
    pc += e;
}
void jr_nz_e() {
    if ((registers.f & FLAGS_ZERO) == 0) {
        const int8_t e = m8;
        pc++;
        pc += e;
    }
}
void jr_z_e() {
    if ((registers.f & FLAGS_ZERO) != 0) {
        const int8_t e = m8;
        pc++;
        pc += e;
    }
}
void jr_nj_e() {
    if ((registers.f & FLAGS_CARRY) == 0) {
        const int8_t e = m8;
        pc++;
        pc += e;
    }
}
void jr_j_e() {
    if ((registers.f & FLAGS_CARRY) != 0) {
        const int8_t e = m8;
        pc++;
        pc += e;
    }
}

void call_nn() {
    registers.sp--;
    write8(registers.sp, msb(pc));
    registers.sp--;
    write8(registers.sp, lsb(pc));
    pc = m16;
}
void call_nz_nn() {
    if ((registers.f & FLAGS_ZERO) == 0) {
        registers.sp--;
        write8(registers.sp, msb(pc));
        registers.sp--;
        write8(registers.sp, lsb(pc));
        pc = m16;
    }
}
void call_z_nn() {
    if ((registers.f & FLAGS_ZERO) != 0) {
        registers.sp--;
        write8(registers.sp, msb(pc));
        registers.sp--;
        write8(registers.sp, lsb(pc));
        pc = m16;
    }
}
void call_nc_nn() {
    if ((registers.f & FLAGS_CARRY) == 0) {
        registers.sp--;
        write8(registers.sp, msb(pc));
        registers.sp--;
        write8(registers.sp, lsb(pc));
        pc = m16;
    }
}
void call_c_nn() {
    if ((registers.f & FLAGS_CARRY) != 0) {
        registers.sp--;
        write8(registers.sp, msb(pc));
        registers.sp--;
        write8(registers.sp, lsb(pc));
        pc = m16;
    }
}

void ret() {
    uint8_t lsb = read8(registers.sp++);
    uint8_t msb = read8(registers.sp++);
    pc = u16(lsb, msb);
}
void ret_nz() {
    if ((registers.f & FLAGS_ZERO) == 0) {
        uint8_t lsb = read8(registers.sp++);
        uint8_t msb = read8(registers.sp++);
        pc = u16(lsb, msb);
    }
}
void ret_z() {
    if ((registers.f & FLAGS_ZERO) != 0) {
        uint8_t lsb = read8(registers.sp++);
        uint8_t msb = read8(registers.sp++);
        pc = u16(lsb, msb);
    }
}
void ret_nc() {
    if ((registers.f & FLAGS_CARRY) == 0) {
        uint8_t lsb = read8(registers.sp++);
        uint8_t msb = read8(registers.sp++);
        pc = u16(lsb, msb);
    }
}
void ret_c() {
    if ((registers.f & FLAGS_CARRY) != 0) {
        uint8_t lsb = read8(registers.sp++);
        uint8_t msb = read8(registers.sp++);
        pc = u16(lsb, msb);
    }
}
void reti() {
    uint8_t lsb = read8(registers.sp++);
    uint8_t msb = read8(registers.sp++);
    pc = u16(lsb, msb);
    // TODO: IME = 1
}

void rst_00() {
    const uint8_t n = 0x00;
    registers.sp--;
    write8(registers.sp--, msb(pc));
    write8(registers.sp, lsb(pc));
    pc = u16(n, 0x00);
}
void rst_08() {
    const uint8_t n = 0x08;
    registers.sp--;
    write8(registers.sp--, msb(pc));
    write8(registers.sp, lsb(pc));
    pc = u16(n, 0x00);
}
void rst_10() {
    const uint8_t n = 0x10;
    registers.sp--;
    write8(registers.sp--, msb(pc));
    write8(registers.sp, lsb(pc));
    pc = u16(n, 0x00);
}
void rst_18() {
    const uint8_t n = 0x18;
    registers.sp--;
    write8(registers.sp--, msb(pc));
    write8(registers.sp, lsb(pc));
    pc = u16(n, 0x00);
}
void rst_20() {
    const uint8_t n = 0x20;
    registers.sp--;
    write8(registers.sp--, msb(pc));
    write8(registers.sp, lsb(pc));
    pc = u16(n, 0x00);
}
void rst_28() {
    const uint8_t n = 0x28;
    registers.sp--;
    write8(registers.sp--, msb(pc));
    write8(registers.sp, lsb(pc));
    pc = u16(n, 0x00);
}
void rst_30() {
    const uint8_t n = 0x30;
    registers.sp--;
    write8(registers.sp--, msb(pc));
    write8(registers.sp, lsb(pc));
    pc = u16(n, 0x00);
}
void rst_38() {
    const uint8_t n = 0x38;
    registers.sp--;
    write8(registers.sp--, msb(pc));
    write8(registers.sp, lsb(pc));
    pc = u16(n, 0x00);
}

void halt() {
    // TODO: IME = 0
}
void stop() {
    // TODO: IME = 0
}
void di() {
    // TODO: IME = 0
}
void ei() {
    // TODO IME_NEXT = 1
}


