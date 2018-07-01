#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <cstdint>

typedef uint8_t byte;
typedef uint16_t word;

inline word word_(byte lsb, byte msb)
{
	return static_cast<word>((lsb << 8) + msb);
}

inline byte msb(word value)
{
	return static_cast<byte>((value & 0xff00) >> 8);
}

inline byte lsb(word value)
{
	return static_cast<byte>(value & 0x00ff);
}

#endif // DATA_TYPES_H