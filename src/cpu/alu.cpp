#include "alu.h"
#include "3rdparty/mlibc_log.h"
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

void ALU::CCF()
{
	m_registers.clearN();
	m_registers.clearH();
	m_registers.testC(!m_registers.checkC());
}

void ALU::SCF()
{
	m_registers.clearN();
	m_registers.clearH();
	m_registers.setC();
}

void ALU::DAA()
{
	// TODO: Implement!
}

void ALU::CPL()
{
	m_registers.setN();
	m_registers.setH();
}

void ALU::AND(byte val, int c)
{
	m_registers.A &= val;

	m_registers.testZ(m_registers.A != 1);
	m_registers.clearN();
	m_registers.setH();
	m_registers.clearC();

	m_state.CLOCK += c;
}

void ALU::OR(byte val, int c)
{
	m_registers.A |= val;

	m_registers.testZ(m_registers.A != 1);
	m_registers.clearN();
	m_registers.clearH();
	m_registers.clearC();

	m_state.CLOCK += c;
}

void ALU::XOR(byte val, int c)
{
	m_registers.A ^= val;

	m_registers.testZ(m_registers.A != 1);
	m_registers.clearN();
	m_registers.clearH();
	m_registers.clearC();

	m_state.CLOCK += c;
}

void ALU::CP(byte val, int c)
{
	word result = static_cast<word>(m_registers.A) - static_cast<word>(val);

	m_registers.testZ(result == 0x0000);
	m_registers.setN();
	m_registers.testH((m_registers.A & 0x0F) < (val & 0x0F));
	m_registers.testC(m_registers.A < val);

	m_state.CLOCK += c;
}

void ALU::INC(word & val, int c)
{
	val++;
	m_state.CLOCK += 4;
	m_state.CLOCK += c;
}

void ALU::INC(byte & val, int c)
{
	byte result = val + 1;

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.testH((val & 0x0F) == 0x0F);

	val = result;

	m_state.CLOCK += c;
}

