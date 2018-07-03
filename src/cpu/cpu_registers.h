#ifndef CPU_REGISTERS_H
#define CPU_REGISTERS_H

#include "data_types.h"

namespace hgb
{

// Flag register bitmasks
#define CPUR_F_Z	0b10000000	// zero flag
#define CPUR_F_ZN	0b01111111	// zero flag inverted
#define CPUR_F_N	0b01000000	// subtract flag
#define CPUR_F_NN	0b10111111	// subtract flag inverted
#define CPUR_F_H	0b00100000	// half-carry flag
#define CPUR_F_HN	0b11011111	// half-carry flag inverted
#define CPUR_F_C	0b00010000	// carry flag
#define CPUR_F_CN	0b11101111	// carry flag inverted
#define CPUR_F_UU	0b00001111	// unused flags, always 0 !
#define CPUR_F_UUN	0b11110000	// unused flags, always 0 ! inverted

struct CPURegisters
{
	// Accumulator and status flags, access as 16-bit uint or individual 8-bit uints
	union
	{
		struct
		{
			byte F, A;
		};
		struct
		{
			word AF;
		};
	};

	// General purpose register(s) B and C, access as 16-bit uint or individual 8-bit uints
	union
	{
		struct
		{
			byte C, B;
		};
		struct
		{
			word BC;
		};
	};

	// General purpose register(s) D and E, access as 16-bit uint or individual 8-bit uints
	union
	{
		struct
		{
			byte E, D;
		};
		struct
		{
			word DE;
		};
	};

	// General purpose register(s) H and L, access as 16-bit uint or individual 8-bit uints
	union
	{
		struct
		{
			byte L, H;
		};
		struct
		{
			word HL;
		};
	};

	// Stack pointer, access as 16-bit uint
	word SP;

	// Program counter, access as 16-bit uint
	word PC;

	inline void setZ()
	{
		F = F | CPUR_F_Z;
	}

	inline void setN()
	{
		F = F | CPUR_F_N;
	}

	inline void setH()
	{
		F = F | CPUR_F_H;
	}

	inline void setC()
	{
		F = F | CPUR_F_C;
	}

	inline void clearZ()
	{
		F = F & CPUR_F_ZN;
	}

	inline void clearN()
	{
		F = F & CPUR_F_NN;
	}

	inline void clearH()
	{
		F = F & CPUR_F_HN;
	}

	inline void clearC()
	{
		F = F & CPUR_F_CN;
	}

	inline void flipZ()
	{
		F = ~(F & CPUR_F_Z) & F;
	}

	inline void flipN()
	{
		F = ~(F & CPUR_F_N) & F;
	}

	inline void flipH()
	{
		F = ~(F & CPUR_F_H) & F;
	}

	inline void flipC()
	{
		F = ~(F & CPUR_F_C) & F;
	}

	inline bool checkZ()
	{
		return (F & CPUR_F_Z) != 0x00;
	}

	inline bool checkN()
	{
		return (F & CPUR_F_N) > 0x00;
	}

	inline bool checkH()
	{
		return (F & CPUR_F_H) > 0x00;
	}

	inline bool checkC()
	{
		return (F & CPUR_F_C) > 0x00;
	}

	// Check flag condition based on OPCODE
	inline bool checkFlag(word op)
	{
		bool result = false;

		switch (op)
		{
			// NZ (check if Z is not set)
			case 0xC2:
			case 0x20:
			case 0xC4:
			case 0xC0:
			{
				return !checkZ();
			} break;
			// NC (check if C is not set)
			case 0xD2:
			case 0x30:
			case 0xD4:
			case 0xD0:
			{
				return !checkC();
			} break;
			// Z (check if Z is set)
			case 0xCA:
			case 0x28:
			case 0xCC:
			case 0xC8:
			{
				return checkZ();
			} break;
			// C (check if C is set)
			case 0xDA:
			case 0x38:
			case 0xDC:
			case 0xD8:
			{
				return checkC();
			} break;
		}

		return result;
	}
};

}

#endif // CPU_REGISTERS_H