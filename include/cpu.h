#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

struct registers {
	struct {
		union {
			struct {
				uint8_t f;
				uint8_t a;
			};
			uint16_t af;
		};
	};
	
	struct {
		union {
			struct {
				uint8_t c;
				uint8_t b;
			};
			uint16_t bc;
		};
	};
	
	struct {
		union {
			struct {
				uint8_t e;
				uint8_t d;
			};
			uint16_t de;
		};
	};
	
	struct {
		union {
			struct {
				uint8_t l;
				uint8_t h;
			};
			uint16_t hl;
		};
	};
	
	uint16_t sp;
	uint16_t pc;
};

struct instruction {
  char *dissassembly;
  uint8_t operand_size;
  void (*fn)();
};

extern struct registers registers;
extern struct instruction instructions[256];

extern bool execute();
void nop();
void rst_38();

void inc_bc();
void inc_b();
void inc_c();
void inc_de(); 
void inc_d(); 
void inc_e(); 
void inc_hl(); 
void inc_h(); 
void inc_l(); 
void inc_sp(); 
void inc_a(); 

void dec_b();
void dec_bc();
void dec_c();
void dec_d();
void dec_de();
void dec_e();
void dec_h();
void dec_hl();
void dec_l();
void dec_sp();
void dec_a();

void ld_bc_n16();
void ld_b_n8();
void ld_c_n8();
void ld_de_n16();
void ld_d_n8();
void ld_e_n8();
void ld_hl_n16();
void ld_h_n8();
void ld_l_n8();
void ld_sp_n16();
void ld_hl_n8();
void ld_a_n8();

void ld_b_b();
void ld_b_c();
void ld_b_d();
void ld_b_e();
void ld_b_l();
void ld_b_h();
void ld_b_a();
void ld_c_b();
void ld_c_c();
void ld_c_d();
void ld_c_e();
void ld_c_h();
void ld_c_l();
void ld_c_a();
void ld_d_b();
void ld_d_c();
void ld_d_d();
void ld_d_e();
void ld_d_h();
void ld_d_l();
void ld_d_a();
void ld_e_b();
void ld_e_c();
void ld_e_d();
void ld_e_e();
void ld_e_h();
void ld_e_l();
void ld_e_a();
void ld_h_b();
void ld_h_c();
void ld_h_d();
void ld_h_e();
void ld_h_h();
void ld_h_l();
void ld_h_a();
void ld_l_b();
void ld_l_c();
void ld_l_d();
void ld_l_e();
void ld_l_h();
void ld_l_l();
void ld_l_a();
void ld_a_b();
void ld_a_c();
void ld_a_d();
void ld_a_e();
void ld_a_h();
void ld_a_l();
void ld_a_a();
void ld_c_a();
void ld_a_c();

void ld_a_hl();
void ld_b_hl();
void ld_c_hl();
void ld_d_hl();
void ld_e_hl();
void ld_h_hl();
void ld_l_hl();

void ld_hl_a();
void ld_hl_b();
void ld_hl_c();
void ld_hl_d();
void ld_hl_e();
void ld_hl_h();
void ld_hl_l();

void ld_hl_n8();

void ld_a_bci();
void ld_a_dei();

void ld_bci_a();
void ld_dei_a();

void ld_a_nn();