void ALU::INC_ADDR(word addr, int c)
{
	byte val = m_mmu->read(addr);
	byte result = val + 1;

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.testH((val & 0x0F) == 0x0F);

	m_mmu->write(addr, result);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::DEC(word & val, int c)
{
	val--;
	m_state.CLOCK += 4;
	m_state.CLOCK += c;
}

void ALU::DEC(byte & val, int c)
{
	byte result = val - 1;

	m_registers.testZ(result == 0x00);
	m_registers.setN();
	m_registers.testH((val & 0x0F) == 0x00);

	val = result;

	m_state.CLOCK += c;
}

void ALU::DEC_ADDR(word addr, int c)
{
	byte val = m_mmu->read(addr);
	byte result = val - 1;

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.testH((val & 0x0F) == 0x00);

	m_mmu->write(addr, result);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::ADD(word & val, word n, int c)
{
	word result = val + n;

	m_registers.clearN();
	m_registers.testH((val & 0x0FFF) == 0x0FFF);
	m_registers.testC((val & 0xFFFF) == 0xFFFF);

	val = result;

	m_state.CLOCK += 4;
	m_state.CLOCK += c;
}

void ALU::ADD(byte & val, byte n, int c)
{
	byte result = val + n;

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.testH((val & 0x0F) == 0x0F);
	m_registers.testC((val & 0xFF) == 0xFF);

	val = result;

	m_state.CLOCK += c;
}

void ALU::ADC(byte & val, byte n, int c)
{
	byte result = val + (n + CPUR_F_C);

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.testH((val & 0x0F) == 0x0F);
	m_registers.testC((val & 0xFF) == 0xFF);

	val = result;

	m_state.CLOCK += c;
}

void ALU::SUB(byte & val, byte n, int c)
{
	word result = static_cast<word>(val) - static_cast<word>(n);

	m_registers.testZ(result == 0x00);
	m_registers.setN();
	m_registers.testH((val & 0x0F) < (n & 0x0F));
	m_registers.testC(val < n);

	val = static_cast<byte>(result);

	m_state.CLOCK += c;
}

void ALU::SBC(byte & val, byte n, int c)
{
	word result = static_cast<word>(val) - static_cast<word>(n + CPUR_F_C);

	m_registers.testZ(result == 0x00);
	m_registers.setN();
	m_registers.testH((val & 0x0F) < (n & 0x0F));
	m_registers.testC(val < n);

	val = static_cast<byte>(result);

	m_state.CLOCK += c;
}

void ALU::ADD_SP_r8(int8_t n)
{
	word result = m_registers.SP + n;

	m_registers.clearZ();
	m_registers.clearN();
	m_registers.testH((m_registers.SP & 0x0FFF) == 0x0FFF);
	m_registers.testC((m_registers.SP & 0xFFFF) == 0xFFFF);

	m_registers.SP = result;

	m_state.CLOCK += 12;
}

void ALU::RLC(byte & val, int c)
{
	byte result = (val << 1) | (val >> 7);

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.clearH();
	m_registers.testC((val & 0b10000000) != 0x00);

	val = result;

	m_state.CLOCK += c;
}

void ALU::RLC_ADDR(word addr, int c)
{
	byte val = m_mmu->read(addr);
	RLC(val, c);
	m_mmu->write(addr, val);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::RRC(byte & val, int c)
{
	byte result = (val >> 1) | (val << 7);

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.clearH();
	m_registers.testC((val & 0b00000001) != 0x00);

	val = result;

	m_state.CLOCK += c;
}

void ALU::RRC_ADDR(word addr, int c)
{
	byte val = m_mmu->read(addr);
	RRC(val, c);
	m_mmu->write(addr, val);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::RL(byte & val, int c)
{
	byte result = (val << 1) | (val >> 7);
	result &= 0b11111110;
	result |= (m_registers.F & CPUR_F_C) >> 4;

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.clearH();
	m_registers.testC((val & 0b10000000) != 0x00);

	val = result;

	m_state.CLOCK += c;
}

void ALU::RL_ADDR(word addr, int c)
{
	byte val = m_mmu->read(addr);
	RL(val, c);
	m_mmu->write(addr, val);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::RR(byte & val, int c)
{
	byte result = (val >> 1) | (val << 7);
	result &= 0b01111111;
	result |= (m_registers.F & CPUR_F_C) << 3;

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.clearH();
	m_registers.testC((val & 0b00000001) != 0x00);

	val = result;

	m_state.CLOCK += c;
}

void ALU::RR_ADDR(word addr, int c)
{
	byte val = m_mmu->read(addr);
	RR(val, c);
	m_mmu->write(addr, val);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::SLA(byte & val, int c)
{
	byte result = val << 1;

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.clearH();
	m_registers.testC((val & 0b10000000) != 0x00);

	val = result;

	m_state.CLOCK += c;
}

void ALU::SLA_ADDR(word addr, int c)
{
	byte val = m_mmu->read(addr);
	SLA(val, c);
	m_mmu->write(addr, val);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::SRA(byte & val, int c)
{
	byte result = val >> 1;
	result &= 0b01111111;
	result |= 0b10000000 & val;

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.clearH();
	m_registers.testC((val & 0b00000001) != 0x00);

	val = result;

	m_state.CLOCK += c;
}

void ALU::SRA_ADDR(word addr, int c)
{
	byte val = m_mmu->read(addr);
	SRA(val, c);
	m_mmu->write(addr, val);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::SWAP(byte & val, int c)
{
	byte result = (val >> 4) | (val << 4); // rotate 4 -> swap h & l bits of val

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.clearH();
	m_registers.clearC();

	val = result;

	m_state.CLOCK += c;
}

void ALU::SWAP_ADDR(word addr, int c)
{
	byte val = m_mmu->read(addr);
	SWAP(val, c);
	m_mmu->write(addr, val);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::SRL(byte & val, int c)
{
	byte result = val >> 1;
	result &= 0b01111111;

	m_registers.testZ(result == 0x00);
	m_registers.clearN();
	m_registers.clearH();
	m_registers.testC((val & 0b00000001) != 0x00);

	val = result;

	m_state.CLOCK += c;
}

void ALU::SRL_ADDR(word addr, int c)
{
	byte val = m_mmu->read(addr);
	SRL(val, c);
	m_mmu->write(addr, val);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::BIT(int n, byte & val, int c)
{
	m_registers.testZ(((val >> n) && 0x01) == 0x00);
	m_registers.clearN();
	m_registers.setH();

	m_state.CLOCK += c;
}

void ALU::BIT_ADDR(int n, word addr, int c)
{
	byte val = m_mmu->read(addr);
	BIT(n, val, c);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::RES(int n, byte & val, int c)
{
	byte result = val & ~(0x01 << n);

	val = result;

	m_state.CLOCK += c;
}

void ALU::RES_ADDR(int n, word addr, int c)
{
	byte val = m_mmu->read(addr);
	RES(n, val, c);
	m_mmu->write(addr, val);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::SET(int n, byte & val, int c)
{
	byte result = val | (0x01 << n);

	val = result;

	m_state.CLOCK += c;
}

void ALU::SET_ADDR(int n, word addr, int c)
{
	byte val = m_mmu->read(addr);
	SET(n, val, c);
	m_mmu->write(addr, val);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

}