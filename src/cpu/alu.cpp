#include "alu.h"
#include "3rd party/mlibc_log.h"
#include "cpu/cpu.h"
#include "cpu/cpu_registers.h"
#include "cpu/cpu_state.h"
#include "mem/mmu.h"

namespace hgb
{

ALU::ALU(
	CPU * cpu
) :
	m_cpu(cpu),
	m_registers(m_cpu->getRegisters()),
	m_state(m_cpu->getState()),
	m_mmu(m_cpu->getMMU())
{
	mlibc_dbg("ALU::ALU()");
}

ALU::~ALU()
{
	mlibc_dbg("ALU::~ALU()");
}

void ALU::AND(byte reg)
{
	m_registers.A = m_registers.A & reg;
	m_registers.flipZ();
	m_registers.clearN();
	m_registers.setH();
	m_registers.clearC();
	m_state.CLOCK += 4;
}

void ALU::OR(byte reg)
{
	m_registers.A = m_registers.A | reg;
	m_registers.flipZ();
	m_registers.clearN();
	m_registers.clearH();
	m_registers.clearC();
	m_state.CLOCK += 4;
}

void ALU::XOR(byte reg)
{
	m_registers.A = m_registers.A ^ reg;
	m_registers.flipZ();
	m_registers.clearN();
	m_registers.clearH();
	m_registers.clearC();
	m_state.CLOCK += 4;
}

void ALU::CP(byte reg)
{
	m_registers.A = m_registers.A == reg;
	m_registers.flipZ();
	m_registers.setN();
	m_registers.flipH();
	m_registers.flipC();
	m_state.CLOCK += 4;
}

void ALU::INC(word & reg)
{
	reg = reg + 1;
	m_state.CLOCK += 8;
}

void ALU::INC(byte & reg)
{
	reg = reg + 1;
	m_registers.flipZ();
	m_registers.clearN();
	m_registers.flipH();
	m_state.CLOCK += 4;
}

void ALU::INC16(word addr)
{
	byte val = m_mmu->read(addr);
	val = val + 1;
	m_mmu->write(addr, val);
	m_registers.flipZ();
	m_registers.clearN();
	m_registers.flipH();
	m_state.CLOCK += 12;
}

void ALU::DEC(word & reg)
{
	reg = reg - 1;
	m_state.CLOCK += 8;
}

void ALU::DEC(byte & reg)
{
	reg = reg - 1;
	m_registers.flipZ();
	m_registers.setN();
	m_registers.flipH();
	m_state.CLOCK += 4;
}

void ALU::DEC16(word addr)
{
	byte val = m_mmu->read(addr);
	val = val - 1;
	m_mmu->write(addr, val);
	m_registers.flipZ();
	m_registers.setN();
	m_registers.flipH();
	m_state.CLOCK += 12;
}

void ALU::ADD(byte & reg1, byte reg2)
{
	reg1 = reg1 + reg2;
	m_registers.flipZ();
	m_registers.clearN();
	m_registers.flipH();
	m_registers.flipC();
	m_state.CLOCK += 4;
}

void ALU::ADC(byte & reg1, byte reg2)
{
	reg1 = reg1 + reg2 + (m_registers.F & CPUR_F_C);
	m_registers.flipZ();
	m_registers.clearN();
	m_registers.flipH();
	m_registers.flipC();
	m_state.CLOCK += 4;
}

void ALU::SUB(byte & reg1, byte reg2)
{
	reg1 = reg1 - reg2;
	m_registers.flipZ();
	m_registers.setN();
	m_registers.flipH();
	m_registers.flipC();
	m_state.CLOCK += 4;
}

void ALU::SBC(byte & reg1, byte reg2)
{
	reg1 = reg1 - reg2 - (m_registers.F & CPUR_F_C);
	m_registers.flipZ();
	m_registers.setN();
	m_registers.flipH();
	m_registers.flipC();
	m_state.CLOCK += 4;
}

void ALU::ADD(word & reg1, word reg2)
{
	reg1 = reg1 + reg2;
	m_registers.clearN();
	m_registers.flipH();
	m_registers.flipC();
	m_state.CLOCK += 8;
}

void ALU::ADD_SP_r8(int8_t r)
{
	m_registers.SP = m_registers.SP + r;
	m_registers.clearZ();
	m_registers.clearN();
	m_registers.flipH();
	m_registers.flipC();
	m_state.CLOCK += 16;
}

void ALU::CCF()
{
	m_registers.clearN();
	m_registers.clearH();
	m_registers.flipC();
	m_state.CLOCK += 4;
}

void ALU::SCF()
{
	m_registers.clearN();
	m_registers.clearH();
	m_registers.setC();
	m_state.CLOCK += 4;
}

void ALU::DAA()
{
	m_registers.flipZ();
	m_registers.clearH();
	m_registers.flipC();
	m_state.CLOCK += 4;
}

void ALU::CPL()
{
	m_registers.A = ~m_registers.A; // flip A
	m_registers.setN();
	m_registers.setH();
	m_state.CLOCK += 4;
}

}