#include "cpu.h"
#include "memory.h"
#include <stdio.h>

uint8_t a8 = 0;
uint16_t a16 = 0;
struct registers registers;
struct instruction instructions[256] = {
    {"NOP", 0, nop},           // 0x00 
    {"LD BC d16", 2, NULL},    // 0x01 
    {"LD (BC) A", 0, NULL},    // 0x02 
    {"INC BC", 0, inc_bc},       // 0x03 
    {"INC B", 0, inc_b},        // 0x04 
    {"DEC B", 0, dec_b},        // 0x05 
    {"LD B d8", 1, NULL},      // 0x06 
    {"RLCA", 0, NULL},         // 0x07 
    {"LD (a16) SP", 2, NULL},  // 0x08 
    {"ADD HL BC", 0, NULL},    // 0x09 
    {"LD A (BC)", 0, NULL},    // 0x0A 
    {"DEC BC", 0, dec_bc},       // 0x0B 
    {"INC C", 0, inc_c},        // 0x0C 
    {"DEC C", 0, dec_c},        // 0x0D 
    {"LD C d8", 1, NULL},      // 0x0E 
    {"RRCA", 0, NULL},         // 0x0F 
    {"STOP 0", 0, NULL},       // 0x10 
    {"LD DE d16", 2, NULL},    // 0x11 
    {"LD (DE) A", 0, NULL},    // 0x12 
    {"INC DE", 0, inc_de},       // 0x13 
    {"INC D", 0, inc_d},        // 0x14 
    {"DEC D", 0, dec_d},        // 0x15 
    {"LD D d8", 1, NULL},      // 0x16 
    {"RLA", 0, NULL},          // 0x17 
    {"JR r8", 1, NULL},        // 0x18 
    {"ADD HL DE", 0, NULL},    // 0x19 
    {"LD A (DE)", 0, NULL},    // 0x1A 
    {"DEC DE", 0, NULL},       // 0x1B 
    {"INC E", 0, inc_e},        // 0x1C 
    {"DEC E", 0, dec_e},        // 0x1D 
    {"LD E d8", 1, NULL},      // 0x1E 
    {"RRA", 0, NULL},          // 0x1F 
    {"JR NZ r8", 1, NULL},     // 0x20 
    {"LD HL d16", 2, NULL},    // 0x21 
    {"LD (HL+) A", 0, NULL},   // 0x22 
    {"INC HL", 0, inc_hl},       // 0x23 
    {"INC H", 0, inc_h},        // 0x24 
    {"DEC H", 0, dec_h},        // 0x25 
    {"LD H d8", 1, NULL},      // 0x26 
    {"DAA", 0, NULL},          // 0x27 
    {"JR Z r8", 1, NULL},      // 0x28 
    {"ADD HL HL", 0, NULL},    // 0x29 
    {"LD A (HL+)", 0, NULL},   // 0x2A 
    {"DEC HL", 0, dec_hl},       // 0x2B 
    {"INC L", 0, inc_l},        // 0x2C 
    {"DEC L", 0, dec_l},        // 0x2D 
    {"LD L d8", 1, NULL},      // 0x2E 
    {"CPL", 0, NULL},          // 0x2F 
    {"JR NC r8", 1, NULL},     // 0x30 
    {"LD SP d16", 2, NULL},    // 0x31 
    {"LD (HL-) A", 0, NULL},   // 0x32 
    {"INC SP", 0, inc_sp},       // 0x33 
    {"INC (HL)", 0, NULL},     // 0x34 
    {"DEC (HL)", 0, NULL},     // 0x35 
    {"LD (HL) d8", 1, NULL},   // 0x36 
    {"SCF", 0, NULL},          // 0x37 
    {"JR C r8", 1, NULL},      // 0x38 
    {"ADD HL SP", 0, NULL},    // 0x39 
    {"LD A (HL-)", 0, NULL},   // 0x3A 
    {"DEC SP", 0, dec_sp},       // 0x3B 
    {"INC A", 0, inc_a},        // 0x3C 
    {"DEC A", 0, dec_a},        // 0x3D 
    {"LD A d8", 1, NULL},      // 0x3E 
    {"CCF", 0, NULL},          // 0x3F 
    {"LD B B", 0, NULL},       // 0x40 
    {"LD B C", 0, NULL},       // 0x41 
    {"LD B D", 0, NULL},       // 0x42 
    {"LD B E", 0, NULL},       // 0x43 
    {"LD B H", 0, NULL},       // 0x44 
    {"LD B L", 0, NULL},       // 0x45 
    {"LD B (HL)", 0, NULL},    // 0x46 
    {"LD B A", 0, NULL},       // 0x47 
    {"LD C B", 0, NULL},       // 0x48 
    {"LD C C", 0, NULL},       // 0x49 
    {"LD C D", 0, NULL},       // 0x4A 
    {"LD C E", 0, NULL},       // 0x4B 
    {"LD C H", 0, NULL},       // 0x4C 
    {"LD C L", 0, NULL},       // 0x4D 
    {"LD C (HL)", 0, NULL},    // 0x4E 
    {"LD C A", 0, NULL},       // 0x4F 
    {"LD D B", 0, NULL},       // 0x50 
    {"LD D C", 0, NULL},       // 0x51 
    {"LD D D", 0, NULL},       // 0x52 
    {"LD D E", 0, NULL},       // 0x53 
    {"LD D H", 0, NULL},       // 0x54 
    {"LD D L", 0, NULL},       // 0x55 
    {"LD D (HL)", 0, NULL},    // 0x56 
    {"LD D A", 0, NULL},       // 0x57 
    {"LD E B", 0, NULL},       // 0x58 
    {"LD E C", 0, NULL},       // 0x59 
    {"LD E D", 0, NULL},       // 0x5A 
    {"LD E E", 0, NULL},       // 0x5B 
    {"LD E H", 0, NULL},       // 0x5C 
    {"LD E L", 0, NULL},       // 0x5D 
    {"LD E (HL)", 0, NULL},    // 0x5E 
    {"LD E A", 0, NULL},       // 0x5F 
    {"LD H B", 0, NULL},       // 0x60 
    {"LD H C", 0, NULL},       // 0x61 
    {"LD H D", 0, NULL},       // 0x62 
    {"LD H E", 0, NULL},       // 0x63 
    {"LD H H", 0, NULL},       // 0x64 
    {"LD H L", 0, NULL},       // 0x65 
    {"LD H (HL)", 0, NULL},    // 0x66 
    {"LD H A", 0, NULL},       // 0x67 
    {"LD L B", 0, NULL},       // 0x68 
    {"LD L C", 0, NULL},       // 0x69 
    {"LD L D", 0, NULL},       // 0x6A 
    {"LD L E", 0, NULL},       // 0x6B 
    {"LD L H", 0, NULL},       // 0x6C 
    {"LD L L", 0, NULL},       // 0x6D 
    {"LD L (HL)", 0, NULL},    // 0x6E 
    {"LD L A", 0, NULL},       // 0x6F 
    {"LD (HL) B", 0, NULL},    // 0x70 
    {"LD (HL) C", 0, NULL},    // 0x71 
    {"LD (HL) D", 0, NULL},    // 0x72 
    {"LD (HL) E", 0, NULL},    // 0x73 
    {"LD (HL) H", 0, NULL},    // 0x74 
    {"LD (HL) L", 0, NULL},    // 0x75 
    {"HALT", 0, NULL},         // 0x76 
    {"LD (HL) A", 0, NULL},    // 0x77 
    {"LD A B", 0, NULL},       // 0x78 
    {"LD A C", 0, NULL},       // 0x79 
    {"LD A D", 0, NULL},       // 0x7A 
    {"LD A E", 0, NULL},       // 0x7B 
    {"LD A H", 0, NULL},       // 0x7C 
    {"LD A L", 0, NULL},       // 0x7D 
    {"LD A (HL)", 0, NULL},    // 0x7E 
    {"LD A A", 0, NULL},       // 0x7F 
    {"ADD A B", 0, NULL},      // 0x80 
    {"ADD A C", 0, NULL},      // 0x81 
    {"ADD A D", 0, NULL},      // 0x82 
    {"ADD A E", 0, NULL},      // 0x83 
    {"ADD A H", 0, NULL},      // 0x84 
    {"ADD A L", 0, NULL},      // 0x85 
    {"ADD A (HL)", 0, NULL},   // 0x86 
    {"ADD A A", 0, NULL},      // 0x87 
    {"ADC A B", 0, NULL},      // 0x88 
    {"ADC A C", 0, NULL},      // 0x89 
    {"ADC A D", 0, NULL},      // 0x8A 
    {"ADC A E", 0, NULL},      // 0x8B 
    {"ADC A H", 0, NULL},      // 0x8C 
    {"ADC A L", 0, NULL},      // 0x8D 
    {"ADC A (HL)", 0, NULL},   // 0x8E 
    {"ADC A A", 0, NULL},      // 0x8F 
    {"SUB B", 0, NULL},        // 0x90 
    {"SUB C", 0, NULL},        // 0x91 
    {"SUB D", 0, NULL},        // 0x92 
    {"SUB E", 0, NULL},        // 0x93 
    {"SUB H", 0, NULL},        // 0x94 
    {"SUB L", 0, NULL},        // 0x95 
    {"SUB (HL)", 0, NULL},     // 0x96 
    {"SUB A", 0, NULL},        // 0x97 
    {"SBC A B", 0, NULL},      // 0x98 
    {"SBC A C", 0, NULL},      // 0x99 
    {"SBC A D", 0, NULL},      // 0x9A 
    {"SBC A E", 0, NULL},      // 0x9B 
    {"SBC A H", 0, NULL},      // 0x9C 
    {"SBC A L", 0, NULL},      // 0x9D 
    {"SBC A (HL)", 0, NULL},   // 0x9E 
    {"SBC A A", 0, NULL},      // 0x9F 
    {"AND B", 0, NULL},        // 0xA0 
    {"AND C", 0, NULL},        // 0xA1 
    {"AND D", 0, NULL},        // 0xA2 
    {"AND E", 0, NULL},        // 0xA3 
    {"AND H", 0, NULL},        // 0xA4 
    {"AND L", 0, NULL},        // 0xA5 
    {"AND (HL)", 0, NULL},     // 0xA6 
    {"AND A", 0, NULL},        // 0xA7 
    {"XOR B", 0, NULL},        // 0xA8 
    {"XOR C", 0, NULL},        // 0xA9 
    {"XOR D", 0, NULL},        // 0xAA 
    {"XOR E", 0, NULL},        // 0xAB 
    {"XOR H", 0, NULL},        // 0xAC 
    {"XOR L", 0, NULL},        // 0xAD 
    {"XOR (HL)", 0, NULL},     // 0xAE 
    {"XOR A", 0, NULL},        // 0xAF 
    {"OR B", 0, NULL},         // 0xB0 
    {"OR C", 0, NULL},         // 0xB1 
    {"OR D", 0, NULL},         // 0xB2 
    {"OR E", 0, NULL},         // 0xB3 
    {"OR H", 0, NULL},         // 0xB4 
    {"OR L", 0, NULL},         // 0xB5 
    {"OR (HL)", 0, NULL},      // 0xB6 
    {"OR A", 0, NULL},         // 0xB7 
    {"CP B", 0, NULL},         // 0xB8 
    {"CP C", 0, NULL},         // 0xB9 
    {"CP D", 0, NULL},         // 0xBA 
    {"CP E", 0, NULL},         // 0xBB 
    {"CP H", 0, NULL},         // 0xBC 
    {"CP L", 0, NULL},         // 0xBD 
    {"CP (HL)", 0, NULL},      // 0xBE 
    {"CP A", 0, NULL},         // 0xBF 
    {"RET NZ", 0, NULL},       // 0xC0 
    {"POP BC", 0, NULL},       // 0xC1 
    {"JP NZ a16", 2, NULL},    // 0xC2 
    {"JP a16", 2, NULL},       // 0xC3 
    {"CALL NZ a16", 2, NULL},  // 0xC4 
    {"PUSH BC", 0, NULL},      // 0xC5 
    {"ADD A d8", 1, NULL},     // 0xC6 
    {"RST 00H", 0, NULL},      // 0xC7 
    {"RET Z", 0, NULL},        // 0xC8 
    {"RET", 0, NULL},          // 0xC9 
    {"JP Z a16", 2, NULL},     // 0xCA 
    {"PREFIX CB", 0, NULL},    // 0xCB 
    {"CALL Z a16", 2, NULL},   // 0xCC 
    {"CALL a16", 2, NULL},     // 0xCD 
    {"ADC A d8", 1, NULL},     // 0xCE 
    {"RST 08H", 0, NULL},      // 0xCF 
    {"RET NC", 0, NULL},       // 0xD0 
    {"POP DE", 0, NULL},       // 0xD1 
    {"JP NC a16", 2, NULL},    // 0xD2 
    {"CALL NC a16", 2, NULL},  // 0xD4 
    {"PUSH DE", 0, NULL},      // 0xD5 
    {"SUB d8", 1, NULL},       // 0xD6 
    {"RST 10H", 0, NULL},      // 0xD7 
    {"RET C", 0, NULL},        // 0xD8 
    {"RETI", 0, NULL},         // 0xD9 
    {"JP C a16", 2, NULL},     // 0xDA 
    {"CALL C a16", 2, NULL},   // 0xDC 
    {"SBC A d8", 1, NULL},     // 0xDE 
    {"RST 18H", 0, NULL},      // 0xDF 
    {"LDH (a8) A", 1, NULL},   // 0xE0 
    {"POP HL", 0, NULL},       // 0xE1 
    {"LD (C) A", 0, NULL},     // 0xE2 
    {"PUSH HL", 0, NULL},      // 0xE5 
    {"AND d8", 1, NULL},       // 0xE6 
    {"RST 20H", 0, NULL},      // 0xE7 
    {"ADD SP r8", 1, NULL},    // 0xE8 
    {"JP (HL)", 0, NULL},      // 0xE9 
    {"LD (a16) A", 2, NULL},   // 0xEA 
    {"XOR d8", 1, NULL},       // 0xEE 
    {"RST 28H", 0, NULL},      // 0xEF 
    {"LDH A (a8)", 1, NULL},   // 0xF0 
    {"POP AF", 0, NULL},       // 0xF1 
    {"LD A (C)", 0, NULL},     // 0xF2 
    {"DI", 0, NULL},           // 0xF3 
    {"PUSH AF", 0, NULL},      // 0xF5 
    {"OR d8", 1, NULL},        // 0xF6 
    {"RST 30H", 0, NULL},      // 0xF7 
    {"LD HL SP+r8", 1, NULL},  // 0xF8 
    {"LD SP HL", 0, NULL},     // 0xF9 
    {"LD A (a16)", 2, NULL},   // 0xFA 
    {"EI", 0, NULL},           // 0xFB 
    {"CP d8", 1, NULL},        // 0xFE 
    {"RST 38H", 0, NULL},      // 0xFF 
};

