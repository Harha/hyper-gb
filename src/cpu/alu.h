#ifndef ALU_H
#define ALU_H

#include "data_types.h"

namespace hgb
{

class CPU;
struct CPURegisters;
struct CPUState;
class MMU;

class ALU
{
public:
	ALU(
		CPU * cpu = nullptr
	);
	~ALU();

	// AND, 8-bit register
	void AND(byte reg);
	// OR, 8-bit register
	void OR(byte reg);
	// XOR, 8-bit register
	void XOR(byte reg);
	// CP, 8-bit register
	void CP(byte reg);
	// INC, 16-bit register
	void INC(word & reg);
	// INC, 8-bit register
	void INC(byte & reg);
	// INC, 16-bit address's 8-bit value
	void INC16(word addr);
	// DEC, 16-bit register
	void DEC(word & reg);
	// DEC, 8-bit register
	void DEC(byte & reg);
	// DEC, 16-bit address's 8-bit value
	void DEC16(word addr);
	// ADD, 8-bit register to 8-bit register
	void ADD(byte & reg1, byte reg2);
	// ADC, 8-bit register to 8-bit register
	void ADC(byte & reg1, byte reg2);
	// SUB, 8-bit register from 8-bit register
	void SUB(byte & reg1, byte reg2);
	// SBC, 8-bit register from 8-bit register
	void SBC(byte & reg1, byte reg2);
	// ADD, 16-bit register to 16-bit register
	void ADD(word & reg1, word reg2);
	// ADD, 8-bit value to 16-bit SP register
	void ADD_SP_r8(int8_t r);
	// CCF
	void CCF();
	// SCF
	void SCF();
	// DAA
	void DAA();
	// CPL
	void CPL();
private:
	CPU * m_cpu;
	CPURegisters & m_registers;
	CPUState & m_state;
	MMU * m_mmu;
};

}

#endif // ALU_H