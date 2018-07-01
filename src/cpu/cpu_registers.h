#ifndef CPU_REGISTERS_H
#define CPU_REGISTERS_H

#include "data_types.h"

namespace hgb
{

// Status flags register bitmasks
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
	// Accumulator and Status flags, access as 16-bit uint or individual 8-bit uints
	union
	{
		struct
		{
			byte A, F;
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
			byte B, C;
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
			byte D, E;
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
			byte H, L;
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
				result = (F & CPUR_F_Z) != CPUR_F_Z;
			} break;
			// NC (check if C is not set)
			case 0xD2:
			case 0x30:
			case 0xD4:
			case 0xD0:
			{
				result = (F & CPUR_F_C) != CPUR_F_C;
			} break;
			// Z (check if Z is set)
			case 0xCA:
			case 0x28:
			case 0xCC:
			case 0xC8:
			{
				result = (F & CPUR_F_Z) == CPUR_F_Z;
			} break;
			// C (check if C is set)
			case 0xDA:
			case 0x38:
			case 0xDC:
			case 0xD8:
			{
				return (F & CPUR_F_C) == CPUR_F_C;
			} break;
		}

		return result;
	}
};

}

#endif // CPU_REGISTERS_H