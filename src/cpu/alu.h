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

	void CCF();
	void SCF();
	void DAA();
	void CPL();
	void AND(byte val, int c = 0);
	void OR(byte val, int c = 0);
	void XOR(byte val, int c = 0);
	void CP(byte val, int c = 0);
	void INC(word & val, int c = 0);
	void INC(byte & val, int c = 0);
	void INC_ADDR(word addr, int c = 0);
	void DEC(word & val, int c = 0);
	void DEC(byte & val, int c = 0);
	void DEC_ADDR(word val, int c = 0);
	void ADD(word & val, word n, int c = 0);
	void ADD(byte & val, byte n, int c = 0);
	void ADC(byte & val, byte n, int c = 0);
	void SUB(byte & val, byte n, int c = 0);
	void SBC(byte & val, byte n, int c = 0);
	void ADD_SP_r8(int8_t n);
	void RL(byte & reg);
	void RLC(byte & reg);
private:
	CPU * m_cpu;
	CPURegisters & m_registers;
	CPUState & m_state;
	MMU * m_mmu;
};

}

#endif // ALU_H