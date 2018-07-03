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

	// TODO: Verify this works!
	if (!m_registers.checkC())
		m_registers.setC();
	else
		m_registers.clearC();
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

	if (m_registers.A == 1)
		m_registers.clearZ();
	else
		m_registers.setZ();

	m_registers.clearN();
	m_registers.setH();
	m_registers.clearC();

	m_state.CLOCK += c;
}

void ALU::OR(byte val, int c)
{
	m_registers.A |= val;

	if (m_registers.A == 1)
		m_registers.clearZ();
	else
		m_registers.setZ();

	m_registers.clearN();
	m_registers.clearH();
	m_registers.clearC();

	m_state.CLOCK += c;
}

void ALU::XOR(byte val, int c)
{
	m_registers.A ^= val;

	if (m_registers.A == 1)
		m_registers.clearZ();
	else
		m_registers.setZ();

	m_registers.clearN();
	m_registers.clearH();
	m_registers.clearC();

	m_state.CLOCK += c;
}

void ALU::CP(byte val, int c)
{
	word result = static_cast<word>(m_registers.A) - static_cast<word>(val);

	if (result == 0x0000)
		m_registers.setZ();
	else
		m_registers.clearZ();

	m_registers.setN();

	if ((m_registers.A & 0x0F) < (val & 0x0F))
		m_registers.setH();
	else
		m_registers.clearH();

	if (m_registers.A < val)
		m_registers.setC();
	else
		m_registers.clearC();

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

	if (result == 0x00)
		m_registers.setZ();
	else
		m_registers.clearZ();

	m_registers.clearN();

	if ((val & 0x0F) == 0x0F)
		m_registers.setH();
	else
		m_registers.clearH();

	val = result;

	m_state.CLOCK += c;
}

void ALU::INC_ADDR(word addr, int c)
{
	byte val = m_mmu->read(addr);
	byte result = val + 1;

	if (result == 0x00)
		m_registers.setZ();
	else
		m_registers.clearZ();

	m_registers.clearN();

	if ((val & 0x0F) == 0x0F)
		m_registers.setH();
	else
		m_registers.clearH();

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

	if (result == 0x00)
		m_registers.setZ();
	else
		m_registers.clearZ();

	m_registers.setN();

	if ((val & 0x0F) == 0x00)
		m_registers.setH();
	else
		m_registers.clearH();

	val = result;

	m_state.CLOCK += c;
}

void ALU::DEC_ADDR(word addr, int c)
{
	byte val = m_mmu->read(addr);
	byte result = val - 1;

	if (result == 0x00)
		m_registers.setZ();
	else
		m_registers.clearZ();

	m_registers.clearN();

	if ((val & 0x0F) == 0x00)
		m_registers.setH();
	else
		m_registers.clearH();

	m_mmu->write(addr, result);

	m_state.CLOCK += 8;
	m_state.CLOCK += c;
}

void ALU::ADD(word & val, word n, int c)
{
	word result = val + n;

	m_registers.clearN();

	// TODO: Verify this works!
	if ((val & 0x0FFF) == 0x0FFF)
		m_registers.setH();
	else
		m_registers.clearH();

	// TODO: Verify this works!
	if ((val & 0xFFFF) == 0xFFFF)
		m_registers.setC();
	else
		m_registers.clearC();

	val = result;

	m_state.CLOCK += 4;
	m_state.CLOCK += c;
}

void ALU::ADD(byte & val, byte n, int c)
{
	byte result = val + n;

	if (result == 0x00)
		m_registers.setZ();
	else
		m_registers.clearZ();

	m_registers.clearN();

	// TODO: Verify this works!
	if ((val & 0x0F) == 0x0F)
		m_registers.setH();
	else
		m_registers.clearH();

	// TODO: Verify this works!
	if ((val & 0xFF) == 0xFF)
		m_registers.clearC();

	val = result;

	m_state.CLOCK += c;
}

void ALU::ADC(byte & val, byte n, int c)
{
	byte result = val + (n + CPUR_F_C);

	if (result == 0x00)
		m_registers.setZ();
	else
		m_registers.clearZ();

	m_registers.clearN();

	// TODO: Verify this works!
	if ((val & 0x0F) == 0x0F)
		m_registers.setH();
	else
		m_registers.clearH();

	// TODO: Verify this works!
	if ((val & 0xFF) == 0xFF)
		m_registers.setC();
	else
		m_registers.clearC();

	val = result;

	m_state.CLOCK += c;
}

void ALU::SUB(byte & val, byte n, int c)
{
	word result = static_cast<word>(val) - static_cast<word>(n);

	if (result == 0x0000)
		m_registers.setZ();
	else
		m_registers.clearZ();

	m_registers.setN();

	if ((val & 0x0F) < (n & 0x0F))
		m_registers.setH();
	else
		m_registers.clearH();

	if (val < n)
		m_registers.setC();
	else
		m_registers.clearC();

	val = static_cast<byte>(result);

	m_state.CLOCK += c;
}

void ALU::SBC(byte & val, byte n, int c)
{
	word result = static_cast<word>(val) - static_cast<word>(n + CPUR_F_C);

	if (result == 0x0000)
		m_registers.setZ();
	else
		m_registers.clearZ();

	m_registers.setN();

	if ((val & 0x0F) < (n & 0x0F))
		m_registers.setH();
	else
		m_registers.clearH();

	if (val < n)
		m_registers.setC();
	else
		m_registers.clearC();

	val = static_cast<byte>(result);

	m_state.CLOCK += c;
}

void ALU::ADD_SP_r8(int8_t n)
{
	word result = m_registers.SP + n;

	m_registers.clearZ();
	m_registers.clearN();

	// TODO: Verify this works!
	if ((m_registers.SP & 0x0FFF) == 0x0FFF)
		m_registers.setH();
	else
		m_registers.clearH();

	// TODO: Verify this works!
	if ((m_registers.SP & 0xFFFF) == 0xFFFF)
		m_registers.setC();
	else
		m_registers.clearC();

	m_registers.SP = result;

	m_state.CLOCK += 12;
}

void ALU::RL(byte & reg)
{
	reg = (reg << 1) | (m_registers.F & CPUR_F_C);
	m_registers.flipZ();
	m_registers.clearN();
	m_registers.clearH();
	m_registers.flipC();
	m_state.CLOCK += 4;
}

void ALU::RLC(byte & reg)
{
	reg = (reg << 1) | (m_registers.F & CPUR_F_C);
	m_registers.flipZ();
	m_registers.clearN();
	m_registers.clearH();
	m_registers.flipC();
	m_state.CLOCK += 4;
}

}