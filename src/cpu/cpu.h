#ifndef CPU_H
#define CPU_H

#include <vector>
#include "cpu_registers.h"
#include "cpu_state.h"

namespace hgb
{

class MMU;
class ALU;

class CPU
{
public:
	CPU(
		std::vector<word> breakpoints = std::vector<word>()
	);
	~CPU();

	// Initialize the CPU
	void init();
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

	CPURegisters & getRegisters();
	CPUState & getState();
	MMU * getMMU();
	ALU * getALU();
private:
	std::vector<word> m_breakpoints;
	CPURegisters m_registers;
	CPUState m_state;
	MMU * m_mmu;
	ALU * m_alu;
};

}

#endif // CPU_H