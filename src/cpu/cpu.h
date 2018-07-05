#ifndef CPU_H
#define CPU_H

#include <vector>
#include "alu.h"
#include "cpu_registers.h"
#include "cpu_state.h"

namespace hgb
{

class MMU;

class CPU
{
public:
	CPU(
		MMU & mmu
	);
	~CPU();

	// Run the CPU, one instruction at a time
	void tick();
	// Handle normal opcode
	void op(byte op);
	// Handle PREFIX CB opcode
	void cb(byte op);

	void LD(word & val, word n, int c = 0);
	void LD(byte & val, byte n, int c = 0);
	void LD_ADDR(word addr, byte n, int c = 0);
	void LD_HL_SP_r8(int8_t n);
	byte RST(byte op);

	MMU & getMMU();
	CPURegisters & getRegisters();
	CPUState & getState();
	ALU & getALU();
	std::vector<word> & getBreakpoints();
private:
	MMU & m_mmu;
	CPURegisters m_registers;
	CPUState m_state;
	ALU m_alu;
	std::vector<word> m_breakpoints;
};

}

#endif // CPU_H