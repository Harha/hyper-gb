#ifndef CPU_H
#define CPU_H

#include <map>
#include "cpu_registers.h"
#include "cpu_state.h"

namespace hgb
{

class MMU;
class ALU;

class CPU
{
public:
	CPU();
	~CPU();

	// Initialize the CPU
	void init();
	// Run the CPU, one instruction at a time
	void tick();
	// Handle normal opcode
	void OP(byte op);
	// Handle PREFIX CB opcode
	void CB(byte op);
	// load 16-bit value to target 16-bit register
	void LD(int c, word & dst, word src);
	// load 8-bit value to target 16-bit address
	void LD(word addr, byte val);
	// load 8-bit value to target 8-bit register
	void LD(byte & reg, byte val);
	// map RST opcode to fixed 8-bit address
	byte RST(byte op);
	// CB BIT n, 8-bit register
	void BIT(int n, byte & reg);

	CPURegisters & getRegisters();
	CPUState & getState();
	MMU * getMMU();
	ALU * getALU();
private:
	CPURegisters m_registers;
	CPUState m_state;
	MMU * m_mmu;
	ALU * m_alu;
};

}

#endif // CPU_H