bool execute() {
    uint8_t op = read8(pc);
    struct instruction instruction = instructions[op];

    if (instruction.operand_size == 1)
        a8 = read8(++pc);
    if (instruction.operand_size == 2) {
        pc += 2;
        a16 = read16(pc);
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

void nop() {}

void rst_38() {}

void inc_bc() { registers.bc++; }
void inc_b() { registers.b++; }
void inc_c() { registers.c++; }
void inc_de() { registers.de++; }
void inc_d() { registers.d++; }
void inc_e() { registers.e++; }
void inc_h() { registers.h++; }
void inc_l() { registers.l++; }
void inc_sp() { registers.sp++; }
void inc_a() { registers.a++; }
void inc_hl() { registers.hl++; }

void dec_b() { registers.b--; }
void dec_bc() { registers.bc--; }
void dec_c() { registers.c--; }
void dec_d() { registers.d--; }
void dec_de() { registers.de--; }
void dec_e() { registers.e--; }
void dec_h() { registers.h--; }
void dec_hl() { registers.hl--; }
void dec_l() { registers.l--; }
void dec_sp() { registers.sp--; }
void dec_a() { registers.a--; }

void ld_bc_n16() { registers.bc = a16; }
void ld_b_n8() { registers.b = a8; }
void ld_c_n8() { registers.c = a8; }
void ld_de_n16() { registers.de = a16; }
void ld_d_n8() { registers.d = a8; }
void ld_e_n8() { registers.e = a8; }
void ld_hl_n16() { registers.hl = a16; }
void ld_h_n8() { registers.h = a8; }
void ld_l_n8() { registers.l = a8; }
void ld_sp_n16() { registers.sp = a16; }
void ld_a_n8() { registers.a = a8; }

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

void ld_hl_n8() { write8(registers.hl, a8); }

void ld_a_bci() { registers.a = read8(registers.bc); }
void ld_a_dei() { registers.a = read8(registers.de); }

void ld_bci_a() { write8(registers.bc, registers.a); }
void ld_dei_a() { write8(registers.de, registers.a); }

void ld_a_nn() { registers.a = read8(a16); }