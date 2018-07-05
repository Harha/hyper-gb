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
	void RLC(byte & val, int c = 0);
	void RLC_ADDR(word addr, int c = 0);
	void RRC(byte & val, int c = 0);
	void RRC_ADDR(word addr, int c = 0);
	void RL(byte & val, int c = 0);
	void RL_ADDR(word addr, int c = 0);
	void RR(byte & val, int c = 0);
	void RR_ADDR(word addr, int c = 0);
	void SLA(byte & val, int c = 0);
	void SLA_ADDR(word addr, int c = 0);
	void SRA(byte & val, int c = 0);
	void SRA_ADDR(word addr, int c = 0);
	void SWAP(byte & val, int c = 0);
	void SWAP_ADDR(word addr, int c = 0);
	void SRL(byte & val, int c = 0);
	void SRL_ADDR(word addr, int c = 0);
	void BIT(int n, byte & val, int c = 0);
	void BIT_ADDR(int n, word addr, int c = 0);
	void RES(int n, byte & val, int c = 0);
	void RES_ADDR(int n, word addr, int c = 0);
	void SET(int n, byte & val, int c = 0);
	void SET_ADDR(int n, word addr, int c = 0);
private:
	CPU * m_cpu;
	CPURegisters & m_registers;
	CPUState & m_state;
	MMU & m_mmu;
};

}

#endif // ALU_H