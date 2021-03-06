#include "cpu.h"
#include "3rdparty/mlibc_log.h"
#include "data_types.h"
#include "mem/mmu.h"

namespace hgb
{

CPU::CPU(
	MMU & mmu
) :
	m_mmu(mmu),
	m_registers(),
	m_state(),
	m_alu(this),
	m_breakpoints()
{
	// Setup CPU registers
	m_registers.AF = 0x0000;
	m_registers.BC = 0x0000;
	m_registers.DE = 0x0000;
	m_registers.HL = 0x0000;
	m_registers.PC = 0x0000;
	m_registers.SP = 0x0000;

	// Setup CPU state
	m_state.CLOCK = 0;
	m_state.IME = 1;
	m_state.IME_scheduled = 0;
	m_state.STATE = CPUState::NORMAL;

	mlibc_dbg("CPU::CPU()");
}

CPU::~CPU()
{
	mlibc_dbg("CPU::~CPU()");
}

void CPU::tick()
{
	// Handle breakpoints
	if (m_breakpoints.size() > 0)
	{
		// Did we hit a breakpoint? If so, print info
		if (std::find(m_breakpoints.begin(), m_breakpoints.end(), m_registers.PC) != m_breakpoints.end())
		{
			mlibc_dbg("CPU::tick(). Breakpoint hit. PC: 0x%04zx, SP: 0x%04zx, OP: 0x%02zx, STATE: %d, CLOCK: %d",
					  m_registers.PC,
					  m_registers.SP,
					  m_mmu.read(m_registers.PC),
					  m_state.STATE,
					  m_state.CLOCK
			);
			mlibc_dbg("A: 0x%02zx, F: 0x%02zx, B: 0x%02zx, C: 0x%02zx, D: 0x%02zx, E: 0x%02zx, H: 0x%02zx, L: 0x%02zx",
					  m_registers.A, m_registers.F, m_registers.B, m_registers.C, m_registers.D, m_registers.E, m_registers.H, m_registers.L
			);
			mlibc_dbg("Z: %d, N: %d, H: %d, C: %d", m_registers.checkZ(), m_registers.checkN(), m_registers.checkH(), m_registers.checkC());
		}
	}

	// Execute normal or cb opcode or do nothing while halted/stopped
	switch (m_state.STATE)
	{
		case CPUState::NORMAL:
		{
			op(m_mmu.read(m_registers.PC++));
		} break;
		case CPUState::PREFIX_CB:
		{
			cb(m_mmu.read(m_registers.PC++));
		} break;
		case CPUState::HALT:
		case CPUState::STOP:
		{
			mlibc_wrn("CPU::tick(), warning! CPU Is halted or stopped!");
		} break;
	}
}

void CPU::op(byte op)
{
	m_state.CLOCK += 4;

	// Get refs to CPU registers
	auto & PC = m_registers.PC;
	auto & SP = m_registers.SP;

	// Get refs to CPU state variables
	auto & CLOCK = m_state.CLOCK;
	auto & IME = m_state.IME;
	auto & IME_scheduled = m_state.IME_scheduled;
	auto & STATE = m_state.STATE;

	// Handle the OPCODE
	switch (op)
	{
		// PREFIX CB
		case 0xCB: STATE = CPUState::PREFIX_CB; break;
			// NOP
		case 0x00: break;
			// STOP
		case 0x10:
		{
			byte r = m_mmu.read(PC++); // unused, wtf ??
			STATE = CPUState::STOP;
		} break;
		// HALT
		case 0x76: STATE = CPUState::HALT; break;
			// DI
		case 0xF3: IME = 0; break;
			// EI
		case 0xFB: IME_scheduled = 1; break;

			// ALU 8-bit CCF, SCF, DAA, CPL
		case 0x3F: m_alu.CCF(); break;
		case 0x37: m_alu.SCF(); break;
		case 0x27: m_alu.DAA(); break;
		case 0x2F: m_alu.CPL(); break;

			// ALU 8-bit RLCA, RLA, RRCA, RRA
		case 0x07: m_alu.RLC(m_registers.A); break;
		case 0x17: m_alu.RL(m_registers.A); break;
		case 0x0F: m_alu.RRC(m_registers.A); break;
		case 0x1F: m_alu.RR(m_registers.A); break;

			// ALU 16-bit
		case 0x03: m_alu.INC(m_registers.BC); break;
		case 0x13: m_alu.INC(m_registers.DE); break;
		case 0x23: m_alu.INC(m_registers.HL); break;
		case 0x33: m_alu.INC(SP); break;
		case 0x0B: m_alu.DEC(m_registers.BC); break;
		case 0x1B: m_alu.DEC(m_registers.DE); break;
		case 0x2B: m_alu.DEC(m_registers.HL); break;
		case 0x3B: m_alu.DEC(SP); break;
		case 0x09: m_alu.ADD(m_registers.HL, m_registers.BC); break;
		case 0x19: m_alu.ADD(m_registers.HL, m_registers.DE); break;
		case 0x29: m_alu.ADD(m_registers.HL, m_registers.HL); break;
		case 0x39: m_alu.ADD(m_registers.HL, SP); break;
		case 0xE8: m_alu.ADD_SP_r8(static_cast<int8_t>(m_mmu.read(PC++))); break;

			// ALU 8-bit
		case 0x04: m_alu.INC(m_registers.B); break;
		case 0x14: m_alu.INC(m_registers.D); break;
		case 0x24: m_alu.INC(m_registers.H); break;
		case 0x34: m_alu.INC_ADDR(m_registers.HL); break;
		case 0x0C: m_alu.INC(m_registers.C); break;
		case 0x1C: m_alu.INC(m_registers.E); break;
		case 0x2C: m_alu.INC(m_registers.L); break;
		case 0x3C: m_alu.INC(m_registers.A); break;
		case 0x05: m_alu.DEC(m_registers.B); break;
		case 0x15: m_alu.DEC(m_registers.D); break;
		case 0x25: m_alu.DEC(m_registers.H); break;
		case 0x35: m_alu.DEC_ADDR(m_registers.HL); break;
		case 0x0D: m_alu.DEC(m_registers.C); break;
		case 0x1D: m_alu.DEC(m_registers.E); break;
		case 0x2D: m_alu.DEC(m_registers.L); break;
		case 0x3D: m_alu.DEC(m_registers.A); break;
		case 0xA0: m_alu.AND(m_registers.B); break;
		case 0xA1: m_alu.AND(m_registers.C); break;
		case 0xA2: m_alu.AND(m_registers.D); break;
		case 0xA3: m_alu.AND(m_registers.E); break;
		case 0xA4: m_alu.AND(m_registers.H); break;
		case 0xA5: m_alu.AND(m_registers.L); break;
		case 0xA6: m_alu.AND(m_mmu.read(m_registers.HL), 4); break;
		case 0xE6: m_alu.AND(m_mmu.read(PC++), 4); break;
		case 0xA7: m_alu.AND(m_registers.A); break;
		case 0xB0: m_alu.OR(m_registers.B); break;
		case 0xB1: m_alu.OR(m_registers.C); break;
		case 0xB2: m_alu.OR(m_registers.D); break;
		case 0xB3: m_alu.OR(m_registers.E); break;
		case 0xB4: m_alu.OR(m_registers.H); break;
		case 0xB5: m_alu.OR(m_registers.L); break;
		case 0xB6: m_alu.OR(m_mmu.read(m_registers.HL), 4); break;
		case 0xF6: m_alu.OR(m_mmu.read(PC++), 4); break;
		case 0xB7: m_alu.OR(m_registers.A); break;
		case 0xA8: m_alu.XOR(m_registers.B); break;
		case 0xA9: m_alu.XOR(m_registers.C); break;
		case 0xAA: m_alu.XOR(m_registers.D); break;
		case 0xAB: m_alu.XOR(m_registers.E); break;
		case 0xAC: m_alu.XOR(m_registers.H); break;
		case 0xAD: m_alu.XOR(m_registers.L); break;
		case 0xAE: m_alu.XOR(m_mmu.read(m_registers.HL), 4); break;
		case 0xEE: m_alu.XOR(m_mmu.read(PC++), 4); break;
		case 0xAF: m_alu.XOR(m_registers.A); break;
		case 0xB8: m_alu.CP(m_registers.B); break;
		case 0xB9: m_alu.CP(m_registers.C); break;
		case 0xBA: m_alu.CP(m_registers.D); break;
		case 0xBB: m_alu.CP(m_registers.E); break;
		case 0xBC: m_alu.CP(m_registers.H); break;
		case 0xBD: m_alu.CP(m_registers.L); break;
		case 0xBE: m_alu.CP(m_mmu.read(m_registers.HL), 4); break;
		case 0xFE: m_alu.CP(m_mmu.read(PC++), 4); break;
		case 0xBF: m_alu.CP(m_registers.A); break;
		case 0x80: m_alu.ADD(m_registers.A, m_registers.B); break;
		case 0x81: m_alu.ADD(m_registers.A, m_registers.C); break;
		case 0x82: m_alu.ADD(m_registers.A, m_registers.D); break;
		case 0x83: m_alu.ADD(m_registers.A, m_registers.E); break;
		case 0x84: m_alu.ADD(m_registers.A, m_registers.H); break;
		case 0x85: m_alu.ADD(m_registers.A, m_registers.L); break;
		case 0x86: m_alu.ADD(m_registers.A, m_mmu.read(m_registers.HL), 4); break;
		case 0xC6: m_alu.ADD(m_registers.A, m_mmu.read(PC++), 4); break;
		case 0x87: m_alu.ADD(m_registers.A, m_registers.A); break;
		case 0x88: m_alu.ADC(m_registers.A, m_registers.B); break;
		case 0x89: m_alu.ADC(m_registers.A, m_registers.C); break;
		case 0x8A: m_alu.ADC(m_registers.A, m_registers.D); break;
		case 0x8B: m_alu.ADC(m_registers.A, m_registers.E); break;
		case 0x8C: m_alu.ADC(m_registers.A, m_registers.H); break;
		case 0x8D: m_alu.ADC(m_registers.A, m_registers.L); break;
		case 0x8E: m_alu.ADC(m_registers.A, m_mmu.read(m_registers.HL), 4); break;
		case 0xCE: m_alu.ADC(m_registers.A, m_mmu.read(PC++), 4); break;
		case 0x8F: m_alu.ADC(m_registers.A, m_registers.A); break;
		case 0x90: m_alu.SUB(m_registers.A, m_registers.B); break;
		case 0x91: m_alu.SUB(m_registers.A, m_registers.C); break;
		case 0x92: m_alu.SUB(m_registers.A, m_registers.D); break;
		case 0x93: m_alu.SUB(m_registers.A, m_registers.E); break;
		case 0x94: m_alu.SUB(m_registers.A, m_registers.H); break;
		case 0x95: m_alu.SUB(m_registers.A, m_registers.L); break;
		case 0x96: m_alu.SUB(m_registers.A, m_mmu.read(m_registers.HL), 4); break;
		case 0xD6: m_alu.SUB(m_registers.A, m_mmu.read(PC++), 4); break;
		case 0x97: m_alu.SUB(m_registers.A, m_registers.A); break;
		case 0x98: m_alu.SBC(m_registers.A, m_registers.B); break;
		case 0x99: m_alu.SBC(m_registers.A, m_registers.C); break;
		case 0x9A: m_alu.SBC(m_registers.A, m_registers.D); break;
		case 0x9B: m_alu.SBC(m_registers.A, m_registers.E); break;
		case 0x9C: m_alu.SBC(m_registers.A, m_registers.H); break;
		case 0x9D: m_alu.SBC(m_registers.A, m_registers.L); break;
		case 0x9E: m_alu.SBC(m_registers.A, m_mmu.read(m_registers.HL), 4); break;
		case 0xDE: m_alu.SBC(m_registers.A, m_mmu.read(PC++), 4); break;
		case 0x9F: m_alu.SBC(m_registers.A, m_registers.A); break;

			// LD 16-bit
		case 0x01: LD(m_registers.BC, word_(m_mmu.read(PC++), m_mmu.read(PC++)), 8); break;
		case 0x11: LD(m_registers.DE, word_(m_mmu.read(PC++), m_mmu.read(PC++)), 8); break;
		case 0x21: LD(m_registers.HL, word_(m_mmu.read(PC++), m_mmu.read(PC++)), 8); break;
		case 0x31: LD(SP, word_(m_mmu.read(PC++), m_mmu.read(PC++)), 8); break;
		case 0xF8: LD_HL_SP_r8(static_cast<int8_t>(m_mmu.read(PC++))); break;
		case 0xF9: LD(SP, m_registers.HL, 4); break;
		case 0x08:
		{
			word a16 = word_(m_mmu.read(PC++), m_mmu.read(PC++));
			m_mmu.write(a16 + 0, lsb(SP));
			m_mmu.write(a16 + 1, msb(SP));
			CLOCK += 16;
		} break;

		// LD 8-bit
		case 0xEA: LD_ADDR(word_(m_mmu.read(PC++), m_mmu.read(PC++)), m_registers.A, 8);
		case 0xFA: LD(m_registers.A, m_mmu.read(word_(m_mmu.read(PC++), m_mmu.read(PC++))), 12); break;
			// LD B, x
		case 0x40: LD(m_registers.B, m_registers.B); break;
		case 0x41: LD(m_registers.B, m_registers.C); break;
		case 0x42: LD(m_registers.B, m_registers.D); break;
		case 0x43: LD(m_registers.B, m_registers.E); break;
		case 0x44: LD(m_registers.B, m_registers.H); break;
		case 0x45: LD(m_registers.B, m_registers.L); break;
		case 0x46: LD(m_registers.B, m_mmu.read(m_registers.HL), 4); break;
		case 0x47: LD(m_registers.B, m_registers.A); break;
			// LD C, x
		case 0x48: LD(m_registers.C, m_registers.B); break;
		case 0x49: LD(m_registers.C, m_registers.C); break;
		case 0x4A: LD(m_registers.C, m_registers.D); break;
		case 0x4B: LD(m_registers.C, m_registers.E); break;
		case 0x4C: LD(m_registers.C, m_registers.H); break;
		case 0x4D: LD(m_registers.C, m_registers.L); break;
		case 0x4E: LD(m_registers.C, m_mmu.read(m_registers.HL), 4); break;
		case 0x4F: LD(m_registers.C, m_registers.A); break;
			// LD D, x
		case 0x50: LD(m_registers.D, m_registers.B); break;
		case 0x51: LD(m_registers.D, m_registers.C); break;
		case 0x52: LD(m_registers.D, m_registers.D); break;
		case 0x53: LD(m_registers.D, m_registers.E); break;
		case 0x54: LD(m_registers.D, m_registers.H); break;
		case 0x55: LD(m_registers.D, m_registers.L); break;
		case 0x56: LD(m_registers.D, m_mmu.read(m_registers.HL), 4); break;
		case 0x57: LD(m_registers.D, m_registers.A); break;
			// LD E, x
		case 0x58: LD(m_registers.E, m_registers.B); break;
		case 0x59: LD(m_registers.E, m_registers.C); break;
		case 0x5A: LD(m_registers.E, m_registers.D); break;
		case 0x5B: LD(m_registers.E, m_registers.E); break;
		case 0x5C: LD(m_registers.E, m_registers.H); break;
		case 0x5D: LD(m_registers.E, m_registers.L); break;
		case 0x5E: LD(m_registers.E, m_mmu.read(m_registers.HL), 4); break;
		case 0x5F: LD(m_registers.E, m_registers.A); break;
			// LD H, x
		case 0x60: LD(m_registers.H, m_registers.B); break;
		case 0x61: LD(m_registers.H, m_registers.C); break;
		case 0x62: LD(m_registers.H, m_registers.D); break;
		case 0x63: LD(m_registers.H, m_registers.E); break;
		case 0x64: LD(m_registers.H, m_registers.H); break;
		case 0x65: LD(m_registers.H, m_registers.L); break;
		case 0x66: LD(m_registers.H, m_mmu.read(m_registers.HL), 4); break;
		case 0x67: LD(m_registers.H, m_registers.A); break;
			// LD L, x
		case 0x68: LD(m_registers.L, m_registers.B); break;
		case 0x69: LD(m_registers.L, m_registers.C); break;
		case 0x6A: LD(m_registers.L, m_registers.D); break;
		case 0x6B: LD(m_registers.L, m_registers.E); break;
		case 0x6C: LD(m_registers.L, m_registers.H); break;
		case 0x6D: LD(m_registers.L, m_registers.L); break;
		case 0x6E: LD(m_registers.L, m_mmu.read(m_registers.HL), 4); break;
		case 0x6F: LD(m_registers.L, m_registers.A); break;
			// LD HL, x
		case 0x70: LD(m_registers.HL, m_registers.B); break;
		case 0x71: LD(m_registers.HL, m_registers.C); break;
		case 0x72: LD(m_registers.HL, m_registers.D); break;
		case 0x73: LD(m_registers.HL, m_registers.E); break;
		case 0x74: LD(m_registers.HL, m_registers.H); break;
		case 0x75: LD(m_registers.HL, m_registers.L); break;
		case 0x77: LD(m_registers.HL, m_registers.A); break;
			// LD A, x
		case 0x78: LD(m_registers.A, m_registers.B); break;
		case 0x79: LD(m_registers.A, m_registers.C); break;
		case 0x7A: LD(m_registers.A, m_registers.D); break;
		case 0x7B: LD(m_registers.A, m_registers.E); break;
		case 0x7C: LD(m_registers.A, m_registers.H); break;
		case 0x7D: LD(m_registers.A, m_registers.L); break;
		case 0x7E: LD(m_registers.A, m_mmu.read(m_registers.HL), 4); break;
		case 0x7F: LD(m_registers.A, m_registers.A); break;
		case 0x02: LD_ADDR(m_registers.BC, m_registers.A); break;
		case 0x12: LD_ADDR(m_registers.DE, m_registers.A); break;
		case 0x22: LD_ADDR(m_registers.HL, m_registers.A); m_registers.HL++; break;
		case 0x32: LD_ADDR(m_registers.HL, m_registers.A); m_registers.HL--; break;
		case 0x06: LD(m_registers.B, m_mmu.read(PC++), 4); break;
		case 0x16: LD(m_registers.D, m_mmu.read(PC++), 4); break;
		case 0x26: LD(m_registers.H, m_mmu.read(PC++), 4); break;
		case 0x36: LD_ADDR(m_registers.HL, m_mmu.read(PC++), 4); break;
		case 0x0A: LD(m_registers.A, m_mmu.read(m_registers.BC), 4); break;
		case 0x1A: LD(m_registers.A, m_mmu.read(m_registers.DE), 4); break;
		case 0x2A: LD(m_registers.A, m_mmu.read(m_registers.HL), 4); m_registers.HL++; break;
		case 0x3A: LD(m_registers.A, m_mmu.read(m_registers.HL), 4); m_registers.HL--; break;
		case 0x0E: LD(m_registers.C, m_mmu.read(PC++), 4); break;
		case 0x1E: LD(m_registers.E, m_mmu.read(PC++), 4); break;
		case 0x2E: LD(m_registers.L, m_mmu.read(PC++), 4); break;
		case 0x3E: LD(m_registers.A, m_mmu.read(PC++), 4); break;
		case 0xE0: LD_ADDR(0xFF00 + m_mmu.read(PC++), m_registers.A, 4); break;
		case 0xF0: LD(m_registers.A, m_mmu.read(0xFF00 + m_mmu.read(PC++)), 8); break;
		case 0xE2: LD_ADDR(0xFF00 + m_registers.C, m_registers.A); break;
		case 0xF2: LD(m_registers.A, m_mmu.read(0xFF00 + m_registers.C), 4); break;

			// POP
		case 0xC1: m_registers.BC = word_(m_mmu.read(SP++), m_mmu.read(SP++)); m_state.CLOCK += 8; break;
		case 0xD1: m_registers.DE = word_(m_mmu.read(SP++), m_mmu.read(SP++)); m_state.CLOCK += 8; break;
		case 0xE1: m_registers.HL = word_(m_mmu.read(SP++), m_mmu.read(SP++)); m_state.CLOCK += 8; break;
		case 0xF1: m_registers.AF = word_(m_mmu.read(SP++), m_mmu.read(SP++)); m_state.CLOCK += 8; break;
			// PUSH
		case 0xC5: m_mmu.write(--SP, msb(m_registers.BC)); m_mmu.write(--SP, lsb(m_registers.BC)); m_state.CLOCK += 12; break;
		case 0xD5: m_mmu.write(--SP, msb(m_registers.DE)); m_mmu.write(--SP, lsb(m_registers.DE)); m_state.CLOCK += 12; break;
		case 0xE5: m_mmu.write(--SP, msb(m_registers.HL)); m_mmu.write(--SP, lsb(m_registers.HL)); m_state.CLOCK += 12; break;
		case 0xF5: m_mmu.write(--SP, msb(m_registers.AF)); m_mmu.write(--SP, lsb(m_registers.AF)); m_state.CLOCK += 12; break;

			// JP nn
		case 0xC3:
		{
			word nn = word_(m_mmu.read(PC++), m_mmu.read(PC++));
			PC = nn;
			CLOCK += 12;
		} break;
		// JP (HL)
		case 0xE9:
		{
			PC = m_registers.HL;
		} break;
		// JP cc, nn
		case 0xC2:
		case 0xD2:
		case 0xCA:
		case 0xDA:
		{
			word nn = word_(m_mmu.read(PC++), m_mmu.read(PC++));
			CLOCK += 8;
			if (m_registers.checkFlag(op))
			{
				PC = nn;
				CLOCK += 4;
			}
		} break;
		// JR r
		case 0x18:
		{
			int8_t r = static_cast<int8_t>(m_mmu.read(PC++));
			PC = PC + r;
			CLOCK += 8;
		} break;
		// JR cc, r
		case 0x20:
		case 0x30:
		case 0x28:
		case 0x38:
		{
			int8_t r = static_cast<int8_t>(m_mmu.read(PC++));
			CLOCK += 4;
			if (m_registers.checkFlag(op))
			{
				PC = PC + r;
				CLOCK += 4;
			}
		} break;
		// CALL nn
		case 0xCD:
		{
			word nn = word_(m_mmu.read(PC++), m_mmu.read(PC++));
			m_mmu.write(--SP, msb(PC));
			m_mmu.write(--SP, lsb(PC));
			PC = nn;
			CLOCK += 20;
		} break;
		// CALL cc, nn
		case 0xC4:
		case 0xD4:
		case 0xCC:
		case 0xDC:
		{
			word nn = word_(m_mmu.read(PC++), m_mmu.read(PC++));
			CLOCK += 8;
			if (m_registers.checkFlag(op))
			{
				m_mmu.write(--SP, msb(PC));
				m_mmu.write(--SP, lsb(PC));
				PC = nn;
				CLOCK += 12;
			}
		} break;
		// RET
		case 0xC9:
		{
			PC = word_(m_mmu.read(SP++), m_mmu.read(SP++));
			CLOCK += 12;
		} break;
		// RET cc
		case 0xC0:
		case 0xD0:
		case 0xC8:
		case 0xD8:
		{
			CLOCK += 4;
			if (m_registers.checkFlag(op))
			{
				PC = word_(m_mmu.read(SP++), m_mmu.read(SP++));
				CLOCK += 12;
			}
		} break;
		// RETI
		case 0xD9:
		{
			PC = word_(m_mmu.read(SP++), m_mmu.read(SP++));
			IME = 1;
			CLOCK += 12;
		} break;
		// RST n
		case 0xC7:
		case 0xD7:
		case 0xE7:
		case 0xF7:
		case 0xCF:
		case 0xDF:
		case 0xEF:
		case 0xFF:
		{
			byte n = RST(op);
			m_mmu.write(--SP, msb(PC));
			m_mmu.write(--SP, lsb(PC));
			PC = word_(n, 0x00);
			CLOCK += 12;
		} break;

		default:
			mlibc_err("CPU::OP(), error! Unknown OPCODE 0x%02zx!", op);
	}
}

void CPU::cb(byte op)
{
	m_state.CLOCK += 4;

	// Get refs to CPU registers
	auto & PC = m_registers.PC;
	auto & SP = m_registers.SP;

	// Get refs to CPU state variables
	auto & CLOCK = m_state.CLOCK;
	auto & IME = m_state.IME;
	auto & IME_scheduled = m_state.IME_scheduled;
	auto & STATE = m_state.STATE;

	// Handle the OPCODE 
	switch (op)
	{
		// RLC, RRC
		case 0x00: m_alu.RLC(m_registers.B); break;
		case 0x01: m_alu.RLC(m_registers.C); break;
		case 0x02: m_alu.RLC(m_registers.D); break;
		case 0x03: m_alu.RLC(m_registers.E); break;
		case 0x04: m_alu.RLC(m_registers.H); break;
		case 0x05: m_alu.RLC(m_registers.L); break;
		case 0x06: m_alu.RLC_ADDR(m_registers.HL); break;
		case 0x07: m_alu.RLC(m_registers.A); break;
		case 0x08: m_alu.RRC(m_registers.B); break;
		case 0x09: m_alu.RRC(m_registers.C); break;
		case 0x0A: m_alu.RRC(m_registers.D); break;
		case 0x0B: m_alu.RRC(m_registers.E); break;
		case 0x0C: m_alu.RRC(m_registers.H); break;
		case 0x0D: m_alu.RRC(m_registers.L); break;
		case 0x0E: m_alu.RRC_ADDR(m_registers.HL); break;
		case 0x0F: m_alu.RRC(m_registers.A); break;
			// RL, RR
		case 0x10: m_alu.RL(m_registers.B); break;
		case 0x11: m_alu.RL(m_registers.C); break;
		case 0x12: m_alu.RL(m_registers.D); break;
		case 0x13: m_alu.RL(m_registers.E); break;
		case 0x14: m_alu.RL(m_registers.H); break;
		case 0x15: m_alu.RL(m_registers.L); break;
		case 0x16: m_alu.RL_ADDR(m_registers.HL); break;
		case 0x17: m_alu.RL(m_registers.A); break;
		case 0x18: m_alu.RR(m_registers.B); break;
		case 0x19: m_alu.RR(m_registers.C); break;
		case 0x1A: m_alu.RR(m_registers.D); break;
		case 0x1B: m_alu.RR(m_registers.E); break;
		case 0x1C: m_alu.RR(m_registers.H); break;
		case 0x1D: m_alu.RR(m_registers.L); break;
		case 0x1E: m_alu.RR_ADDR(m_registers.HL); break;
		case 0x1F: m_alu.RR(m_registers.A); break;
			// SLA, SRA
		case 0x20: m_alu.SLA(m_registers.B); break;
		case 0x21: m_alu.SLA(m_registers.C); break;
		case 0x22: m_alu.SLA(m_registers.D); break;
		case 0x23: m_alu.SLA(m_registers.E); break;
		case 0x24: m_alu.SLA(m_registers.H); break;
		case 0x25: m_alu.SLA(m_registers.L); break;
		case 0x26: m_alu.SLA_ADDR(m_registers.HL); break;
		case 0x27: m_alu.SLA(m_registers.A); break;
		case 0x28: m_alu.SRA(m_registers.B); break;
		case 0x29: m_alu.SRA(m_registers.C); break;
		case 0x2A: m_alu.SRA(m_registers.D); break;
		case 0x2B: m_alu.SRA(m_registers.E); break;
		case 0x2C: m_alu.SRA(m_registers.H); break;
		case 0x2D: m_alu.SRA(m_registers.L); break;
		case 0x2E: m_alu.SRA_ADDR(m_registers.HL); break;
		case 0x2F: m_alu.SRA(m_registers.A); break;
			// SWAP, SRL
		case 0x30: m_alu.SWAP(m_registers.B); break;
		case 0x31: m_alu.SWAP(m_registers.C); break;
		case 0x32: m_alu.SWAP(m_registers.D); break;
		case 0x33: m_alu.SWAP(m_registers.E); break;
		case 0x34: m_alu.SWAP(m_registers.H); break;
		case 0x35: m_alu.SWAP(m_registers.L); break;
		case 0x36: m_alu.SWAP_ADDR(m_registers.HL); break;
		case 0x37: m_alu.SWAP(m_registers.A); break;
		case 0x38: m_alu.SRL(m_registers.B); break;
		case 0x39: m_alu.SRL(m_registers.C); break;
		case 0x3A: m_alu.SRL(m_registers.D); break;
		case 0x3B: m_alu.SRL(m_registers.E); break;
		case 0x3C: m_alu.SRL(m_registers.H); break;
		case 0x3D: m_alu.SRL(m_registers.L); break;
		case 0x3E: m_alu.SRL_ADDR(m_registers.HL); break;
		case 0x3F: m_alu.SRL(m_registers.A); break;
			// BIT 0, 1
		case 0x40: m_alu.BIT(0, m_registers.B); break;
		case 0x41: m_alu.BIT(0, m_registers.C); break;
		case 0x42: m_alu.BIT(0, m_registers.D); break;
		case 0x43: m_alu.BIT(0, m_registers.E); break;
		case 0x44: m_alu.BIT(0, m_registers.H); break;
		case 0x45: m_alu.BIT(0, m_registers.L); break;
		case 0x46: m_alu.BIT_ADDR(0, m_registers.HL); break;
		case 0x47: m_alu.BIT(0, m_registers.A); break;
		case 0x48: m_alu.BIT(1, m_registers.B); break;
		case 0x49: m_alu.BIT(1, m_registers.C); break;
		case 0x4A: m_alu.BIT(1, m_registers.D); break;
		case 0x4B: m_alu.BIT(1, m_registers.E); break;
		case 0x4C: m_alu.BIT(1, m_registers.H); break;
		case 0x4D: m_alu.BIT(1, m_registers.L); break;
		case 0x4E: m_alu.BIT_ADDR(1, m_registers.HL); break;
		case 0x4F: m_alu.BIT(1, m_registers.A); break;
			// BIT 2, 3
		case 0x50: m_alu.BIT(2, m_registers.B); break;
		case 0x51: m_alu.BIT(2, m_registers.C); break;
		case 0x52: m_alu.BIT(2, m_registers.D); break;
		case 0x53: m_alu.BIT(2, m_registers.E); break;
		case 0x54: m_alu.BIT(2, m_registers.H); break;
		case 0x55: m_alu.BIT(2, m_registers.L); break;
		case 0x56: m_alu.BIT_ADDR(2, m_registers.HL); break;
		case 0x57: m_alu.BIT(2, m_registers.A); break;
		case 0x58: m_alu.BIT(3, m_registers.B); break;
		case 0x59: m_alu.BIT(3, m_registers.C); break;
		case 0x5A: m_alu.BIT(3, m_registers.D); break;
		case 0x5B: m_alu.BIT(3, m_registers.E); break;
		case 0x5C: m_alu.BIT(3, m_registers.H); break;
		case 0x5D: m_alu.BIT(3, m_registers.L); break;
		case 0x5E: m_alu.BIT_ADDR(3, m_registers.HL); break;
		case 0x5F: m_alu.BIT(3, m_registers.A); break;
			// BIT 4, 5
		case 0x60: m_alu.BIT(4, m_registers.B); break;
		case 0x61: m_alu.BIT(4, m_registers.C); break;
		case 0x62: m_alu.BIT(4, m_registers.D); break;
		case 0x63: m_alu.BIT(4, m_registers.E); break;
		case 0x64: m_alu.BIT(4, m_registers.H); break;
		case 0x65: m_alu.BIT(4, m_registers.L); break;
		case 0x66: m_alu.BIT_ADDR(4, m_registers.HL); break;
		case 0x67: m_alu.BIT(4, m_registers.A); break;
		case 0x68: m_alu.BIT(5, m_registers.B); break;
		case 0x69: m_alu.BIT(5, m_registers.C); break;
		case 0x6A: m_alu.BIT(5, m_registers.D); break;
		case 0x6B: m_alu.BIT(5, m_registers.E); break;
		case 0x6C: m_alu.BIT(5, m_registers.H); break;
		case 0x6D: m_alu.BIT(5, m_registers.L); break;
		case 0x6E: m_alu.BIT_ADDR(5, m_registers.HL); break;
		case 0x6F: m_alu.BIT(5, m_registers.A); break;
			// BIT 6, 7
		case 0x70: m_alu.BIT(6, m_registers.B); break;
		case 0x71: m_alu.BIT(6, m_registers.C); break;
		case 0x72: m_alu.BIT(6, m_registers.D); break;
		case 0x73: m_alu.BIT(6, m_registers.E); break;
		case 0x74: m_alu.BIT(6, m_registers.H); break;
		case 0x75: m_alu.BIT(6, m_registers.L); break;
		case 0x76: m_alu.BIT_ADDR(6, m_registers.HL); break;
		case 0x77: m_alu.BIT(6, m_registers.A); break;
		case 0x78: m_alu.BIT(7, m_registers.B); break;
		case 0x79: m_alu.BIT(7, m_registers.C); break;
		case 0x7A: m_alu.BIT(7, m_registers.D); break;
		case 0x7B: m_alu.BIT(7, m_registers.E); break;
		case 0x7C: m_alu.BIT(7, m_registers.H); break;
		case 0x7D: m_alu.BIT(7, m_registers.L); break;
		case 0x7E: m_alu.BIT_ADDR(7, m_registers.HL); break;
		case 0x7F: m_alu.BIT(7, m_registers.A); break;
			// RES 0, 1
		case 0x80: m_alu.RES(0, m_registers.B); break;
		case 0x81: m_alu.RES(0, m_registers.C); break;
		case 0x82: m_alu.RES(0, m_registers.D); break;
		case 0x83: m_alu.RES(0, m_registers.E); break;
		case 0x84: m_alu.RES(0, m_registers.H); break;
		case 0x85: m_alu.RES(0, m_registers.L); break;
		case 0x86: m_alu.RES_ADDR(0, m_registers.HL); break;
		case 0x87: m_alu.RES(0, m_registers.A); break;
		case 0x88: m_alu.RES(1, m_registers.B); break;
		case 0x89: m_alu.RES(1, m_registers.C); break;
		case 0x8A: m_alu.RES(1, m_registers.D); break;
		case 0x8B: m_alu.RES(1, m_registers.E); break;
		case 0x8C: m_alu.RES(1, m_registers.H); break;
		case 0x8D: m_alu.RES(1, m_registers.L); break;
		case 0x8E: m_alu.RES_ADDR(1, m_registers.HL); break;
		case 0x8F: m_alu.RES(1, m_registers.A); break;
			// RES 2, 3
		case 0x90: m_alu.RES(2, m_registers.B); break;
		case 0x91: m_alu.RES(2, m_registers.C); break;
		case 0x92: m_alu.RES(2, m_registers.D); break;
		case 0x93: m_alu.RES(2, m_registers.E); break;
		case 0x94: m_alu.RES(2, m_registers.H); break;
		case 0x95: m_alu.RES(2, m_registers.L); break;
		case 0x96: m_alu.RES_ADDR(2, m_registers.HL); break;
		case 0x97: m_alu.RES(2, m_registers.A); break;
		case 0x98: m_alu.RES(3, m_registers.B); break;
		case 0x99: m_alu.RES(3, m_registers.C); break;
		case 0x9A: m_alu.RES(3, m_registers.D); break;
		case 0x9B: m_alu.RES(3, m_registers.E); break;
		case 0x9C: m_alu.RES(3, m_registers.H); break;
		case 0x9D: m_alu.RES(3, m_registers.L); break;
		case 0x9E: m_alu.RES_ADDR(3, m_registers.HL); break;
		case 0x9F: m_alu.RES(3, m_registers.A); break;
			// RES 4, 5
		case 0xA0: m_alu.RES(4, m_registers.B); break;
		case 0xA1: m_alu.RES(4, m_registers.C); break;
		case 0xA2: m_alu.RES(4, m_registers.D); break;
		case 0xA3: m_alu.RES(4, m_registers.E); break;
		case 0xA4: m_alu.RES(4, m_registers.H); break;
		case 0xA5: m_alu.RES(4, m_registers.L); break;
		case 0xA6: m_alu.RES_ADDR(4, m_registers.HL); break;
		case 0xA7: m_alu.RES(4, m_registers.A); break;
		case 0xA8: m_alu.RES(5, m_registers.B); break;
		case 0xA9: m_alu.RES(5, m_registers.C); break;
		case 0xAA: m_alu.RES(5, m_registers.D); break;
		case 0xAB: m_alu.RES(5, m_registers.E); break;
		case 0xAC: m_alu.RES(5, m_registers.H); break;
		case 0xAD: m_alu.RES(5, m_registers.L); break;
		case 0xAE: m_alu.RES_ADDR(5, m_registers.HL); break;
		case 0xAF: m_alu.RES(5, m_registers.A); break;
			// RES 6, 7
		case 0xB0: m_alu.RES(6, m_registers.B); break;
		case 0xB1: m_alu.RES(6, m_registers.C); break;
		case 0xB2: m_alu.RES(6, m_registers.D); break;
		case 0xB3: m_alu.RES(6, m_registers.E); break;
		case 0xB4: m_alu.RES(6, m_registers.H); break;
		case 0xB5: m_alu.RES(6, m_registers.L); break;
		case 0xB6: m_alu.RES_ADDR(6, m_registers.HL); break;
		case 0xB7: m_alu.RES(6, m_registers.A); break;
		case 0xB8: m_alu.RES(7, m_registers.B); break;
		case 0xB9: m_alu.RES(7, m_registers.C); break;
		case 0xBA: m_alu.RES(7, m_registers.D); break;
		case 0xBB: m_alu.RES(7, m_registers.E); break;
		case 0xBC: m_alu.RES(7, m_registers.H); break;
		case 0xBD: m_alu.RES(7, m_registers.L); break;
		case 0xBE: m_alu.RES_ADDR(7, m_registers.HL); break;
		case 0xBF: m_alu.RES(7, m_registers.A); break;
			// SET 0, 1
		case 0xC0: m_alu.SET(0, m_registers.B); break;
		case 0xC1: m_alu.SET(0, m_registers.C); break;
		case 0xC2: m_alu.SET(0, m_registers.D); break;
		case 0xC3: m_alu.SET(0, m_registers.E); break;
		case 0xC4: m_alu.SET(0, m_registers.H); break;
		case 0xC5: m_alu.SET(0, m_registers.L); break;
		case 0xC6: m_alu.SET_ADDR(0, m_registers.HL); break;
		case 0xC7: m_alu.SET(0, m_registers.A); break;
		case 0xC8: m_alu.SET(1, m_registers.B); break;
		case 0xC9: m_alu.SET(1, m_registers.C); break;
		case 0xCA: m_alu.SET(1, m_registers.D); break;
		case 0xCB: m_alu.SET(1, m_registers.E); break;
		case 0xCC: m_alu.SET(1, m_registers.H); break;
		case 0xCD: m_alu.SET(1, m_registers.L); break;
		case 0xCE: m_alu.SET_ADDR(1, m_registers.HL); break;
		case 0xCF: m_alu.SET(1, m_registers.A); break;
			// SET 2, 3
		case 0xD0: m_alu.SET(2, m_registers.B); break;
		case 0xD1: m_alu.SET(2, m_registers.C); break;
		case 0xD2: m_alu.SET(2, m_registers.D); break;
		case 0xD3: m_alu.SET(2, m_registers.E); break;
		case 0xD4: m_alu.SET(2, m_registers.H); break;
		case 0xD5: m_alu.SET(2, m_registers.L); break;
		case 0xD6: m_alu.SET_ADDR(2, m_registers.HL); break;
		case 0xD7: m_alu.SET(2, m_registers.A); break;
		case 0xD8: m_alu.SET(3, m_registers.B); break;
		case 0xD9: m_alu.SET(3, m_registers.C); break;
		case 0xDA: m_alu.SET(3, m_registers.D); break;
		case 0xDB: m_alu.SET(3, m_registers.E); break;
		case 0xDC: m_alu.SET(3, m_registers.H); break;
		case 0xDD: m_alu.SET(3, m_registers.L); break;
		case 0xDE: m_alu.SET_ADDR(3, m_registers.HL); break;
		case 0xDF: m_alu.SET(3, m_registers.A); break;
			// SET 4, 5
		case 0xE0: m_alu.SET(4, m_registers.B); break;
		case 0xE1: m_alu.SET(4, m_registers.C); break;
		case 0xE2: m_alu.SET(4, m_registers.D); break;
		case 0xE3: m_alu.SET(4, m_registers.E); break;
		case 0xE4: m_alu.SET(4, m_registers.H); break;
		case 0xE5: m_alu.SET(4, m_registers.L); break;
		case 0xE6: m_alu.SET_ADDR(4, m_registers.HL); break;
		case 0xE7: m_alu.SET(4, m_registers.A); break;
		case 0xE8: m_alu.SET(5, m_registers.B); break;
		case 0xE9: m_alu.SET(5, m_registers.C); break;
		case 0xEA: m_alu.SET(5, m_registers.D); break;
		case 0xEB: m_alu.SET(5, m_registers.E); break;
		case 0xEC: m_alu.SET(5, m_registers.H); break;
		case 0xED: m_alu.SET(5, m_registers.L); break;
		case 0xEE: m_alu.SET_ADDR(5, m_registers.HL); break;
		case 0xEF: m_alu.SET(5, m_registers.A); break;
			// SET 6, 7
		case 0xF0: m_alu.SET(6, m_registers.B); break;
		case 0xF1: m_alu.SET(6, m_registers.C); break;
		case 0xF2: m_alu.SET(6, m_registers.D); break;
		case 0xF3: m_alu.SET(6, m_registers.E); break;
		case 0xF4: m_alu.SET(6, m_registers.H); break;
		case 0xF5: m_alu.SET(6, m_registers.L); break;
		case 0xF6: m_alu.SET_ADDR(6, m_registers.HL); break;
		case 0xF7: m_alu.SET(6, m_registers.A); break;
		case 0xF8: m_alu.SET(7, m_registers.B); break;
		case 0xF9: m_alu.SET(7, m_registers.C); break;
		case 0xFA: m_alu.SET(7, m_registers.D); break;
		case 0xFB: m_alu.SET(7, m_registers.E); break;
		case 0xFC: m_alu.SET(7, m_registers.H); break;
		case 0xFD: m_alu.SET(7, m_registers.L); break;
		case 0xFE: m_alu.SET_ADDR(7, m_registers.HL); break;
		case 0xFF: m_alu.SET(7, m_registers.A); break;

		default:
			mlibc_err("CPU::CB(), error! Unknown OPCODE 0x%02zx!", op);
	}

	// Set state back to normal
	m_state.STATE = CPUState::NORMAL;
}

void CPU::LD(word & val, word n, int c)
{
	val = n;

	m_state.CLOCK += c;
}

void CPU::LD(byte & val, byte n, int c)
{
	val = n;

	m_state.CLOCK += c;
}

void CPU::LD_ADDR(word addr, byte n, int c)
{
	m_mmu.write(addr, n);

	m_state.CLOCK += 4;
	m_state.CLOCK += c;
}

void CPU::LD_HL_SP_r8(int8_t n)
{
	word result = m_registers.HL + m_registers.SP + n;

	m_registers.clearZ();
	m_registers.clearN();
	m_registers.testH((m_registers.SP & 0x0FFF) == 0x0FFF);
	m_registers.testC((m_registers.SP & 0xFFFF) == 0xFFFF);

	m_registers.HL = result;

	m_state.CLOCK += 8;
}

byte CPU::RST(byte op)
{
	switch (op)
	{
		case 0xC7: return 0x00;
		case 0xD7: return 0x10;
		case 0xE7: return 0x20;
		case 0xF7: return 0x30;
		case 0xCF: return 0x08;
		case 0xDF: return 0x18;
		case 0xEF: return 0x28;
		case 0xFF: return 0x38;
	}

	mlibc_err("CPU::RST(0x%02zx), error! Attempted to map a non-RST opcode to a pointer!", op);

	return 0x0000;
}

MMU & CPU::getMMU()
{
	return m_mmu;
}

CPURegisters & CPU::getRegisters()
{
	return m_registers;
}

CPUState & CPU::getState()
{
	return m_state;
}

ALU & CPU::getALU()
{
	return m_alu;
}

std::vector<word> & CPU::getBreakpoints()
{
	return m_breakpoints;
}

}